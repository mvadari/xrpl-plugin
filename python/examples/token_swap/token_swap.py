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
    soeOPTIONAL,
    VoteBehavior,
)
from xrpl_plugin.keylets import (
    account_keylet,
    trustline_keylet,
    Keylet,
    owner_dir_keylet,
)
from xrpl_plugin.return_codes import (
    tecINSUFFICIENT_RESERVE,
    tecNO_LINE,
    temBAD_AMOUNT,
    temBAD_EXPIRATION,
    tecEXPIRED,
    temBAD_AMOUNT,
    tesSUCCESS,
    tecDIR_FULL,
)

from xrpl_plugin.sfields import (
    sf_balance,
    create_new_sfield,
    sf_account,
    sf_expiration,
    sf_amount,
    sf_owner_count,
    sf_owner_node,
)
from xrpl_plugin.stypes import (
    STAmount,
    STAccount,
    STUInt64,
    STBlob,
    index_hash,
)

from xrpl_plugin.ledger_objects import (
    adjust_owner_count,
    make_sle,
)

sf_amount_other = create_new_sfield(STAmount, "AmountOther", 51)
sf_account_other = create_new_sfield(STAccount, "AccountOther", 52)
sf_token_swap_id = create_new_sfield(STUInt64, "TokenSwapId", 53)

sfields = [
    sf_amount_other,
    sf_account_other,
    sf_token_swap_id,
]

ltTOKEN_SWAP_PROPOSE = 0x77
NEW_TOKEN_SWAP_NAMESPACE = ord("w")

temINVALID_TOKEN_SWAP_ID = -333
ter_codes = [
    TERCode(temINVALID_TOKEN_SWAP_ID, "temINVALID_TOKEN_SWAP_ID", "The token swap is incorrect or has ben accepted"),
]

def new_token_swap_keylet(src, seq):
    return Keylet(ltTOKEN_SWAP_PROPOSE, index_hash(NEW_TOKEN_SWAP_NAMESPACE, src, seq))

def visit_entry_xrp_change_token_swap(is_delete, entry, is_before):
    if is_before:
        return -1 * entry[sf_amount].xrp().drops
    if is_delete:
        return 0
    return entry[sf_amount].xrp().drops

def expired(expiration, now):
    return expiration <= now.time_since_epoch()

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
            (sf_account_other, soeREQUIRED),
            (sf_owner_node, soeREQUIRED),
            (sf_expiration, soeREQUIRED),
        ],
        is_deletion_blocker=True,
        # visit_entry_xrp_change=visit_entry_xrp_change_token_swap,
    )
]

amendment = Amendment("featureTokenSwap", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]

def preflight_propose(ctx):

    print("Token Swap Create test preflight...")

    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT

    if not ctx.tx.is_field_present(sf_expiration):
        return temBAD_EXPIRATION

    return preflight2(ctx)

def do_apply_propose(ctx, _mPriorBalance, _mSourceBalance):
    print("Token Swap Create test doApply...")

    amount = ctx.tx[sf_amount]
    amount_other = ctx.tx[sf_amount_other]
    account = ctx.tx[sf_account]
    account_other = ctx.tx[sf_account_other]
    expiration = ctx.tx[sf_expiration]
   
    sle_a = ctx.view().peek(account_keylet(account))
    slea_a = ctx.view().peek(trustline_keylet(account, amount.issuer()))
    slea_b = ctx.view().peek(trustline_keylet(account, amount_other.issuer()))
    sleb_b = ctx.view().peek(trustline_keylet(account_other, amount_other.issuer()))
    sleb_a = ctx.view().peek(trustline_keylet(account_other, amount.issuer()))

    close_time = ctx.view().info().parent_close_time
    acc_seq = int(str(ctx.tx.get_seq_proxy().value()), 16)
    
    if expired(expiration, close_time):
        return tecEXPIRED
    
    # Check trustlines exist
    if not slea_a or not slea_b or not sleb_b or not sleb_a:
        return tecNO_LINE
    
    token_swap_keylet = new_token_swap_keylet(account, acc_seq)
    slep = make_sle(token_swap_keylet)
    slep[sf_account] = account
    slep[sf_amount] = amount
    slep[sf_token_swap_id] = acc_seq
    slep[sf_amount_other] = amount_other
    slep[sf_account_other] = account_other
    slep[sf_expiration] = expiration

    ctx.view().insert(slep)

    page = ctx.view().dir_insert(account, token_swap_keylet)
    if page is None:
        return tecDIR_FULL
    slep[sf_owner_node] = page

    adjust_owner_count(ctx.view(), sle_a, 1, ctx.journal)

    return tesSUCCESS

