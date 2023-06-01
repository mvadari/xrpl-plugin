from plugin_transactor import (
    tesSUCCESS,
    temINVALID_FLAG,
    preflight1,
    preflight2,
    tfUniversalMask,
    fix1543,
    zeroAmount,
    accountKeylet,
    sfAccount,
    sfAmount,
    temBAD_AMOUNT,
    sfCancelAfter,
    sfFinishAfter,
    temBAD_EXPIRATION,
    sfCondition,
    fix1571,
    temMALFORMED,
    tecNO_PERMISSION,
    tecINTERNAL,
    sfBalance,
    STAmount,
    sfOwnerCount,
    tecUNFUNDED,
    tecINSUFFICIENT_RESERVE,
    sfDestination,
    tecNO_DST,
    makeSLE,
    escrowKeylet,
    ownerDirKeylet,
    describeOwnerDir,
    tecDIR_FULL,
    sfOwnerNode,
    adjustOwnerCount,
    soeREQUIRED,
    soeOPTIONAL,
    sfDestinationTag,
    sfTicketSequence,
    STUInt32,
    STBase,
    STPluginType,
    constructCustomSField,
    bad_type,
    AccountID,
    make_stplugintype,
    sfFinishAfter,
    invalid_data,
    sfSourceTag,
    sfPreviousTxnID,
    sfPreviousTxnLgrSeq,
    sfDestinationNode,
    SField,
    SOEStyle,
    indexHash,
    Keylet,
    registerLedgerObject,
    STLedgerEntry,
    parseBase58
)
import plugin_transactor
from dataclasses import dataclass

from typing import Callable, List, Optional, Tuple

# TODO: helper function, move this to the package
def create_new_sfield(cls, field_name, field_value):
    if not issubclass(cls, STBase):
        raise Exception("SField must be of an `ST` type.")
    fn_name = f"createNewSField_{cls.__name__}"
    create_fn = getattr(plugin_transactor, fn_name, None)
    if create_fn is None:
        # NOTE: This should never be hit in prod
        # It may be hit during dev work since not everything is implemented yet
        raise Exception(
            f"`createNewSField` function does not exist for {cls.__name__}"    
        )
    return create_fn(field_value, field_name)


sfFinishAfter2 = create_new_sfield(STUInt32, "FinishAfter2", 47)

STI_ACCOUNT2 = 24

def parse_account2(field, json_name, field_name, _name, value):
    if not value.isString():
        return None, bad_type(json_name, field_name)
    str_value = value.asString()
    try:
        account = AccountID()
        if (account.parseHex(str_value)):
            return make_stplugintype(field, account.to_buffer()), None

        if result := parseBase58(str_value):
            ret = make_stplugintype(field, result.to_buffer())
            return ret, None
        return None, invalid_data(json_name, field_name)
    except Exception as err:
        print("Error in parsing Account2:", err)
        return None, invalid_data(json_name, field_name)


sfDestination2 = constructCustomSField(STI_ACCOUNT2, "Destination2", 1)

# new_stypes = [(STI_ACCOUNT2, parse_account2)]
# new_sfields = [sfFinishAfter2, sfDestination2]

@dataclass(frozen=True)
class NewLedgerObject:
    object_type: int
    object_name: str
    object_rpc_name: str
    object_format: List[Tuple[SField, SOEStyle]]
    is_deletion_blocker: bool = False
    visit_entry_xrp_change: Optional[Callable[[bool, STLedgerEntry, bool], int]] = None

    def __post_init__(self):
        registerLedgerObject(self.object_type, self.object_name, self.object_format)


ltNEW_ESCROW = 0x74
NEW_ESCROW_NAMESPACE = ord('t')


def visit_entry_xrp_change_escrow(is_delete, entry, is_before):
    if is_before:
        return -1 * entry[sfAmount].xrp().drops
    if is_delete:
        return 0
    return entry[sfAmount].xrp().drops


