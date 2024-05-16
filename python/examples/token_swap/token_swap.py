from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    preflight2,
)

from xrpl_plugin.models import (
    Transactor,
    LedgerObject,
    Amendment,
    InvariantCheck,
    TERCode,
)
from xrpl_plugin.basic_types import (
    soeREQUIRED,
    VoteBehavior,
    zero_amount,
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
    tecINSUFFICIENT_FUNDS,
    tesSUCCESS,
    tecDIR_FULL,
)

from xrpl_plugin.sfields import (
    sf_balance,
    create_new_sfield,
    sf_account,
    sf_expiration,
    sf_amount,
    sf_low_limit,
    sf_owner_count,
    sf_owner_node,
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
    ltRIPPLE_STATE,
)

class TokensNetBalance(InvariantCheck):

    def __init__(self):
        self.total = 0

    def visit_entry(self, is_delete, before, after):
        if before is not None and before.get_type() == ltRIPPLE_STATE:
            self.total += float(after[sf_balance].get_text()) - float(before[sf_balance].get_text())

    def finalize(self, tx, result, fee, view, j):
        if tx.get_txn_type() == 54:
            return self.total == 0

        return True

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
    )
]

invariant_checks = [TokensNetBalance]

amendment = Amendment("featureTokenSwap", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]

def preflight_propose(ctx):

    print("Token Swap Create test preflight...", flush=True)

    tx_proposer_send_token = ctx.tx[sf_amount]
    tx_approver_send_token = ctx.tx[sf_amount_other] 
    
    if tx_approver_send_token.is_xrp() or tx_proposer_send_token.is_xrp():
        return temBAD_AMOUNT

    if not ctx.tx.is_field_present(sf_expiration):
        return temBAD_EXPIRATION


    return preflight2(ctx)

def do_apply_propose(ctx, _mPriorBalance, _mSourceBalance):
    
    print("Token Swap Create test doApply...", flush=True)

    tx_approver_send_token = ctx.tx[sf_amount]
    tx_proposer_send_token = ctx.tx[sf_amount_other]
    tx_account = ctx.tx[sf_account]
    tx_account_other = ctx.tx[sf_account_other]
    tx_expiration = ctx.tx[sf_expiration]

    account = ctx.view().peek(account_keylet(tx_account))
    acc_seq = int(str(ctx.tx.get_seq_proxy().value()), 16)

    # Token that the proposer is willing to receive
    proposer_send_token_issue = tx_proposer_send_token.issuer()
    # Token that the proposer is willing to provide
    proposer_receive_token_issue = tx_approver_send_token.issuer()

    close_time = ctx.view().info().parent_close_time

    if expired(tx_expiration, close_time):
        return tecEXPIRED

    trustline_proposer_sending_token = ctx.view().peek(trustline_keylet(tx_account, proposer_send_token_issue))
    trustline_receiving_sending_token = ctx.view().peek(trustline_keylet(tx_account, proposer_receive_token_issue))
    trustline_approver_sending_token = ctx.view().peek(trustline_keylet(tx_account_other, proposer_receive_token_issue))
    trustline_receiving_sending_token = ctx.view().peek(trustline_keylet(tx_account_other, proposer_send_token_issue))

    # Check trustlines exist
    if not trustline_proposer_sending_token or not trustline_receiving_sending_token or not trustline_approver_sending_token or not trustline_receiving_sending_token:
        return tecNO_LINE
    
    token_swap_keylet = new_token_swap_keylet(tx_account, acc_seq)

    slep = make_sle(token_swap_keylet)
    slep[sf_account] = tx_account
    slep[sf_amount] = tx_approver_send_token
    slep[sf_token_swap_id] = acc_seq
    slep[sf_amount_other] = tx_proposer_send_token
    slep[sf_account_other] = tx_account_other
    slep[sf_expiration] = tx_expiration

    ctx.view().insert(slep)

    page = ctx.view().dir_insert(tx_account, token_swap_keylet)
    
    if page is None:
        return tecDIR_FULL
    
    slep[sf_owner_node] = page

    adjust_owner_count(ctx.view(), account, 1, ctx.journal)

    return tesSUCCESS

def preflight_accept(ctx):

    print("Token Swap Accept test preflight...")

    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]

    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT
    
    return preflight2(ctx)