def preflight_accept(ctx):

    print("Token Swap Accept test preflight...")

    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT
    
    # TODO: more checks
    
    return preflight2(ctx)

def do_apply_accept(ctx, _mPriorBalance, _mSourceBalance):

    print("Token Swap Accept test doApply...")

    amount = ctx.tx[sf_amount]
    amount_other = ctx.tx[sf_amount_other]
    account = ctx.tx[sf_account]
    token_swap_id = ctx.tx[sf_token_swap_id]
    account_other = ctx.tx[sf_account_other]

    sle_a = ctx.view().peek(account_keylet(account))
    sle_b = ctx.view().peek(account_keylet(account_other))

    # Token that the initiator is willing to provide
    token_issuer_left_to_right = amount_other.issuer()
    # amount.issuer() -> token that the initiator is willing to receive
    token_issuer_right_to_left = amount.issuer()

    # Trustline for the token swap receiver account (account)
    slea_a = ctx.view().peek(trustline_keylet(account, token_issuer_right_to_left))
    slea_b = ctx.view().peek(trustline_keylet(account, token_issuer_left_to_right))

    # Trustline for the token swap initiator account (account_other)
    sleb_b = ctx.view().peek(trustline_keylet(account_other, token_issuer_left_to_right))
    sleb_a = ctx.view().peek(trustline_keylet(account_other, token_issuer_right_to_left))

    close_time = ctx.view().info().parent_close_time

    # Check reserves
    balance = STAmount(sle_a[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle_a[sf_owner_count])

    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    # Check trustlines exist
    if not slea_a or not slea_b or not sleb_b or not sleb_a:
        return tecNO_LINE
    
    token_swap_keylet = new_token_swap_keylet(account_other, token_swap_id)
    slep = ctx.view().peek(token_swap_keylet)

    # Check swap id
    if not slep: #or slep[sf_token_swap_id] != token_swap_id:
        return temINVALID_TOKEN_SWAP_ID

    # Validate expiration
    if expired(slep[sf_expiration], close_time):
        return tecEXPIRED
    
    # Check amounts
    if slep[sf_amount] != amount_other or slep[sf_amount_other] != amount:
        return temBAD_AMOUNT

    # TODO: more checks (frozen trustline...)
    
    slea_a[sf_balance] = slea_a[sf_balance] + amount
    slea_b[sf_balance] = slea_b[sf_balance] - amount_other
    
    sleb_a[sf_balance] = sleb_a[sf_balance] + amount
    sleb_b[sf_balance] = sleb_b[sf_balance] - amount_other
    
    
    ctx.view().update(slea_a)
    ctx.view().update(slea_b)
    ctx.view().update(sleb_b)
    ctx.view().update(sleb_a)

    page = slep[sf_owner_node]
    # Delete token swap object on originating account
    ctx.view().dir_remove(owner_dir_keylet(account_other), page, token_swap_keylet, True)
    ctx.view().erase(slep)

    #ctx.view().update(slep)

    # Adjust reserve count
    adjust_owner_count(ctx.view(), sle_b, -1, ctx.journal)

    return tesSUCCESS

transactors = [
    Transactor(
        name="TokenSwapPropose",
        tx_type=53,
        tx_format=[
            (sf_amount, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED),
            (sf_expiration, soeREQUIRED),
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight_propose,
        do_apply=do_apply_propose,
    ),
    Transactor(
        name="TokenSwapAccept",
        tx_type=54,
        tx_format=[
            (sf_amount, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED),
            (sf_token_swap_id, soeREQUIRED),
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight_accept,
        do_apply=do_apply_accept,
    )
]