new_ledger_objects = [
    NewLedgerObject(
        ltNEW_ESCROW,
        "NewEscrow",
        "new_escrow",
        [
            (sfAccount,              soeREQUIRED),
            (sfDestination,          soeREQUIRED),
            (sfAmount,               soeREQUIRED),
            (sfCondition,            soeOPTIONAL),
            (sfCancelAfter,          soeOPTIONAL),
            (sfFinishAfter,          soeOPTIONAL),
            (sfSourceTag,            soeOPTIONAL),
            (sfDestinationTag,       soeOPTIONAL),
            (sfOwnerNode,            soeREQUIRED),
            (sfPreviousTxnID,        soeREQUIRED),
            (sfPreviousTxnLgrSeq,    soeREQUIRED),
            (sfDestinationNode,      soeOPTIONAL),
        ],
        True,
        visit_entry_xrp_change_escrow
    )
]

def new_escrow_keylet(src, seq):
    return Keylet(ltNEW_ESCROW, indexHash(NEW_ESCROW_NAMESPACE, src, seq))

tx_name = "NewEscrowCreate"
tx_type = 47

tx_format = [
    (sfDestination, soeREQUIRED),
    (sfAmount, soeREQUIRED),
    (sfCondition, soeOPTIONAL),
    (sfCancelAfter, soeOPTIONAL),
    (sfFinishAfter, soeOPTIONAL),
    (sfDestinationTag, soeOPTIONAL),
    (sfTicketSequence, soeOPTIONAL)
]


def after(now, mark):
    return now.time_since_epoch() > mark


def preflight(ctx):
    if ctx.rules.enabled(fix1543) and ctx.tx.getFlags() & tfUniversalMask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    amount = ctx.tx[sfAmount]
    if not amount.is_xrp():
        return temBAD_AMOUNT

    if amount < zeroAmount:
        return temBAD_AMOUNT

    if not ctx.tx.isFieldPresent(sfCancelAfter) and \
        not ctx.tx.isFieldPresent(sfFinishAfter):
        return temBAD_EXPIRATION

    if ctx.tx.isFieldPresent(sfCancelAfter) and \
        ctx.tx.isFieldPresent(sfFinishAfter) and \
            ctx.tx[sfCancelAfter] <= ctx.tx[sfFinishAfter]:
        return temBAD_EXPIRATION

    if ctx.rules.enabled(fix1571):
        if not ctx.tx.isFieldPresent(sfFinishAfter) and \
            not ctx.tx.isFieldPresent(sfCondition):
            return temMALFORMED

    # TODO: figure out the conditions logic

    return preflight2(ctx)

def preclaim(ctx):
    return tesSUCCESS

def doApply(ctx, _mPriorBalance, _mSourceBalance):
    close_time = ctx.view().info().parent_close_time

    if ctx.view().rules().enabled(fix1571):
        if ctx.tx.isFieldPresent(sfCancelAfter) and \
            after(close_time, ctx.tx[sfCancelAfter]):
            return tecNO_PERMISSION

        if ctx.tx.isFieldPresent(sfFinishAfter) and \
            after(close_time, ctx.tx[sfFinishAfter]):
            return tecNO_PERMISSION
    
    account = ctx.tx[sfAccount]
    sle = ctx.view().peek(accountKeylet(account))
    if not sle:
        return tecINTERNAL
    
    balance = STAmount(sle[sfBalance]).xrp()
    reserve = ctx.view().fees().accountReserve(sle[sfOwnerCount] + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    if balance < reserve + STAmount(ctx.tx[sfAmount]).xrp():
        return tecUNFUNDED

    dest_acct = ctx.tx[sfDestination]

    sled = ctx.view().peek(accountKeylet(dest_acct))
    if not sled:
        return tecNO_DST
    # TODO: implement dest tag check

    # TODO: implement deposit auth check

    keylet = new_escrow_keylet(account, ctx.tx.getSeqProxy().value())
    slep = makeSLE(keylet)
    slep[sfAccount] = account
    amount = ctx.tx[sfAmount]
    slep[sfAmount] = amount
    slep[sfDestination] = dest_acct
    ctx.view().insert(slep)

    page = ctx.view().dirInsert(account, keylet)
    if page is None:
        return tecDIR_FULL
    slep[sfOwnerNode] = page

    if (destination := dest_acct) != account:
        page2 = ctx.view().dirInsert(destination, keylet)
        if page2 is None:
            return tecDIR_FULL
        slep[sfOwnerNode] = page2

    sle[sfBalance] = sle[sfBalance] - ctx.tx[sfAmount]
    adjustOwnerCount(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)

    return tesSUCCESS
