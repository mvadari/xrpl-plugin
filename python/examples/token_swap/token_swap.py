from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    preflight2,
)

from xrpl_plugin.models import (
    Transactor,
    LedgerObject,
    Amendment,
    TERCode,
)
from xrpl_plugin.basic_types import (
    soeREQUIRED,
    VoteBehavior,
)
from xrpl_plugin.keylets import (
    account_keylet,
    trustline_keylet,
    Keylet,
)
from xrpl_plugin.return_codes import (
    tecINSUFFICIENT_RESERVE,
    tecINTERNAL,
    temBAD_AMOUNT,
    tesSUCCESS,
)

from xrpl_plugin.sfields import (
    sf_balance,
    create_new_sfield,
    sf_account,
    sf_amount,
    sf_owner_count,
)
from xrpl_plugin.stypes import (
    STAmount,
    STAccount,
    STUInt64,
    index_hash,
)

from xrpl_plugin.ledger_objects import (
    adjust_owner_count,
    make_sle,
)

sf_amount_other = create_new_sfield(STAmount, "AmountOther", 33)
sf_account_other = create_new_sfield(STAccount, "AccountOther", 35)
sf_token_swap_id = create_new_sfield(STUInt64, "TokenSwapId", 36)

sfields = [sf_amount_other, sf_account_other, sf_token_swap_id]

ltTOKEN_SWAP_PROPOSE = 0x77
NEW_TOKEN_SWAP_NAMESPACE = ord("w")

temINVALID_TOKEN_SWAP_ID = -256
ter_codes = [TERCode(temINVALID_TOKEN_SWAP_ID, "temINVALID_TOKEN_SWAP_ID", "The token swap is incorrect or has ben accepted")]

def new_token_swap_keylet(src, seq):
    return Keylet(ltTOKEN_SWAP_PROPOSE, index_hash(NEW_TOKEN_SWAP_NAMESPACE, src, seq))

def visit_entry_xrp_change_token_swap(is_delete, entry, is_before):
    if is_before:
        return -1 * entry[sf_amount].xrp().drops
    if is_delete:
        return 0
    return entry[sf_amount].xrp().drops

ledger_objects = [
    LedgerObject(
        object_type=ltTOKEN_SWAP_PROPOSE,
        name="TokenSwap",
        rpc_name="token_swap",
        object_format=[
            (sf_account, soeREQUIRED),
            (sf_amount, soeREQUIRED),
            (sf_token_swap_id, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED)
        ],
        is_deletion_blocker=True,
        visit_entry_xrp_change=visit_entry_xrp_change_token_swap,
    )
]

amendment = Amendment("featureTokenSwap", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]

def preflight_propose(ctx):
    print("Token Swap Create test preflight...")

    # TODO: check rules
    # TODO: more checks

    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT
    return preflight2(ctx)

def do_apply_propose(ctx, _mPriorBalance, _mSourceBalance):
    print("Token Swap Create test doApply...")

    amount = ctx.tx[sf_amount]
    amount_other = ctx.tx[sf_amount_other]
    account = ctx.tx[sf_account]
    account_other = ctx.tx[sf_account_other]
    sle = ctx.view().peek(account_keylet(account))
    sleA_A = ctx.view().peek(trustline_keylet(account, amount.issuer()))
    sleA_B = ctx.view().peek(trustline_keylet(account, amount_other.issuer()))
    sleB_B = ctx.view().peek(trustline_keylet(account_other, amount_other.issuer()))
    sleB_A = ctx.view().peek(trustline_keylet(account_other, amount.issuer()))

    # Check reserves
    balance = STAmount(sle[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle[sf_owner_count])
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    # Check trustlines exist
    if not sleA_A or not sleA_B or not sleB_B or not sleB_A:
        return tecINTERNAL
    
    # TODO: check that it doesn't exists!
    print("ctx.tx.get_seq_proxy().value()")
    print(ctx.tx.get_seq_proxy().value())
    token_swap_keylet = new_token_swap_keylet(account, ctx.tx.get_seq_proxy().value())
    print("token_swap_keylet")
    print(token_swap_keylet)

    # TODO: Generate Swap Id
    
    slep = make_sle(token_swap_keylet)
    slep[sf_account] = account
    print("tokenKeylet")
    print(slep)

    # TODO: Insert new Keylet
    ctx.view().insert(slep)

    # adjust_owner_count(ctx.view(), sle, 1, ctx.journal)

    # TODO: A lot more

    return tesSUCCESS

def preflight_accept(ctx):
    print("Token Swap Accept test preflight...")

    # TODO: check rules
    # TODO: more checks

    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT
    return preflight2(ctx)

def do_apply_accept(ctx, _mPriorBalance, _mSourceBalance):
    print("Token Swap Accept test doApply...")

    amount = ctx.tx[sf_amount]
    amount_other = ctx.tx[sf_amount_other]
    account = ctx.tx[sf_account]
    account_other = ctx.tx[sf_account_other]
    sle = ctx.view().peek(account_keylet(account))
    sleA_A = ctx.view().peek(trustline_keylet(account, amount.issuer()))
    sleA_B = ctx.view().peek(trustline_keylet(account, amount_other.issuer()))
    sleB_B = ctx.view().peek(trustline_keylet(account_other, amount_other.issuer()))
    sleB_A = ctx.view().peek(trustline_keylet(account_other, amount.issuer()))

    # Check reserves
    balance = STAmount(sle[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle[sf_owner_count])
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    # Check trustlines exist
    if not sleA_A or not sleA_B or not sleB_B or not sleB_A:
        return tecINTERNAL
    
    # TODO: check that it doesn't exists!
    token_swap_keylet = new_token_swap_keylet(account_other, ctx.tx.get_seq_proxy().value())
    print("token_swap_keylet")
    print(token_swap_keylet)
    sle = ctx.view().peek(token_swap_keylet)

    # Check swap id
    if not sle:
        return temINVALID_TOKEN_SWAP_ID
    
    # TODO: A lot more
    
    sleA_A[sf_balance] = sleA_A[sf_balance] + amount
    sleA_B[sf_balance] = sleA_B[sf_balance] - amount_other
    sleB_B[sf_balance] = sleB_B[sf_balance] + amount_other
    sleB_A[sf_balance] = sleB_A[sf_balance] - amount
    ctx.view().update(sleA_A)
    ctx.view().update(sleA_B)
    ctx.view().update(sleB_B)
    ctx.view().update(sleB_A)

    # TODO: delete token swap object on originating account
    # adjust_owner_count(ctx.view(), sleB_B, -1, ctx.journal)

    return tesSUCCESS

transactors = [
    Transactor(
        name="TokenSwapPropose",
        tx_type=51,
        tx_format=[
            (sf_amount, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED)
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight_propose,
        do_apply=do_apply_propose,
    ),
    Transactor(
        name="TokenSwapAccept",
        tx_type=52,
        tx_format=[
            (sf_amount, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED),
            (sf_token_swap_id, soeREQUIRED)
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight_accept,
        do_apply=do_apply_accept,
    )
]