def do_apply_accept(ctx, _mPriorBalance, _mSourceBalance):

    print("Token Swap Accept test doApply...")

    tx_approver_send_token = ctx.tx[sf_amount]
    tx_proposer_send_token = ctx.tx[sf_amount_other] 
    tx_account = ctx.tx[sf_account]
    tx_account_other = ctx.tx[sf_account_other]
    tx_token_swap_id = ctx.tx[sf_token_swap_id]
    
    close_time = ctx.view().info().parent_close_time
    proposer_account = ctx.view().peek(account_keylet(tx_account_other))

    # Token that the initiator is willing to receive by accepting
    approver_receive_token_issue = tx_proposer_send_token.issuer()
    # Token that the initiator is willing to provide by accepting
    proposer_receive_token_issue = tx_approver_send_token.issuer()

    # Trustline for the token swap approver account (account)
    trustline_approver_sending_token = ctx.view().peek(trustline_keylet(tx_account, proposer_receive_token_issue))
    trustline_approver_receiving_token = ctx.view().peek(trustline_keylet(tx_account, approver_receive_token_issue))

    # Trustline for the token swap proposer account (account_other)
    trustline_proposer_sending_token = ctx.view().peek(trustline_keylet(tx_account_other, approver_receive_token_issue))
    trustline_proposer_receiving_token = ctx.view().peek(trustline_keylet(tx_account_other, proposer_receive_token_issue))
    
    token_swap_keylet = new_token_swap_keylet(tx_account_other, tx_token_swap_id)
    slep = ctx.view().peek(token_swap_keylet)

    approver_account = ctx.view().peek(account_keylet(tx_account))

    # Check reserves
    balance = STAmount(approver_account[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(approver_account[sf_owner_count])

    token_swap_keylet = new_token_swap_keylet(tx_account_other, tx_token_swap_id)
    slep = ctx.view().peek(token_swap_keylet)

    if balance < reserve:
        return tecINSUFFICIENT_RESERVE
    
    # Check swap id
    if not slep or slep[sf_token_swap_id] != tx_token_swap_id:
        return temINVALID_TOKEN_SWAP_ID
    
    # Check amounts
    if slep[sf_amount] != tx_proposer_send_token or slep[sf_amount_other] != tx_approver_send_token:
        return temBAD_AMOUNT

    # Check trustlines exist
    if not trustline_approver_sending_token or not trustline_approver_receiving_token or not trustline_proposer_sending_token or not trustline_proposer_receiving_token:
        return tecNO_LINE
    
    # TODO: more checks (frozen trustline...)

    # Balances
    approver_token_balance_enough = trustline_approver_sending_token[sf_balance] >= tx_approver_send_token if trustline_approver_sending_token[sf_low_limit].issuer().account == tx_account else trustline_approver_sending_token[sf_balance] <= tx_approver_send_token
    proposer_token_balance_enough = trustline_proposer_sending_token[sf_balance] >= tx_proposer_send_token if trustline_proposer_sending_token[sf_low_limit].issuer().account == tx_account_other else trustline_proposer_sending_token[sf_balance] <= tx_proposer_send_token

    if not approver_token_balance_enough or not proposer_token_balance_enough:
        return tecINSUFFICIENT_FUNDS
    
    # Validate expiration
    if expired(slep[sf_expiration], close_time):
        return tecEXPIRED
    
    # The "issuer" for the balance in a trust line depends on whether the balance is positive or negative.
    # If a RippleState entry shows a positive balance, the high account is the issuer.
    # If the balance is negative, the low account is the issuer.

    print("trustline_approver_sending_token")
    print("--------------------------------")
    print(f"-> before {trustline_approver_sending_token[sf_balance]}")

    if trustline_approver_sending_token[sf_balance] < zero_amount:
        trustline_approver_sending_token[sf_balance] = trustline_approver_sending_token[sf_balance] + tx_approver_send_token
    else:
        trustline_approver_sending_token[sf_balance] = trustline_approver_sending_token[sf_balance] - tx_approver_send_token
    
    print(f"-> after {trustline_approver_sending_token[sf_balance]}")

    print("\n\n")

    print("trustline_approver_receiving_token")
    print("--------------------------------")
    print(f"-> before {trustline_approver_receiving_token[sf_balance]}")

    if trustline_approver_receiving_token[sf_balance] < zero_amount:
        trustline_approver_receiving_token[sf_balance] = trustline_approver_receiving_token[sf_balance] - tx_proposer_send_token
    else:
        trustline_approver_receiving_token[sf_balance] = trustline_approver_receiving_token[sf_balance] + tx_proposer_send_token

    print(f"-> after {trustline_approver_receiving_token[sf_balance]}")

    print("\n\n")

    print("trustline_proposer_sending_token")
    print("--------------------------------")
    print(f"-> before {trustline_proposer_sending_token[sf_balance]}")
   
    if trustline_proposer_sending_token[sf_balance] < zero_amount:
        trustline_proposer_sending_token[sf_balance] = trustline_proposer_sending_token[sf_balance] + tx_proposer_send_token
    else:
        trustline_proposer_sending_token[sf_balance] = trustline_proposer_sending_token[sf_balance] - tx_proposer_send_token

    print(f"-> after {trustline_proposer_sending_token[sf_balance]}")

    print("\n\n")

    print("trustline_proposer_receiving_token")
    print("----------------------------------")
    print(f"-> before {trustline_proposer_receiving_token[sf_balance]}")
    
    if trustline_proposer_receiving_token[sf_balance] < zero_amount:
        trustline_proposer_receiving_token[sf_balance] = trustline_proposer_receiving_token[sf_balance] - tx_approver_send_token
    else:
        trustline_proposer_receiving_token[sf_balance] = trustline_proposer_receiving_token[sf_balance] + tx_approver_send_token
    
    print(f"-> after {trustline_proposer_receiving_token[sf_balance]}")

    print("\n\n")

    ctx.view().update(trustline_approver_sending_token)
    ctx.view().update(trustline_approver_receiving_token)
    ctx.view().update(trustline_proposer_sending_token)
    ctx.view().update(trustline_proposer_receiving_token)

    page = slep[sf_owner_node]
    
    # Delete token swap object on originating account
    ctx.view().dir_remove(owner_dir_keylet(tx_account_other), page, token_swap_keylet, True)
    ctx.view().erase(slep)

    # Adjust reserve count
    adjust_owner_count(ctx.view(), proposer_account, -1, ctx.journal)

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