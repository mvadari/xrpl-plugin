from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    fix1543,
    fix1571,
    preflight1,
    preflight2,
    tf_universal_mask,
)
from xrpl_plugin.ledger_objects import (
    adjust_owner_count,
    make_sle,
)
from xrpl_plugin.rippled_py.stypes.parser_errors import bad_type, invalid_data
from xrpl_plugin.models import (
    Amendment,
    InnerObject,
    InvariantCheck,
    LedgerObject,
    SType,
    TERCode,
    Transactor,
)
from xrpl_plugin.basic_types import (
    zero_amount,
    VoteBehavior,
    XRPAmount,
    Slice,
    parse_base58,
    soeOPTIONAL,
    soeREQUIRED,
    AccountID,
)
from xrpl_plugin.keylets import Keylet, account_keylet
from xrpl_plugin.return_codes import (
    tecDIR_FULL,
    tecINSUFFICIENT_RESERVE,
    tecINTERNAL,
    tecNO_DST,
    tecNO_PERMISSION,
    tecUNFUNDED,
    temBAD_AMOUNT,
    temBAD_EXPIRATION,
    temDISABLED,
    temMALFORMED,
    tesSUCCESS,
)
from xrpl_plugin.sfields import (
    sf_account,
    sf_amount,
    sf_balance,
    sf_cancel_after,
    sf_condition,
    sf_destination_node,
    sf_destination_tag,
    sf_owner_count,
    sf_owner_node,
    sf_previous_txn_id,
    sf_previous_txn_lgr_seq,
    sf_source_tag,
    sf_ticket_sequence,
    construct_custom_sfield,
    create_new_sfield,
)
from xrpl_plugin.stypes import (
    STAmount,
    STArray,
    STObject,
    STUInt32,
    index_hash,
)

sf_finish_after2 = create_new_sfield(STUInt32, "FinishAfter2", 53)
sf_fake_array = create_new_sfield(STArray, "FakeArray", 13)
sf_fake_element = create_new_sfield(STObject, "FakeElement", 17)

STI_ACCOUNT2 = 28


def parse_account2(field, json_name, field_name, _name, value):
    if not value.is_string():
        return bad_type(json_name, field_name)
    str_value = value.as_string()
    try:
        account = AccountID()
        if account.parse_hex(str_value):
            return account.to_buffer()

        if result := parse_base58(str_value):
            return result.to_buffer()
        return invalid_data(json_name, field_name)
    except Exception as err:
        print("Error in parsing Account2:", err)
        return invalid_data(json_name, field_name)


def to_string(buf):
    return AccountID.from_buffer(buf).to_base58()


def to_serializer(buf, serializer):
    serializer.add_vl(Slice.from_buffer(buf))


def from_serial_iter(sit):
    return sit.get_vl_buffer()


sf_destination2 = construct_custom_sfield(STI_ACCOUNT2, "Destination2", 1)

stypes = [
    SType(
        type_id=STI_ACCOUNT2,
        parse_value=parse_account2,
        to_string=to_string,
        to_serializer=to_serializer,
        from_serial_iter=from_serial_iter,
    )
]
sfields = [sf_finish_after2, sf_destination2, sf_fake_array, sf_fake_element]


ltNEW_ESCROW = 0x74
NEW_ESCROW_NAMESPACE = ord("t")


def visit_entry_xrp_change_escrow(is_delete, entry, is_before):
    if is_before:
        return -1 * entry[sf_amount].xrp().drops
    if is_delete:
        return 0
    return entry[sf_amount].xrp().drops


class NoZeroEscrow(InvariantCheck):
    def __init__(self):
        self.bad = False

    def visit_entry(self, is_delete, before, after):
        def is_bad(amount):
            if not amount.native():
                return True
            if amount.xrp() <= XRPAmount(0):
                return True
            if amount.xrp() > XRPAmount(100_000_000_000 * 1_000_000):
                return True
            return False

        if before is not None and before.get_type() == ltNEW_ESCROW:
            self.bad |= is_bad(before[sf_amount])

        if after is not None and after.get_type() == ltNEW_ESCROW:
            self.bad |= is_bad(after[sf_amount])

    def finalize(self, tx, result, fee, view, j):
        if self.bad:
            print("Invariant failed: new escrow specifies invalid amount")
            return False

        return True


ledger_objects = [
    LedgerObject(
        object_type=ltNEW_ESCROW,
        name="NewEscrow",
        rpc_name="new_escrow",
        object_format=[
            (sf_account, soeREQUIRED),
            (sf_destination2, soeREQUIRED),
            (sf_amount, soeREQUIRED),
            (sf_condition, soeOPTIONAL),
            (sf_cancel_after, soeOPTIONAL),
            (sf_finish_after2, soeOPTIONAL),
            (sf_source_tag, soeOPTIONAL),
            (sf_destination_tag, soeOPTIONAL),
            (sf_owner_node, soeREQUIRED),
            (sf_previous_txn_id, soeREQUIRED),
            (sf_previous_txn_lgr_seq, soeREQUIRED),
            (sf_destination_node, soeOPTIONAL),
        ],
        is_deletion_blocker=True,
        visit_entry_xrp_change=visit_entry_xrp_change_escrow,
    )
]

invariant_checks = [NoZeroEscrow]

amendment = Amendment("featurePluginTest2", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]


def new_escrow_keylet(src, seq):
    return Keylet(ltNEW_ESCROW, index_hash(NEW_ESCROW_NAMESPACE, src, seq))


def after(now, mark):
    return now.time_since_epoch() > mark


temINVALID_FLAG2 = -256
ter_codes = [TERCode(temINVALID_FLAG2, "temINVALID_FLAG2", "Test code")]


def preflight(ctx):
    print("This is a Python plugin")
    if not ctx.rules.enabled(amendment):
        return temDISABLED

    if ctx.rules.enabled(fix1543) and ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG2

    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    amount = ctx.tx[sf_amount]
    if not amount.is_xrp():
        return temBAD_AMOUNT

    # NOTE: uncomment this if block to test out the invariant checker
    # You shouldn't be able to create a 0-amount escrow
    if amount <= zero_amount:
        return temBAD_AMOUNT

    if not ctx.tx.is_field_present(sf_cancel_after) and not ctx.tx.is_field_present(
        sf_finish_after2
    ):
        return temBAD_EXPIRATION

    if (
        ctx.tx.is_field_present(sf_cancel_after)
        and ctx.tx.is_field_present(sf_finish_after2)
        and ctx.tx[sf_cancel_after] <= ctx.tx[sf_finish_after2]
    ):
        return temBAD_EXPIRATION

    if ctx.rules.enabled(fix1571):
        if not ctx.tx.is_field_present(
            sf_finish_after2
        ) and not ctx.tx.is_field_present(sf_condition):
            return temMALFORMED

    # TODO: figure out the conditions logic

    return preflight2(ctx)


def do_apply(ctx, _m_prior_balance, _m_source_balance):
    close_time = ctx.view().info().parent_close_time

    if ctx.view().rules.enabled(fix1571):
        if ctx.tx.is_field_present(sf_cancel_after) and after(
            close_time, ctx.tx[sf_cancel_after]
        ):
            print("CancelAfter is not after the last ledger close")
            return tecNO_PERMISSION
        
        if ctx.tx.is_field_present(sf_finish_after2) and after(
            close_time, ctx.tx[sf_finish_after2]
        ):
            print("FinishAfter2 is not after the last ledger close")
            return tecNO_PERMISSION

    account = ctx.tx[sf_account]
    sle = ctx.view().peek(account_keylet(account))
    if not sle:
        return tecINTERNAL

    balance = STAmount(sle[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle[sf_owner_count] + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    if balance < reserve + STAmount(ctx.tx[sf_amount]).xrp():
        return tecUNFUNDED

    dest_acct = AccountID.from_buffer(ctx.tx[sf_destination2])

    sled = ctx.view().peek(account_keylet(dest_acct))
    if not sled:
        return tecNO_DST
    # TODO: implement dest tag check

    # TODO: implement deposit auth check

    keylet = new_escrow_keylet(account, ctx.tx.get_seq_proxy().value())
    slep = make_sle(keylet)
    slep[sf_account] = account
    amount = ctx.tx[sf_amount]
    slep[sf_amount] = amount
    slep[sf_destination2] = ctx.tx[sf_destination2]
    slep[sf_finish_after2] = ctx.tx[sf_finish_after2]
    ctx.view().insert(slep)

    page = ctx.view().dir_insert(account, keylet)
    if page is None:
        return tecDIR_FULL
    slep[sf_owner_node] = page

    if (destination := dest_acct) != account:
        page2 = ctx.view().dir_insert(destination, keylet)
        if page2 is None:
            return tecDIR_FULL
        slep[sf_owner_node] = page2

    sle[sf_balance] = sle[sf_balance] - ctx.tx[sf_amount]
    adjust_owner_count(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)

    return tesSUCCESS


transactors = [
    Transactor(
        name="NewEscrowCreate",
        tx_type=47,
        tx_format=[
            (sf_destination2, soeREQUIRED),
            (sf_amount, soeREQUIRED),
            (sf_condition, soeOPTIONAL),
            (sf_cancel_after, soeOPTIONAL),
            (sf_finish_after2, soeOPTIONAL),
            (sf_destination_tag, soeOPTIONAL),
            (sf_ticket_sequence, soeOPTIONAL),
            (sf_fake_array, soeOPTIONAL),
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        do_apply=do_apply,
    )
]

inner_objects = [InnerObject(sf_fake_element, [(sf_account, soeOPTIONAL)])]
