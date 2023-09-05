from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    preflight1,
    preflight2,
    tf_universal_mask,
)
from xrpl_plugin.ledger_objects import (
    adjust_owner_count,
    make_sle,
)
from xrpl_plugin.models import (
    Amendment,
    LedgerObject,
    Transactor,
)
from xrpl_plugin.basic_types import (
    VoteBehavior,
    soeOPTIONAL,
    soeREQUIRED,
)
from xrpl_plugin.nfts import (
    find_token,
    get_flags,
    flag_transferable,
    find_token_and_page,
    remove_token,
)
from xrpl_plugin.keylets import Keylet, account_keylet
from xrpl_plugin.return_codes import (
    tecDIR_FULL,
    tecINSUFFICIENT_RESERVE,
    tecINTERNAL,
    tecNO_DST,
    tecNO_PERMISSION,
    temBAD_EXPIRATION,
    temDISABLED,
    tesSUCCESS,
    temINVALID_FLAG,
    tecNO_ENTRY,
    tefNFTOKEN_IS_NOT_TRANSFERABLE,
    is_tes_success,
)
from xrpl_plugin.sfields import (
    sf_account,
    sf_balance,
    sf_cancel_after,
    sf_destination_node,
    sf_destination_tag,
    sf_owner_count,
    sf_owner_node,
    sf_previous_txn_id,
    sf_previous_txn_lgr_seq,
    sf_source_tag,
    sf_ticket_sequence,
    sf_nftoken_id,
    sf_destination,
    sf_finish_after,
    sf_nftokens,
)
from xrpl_plugin.stypes import (
    STAmount,
    STArray,
    index_hash,
)

###############################################################################
###############################################################################
# WARNING: This is only for demo purposes
# This has not been fully tested.
# It likely has bugs and is therefore not production-ready.
###############################################################################
###############################################################################


ltNFTOKEN_ESCROW = 0x74
NFTOKEN_ESCROW_NAMESPACE = ord("t")


ledger_objects = [
    LedgerObject(
        object_type=ltNFTOKEN_ESCROW,
        name="NFTokenEscrow",
        rpc_name="nftoken_escrow",
        object_format=[
            (sf_account, soeREQUIRED),
            (sf_destination, soeREQUIRED),
            (sf_nftokens, soeREQUIRED),
            (sf_cancel_after, soeOPTIONAL),
            (sf_finish_after, soeOPTIONAL),
            (sf_source_tag, soeOPTIONAL),
            (sf_destination_tag, soeOPTIONAL),
            (sf_owner_node, soeREQUIRED),
            (sf_previous_txn_id, soeREQUIRED),
            (sf_previous_txn_lgr_seq, soeREQUIRED),
            (sf_destination_node, soeOPTIONAL),
        ],
        is_deletion_blocker=True,
    )
]

amendment = Amendment("featureNFTokenEscrow", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]


def nftoken_escrow_keylet(src, seq):
    return Keylet(ltNFTOKEN_ESCROW, index_hash(NFTOKEN_ESCROW_NAMESPACE, src, seq))


def after(now, mark):
    return now.time_since_epoch() > mark


def preflight(ctx):
    if not ctx.rules.enabled(amendment):
        return temDISABLED

    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    if not ctx.tx.is_field_present(sf_cancel_after) and not ctx.tx.is_field_present(
        sf_finish_after
    ):
        return temBAD_EXPIRATION

    if (
        ctx.tx.is_field_present(sf_cancel_after)
        and ctx.tx.is_field_present(sf_finish_after)
        and ctx.tx[sf_cancel_after] <= ctx.tx[sf_finish_after]
    ):
        return temBAD_EXPIRATION

    return preflight2(ctx)


def preclaim(ctx):
    nftoken_id = ctx.tx[sf_nftoken_id]
    if not find_token(ctx.view, ctx.tx[sf_account], nftoken_id):
        return tecNO_ENTRY

    if not get_flags(nftoken_id) & flag_transferable:
        return tefNFTOKEN_IS_NOT_TRANSFERABLE

    # if not ctx.view.read(account_keylet(ctx.tx[sf_account])):
    #     return tecNO_DST

    return tesSUCCESS


def do_apply(ctx, _m_prior_balance, _m_source_balance):
    close_time = ctx.view().info().parent_close_time

    if ctx.tx.is_field_present(sf_cancel_after) and after(
        close_time, ctx.tx[sf_cancel_after]
    ):
        return tecNO_PERMISSION

    if ctx.tx.is_field_present(sf_finish_after) and after(
        close_time, ctx.tx[sf_finish_after]
    ):
        return tecNO_PERMISSION

    account = ctx.tx[sf_account]
    sle = ctx.view().peek(account_keylet(account))
    if not sle:
        return tecINTERNAL

    balance = sle[sf_balance].xrp()
    reserve = ctx.view().fees.account_reserve(sle[sf_owner_count] + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    dest_acct = ctx.tx[sf_destination]

    sled = ctx.view().peek(account_keylet(dest_acct))
    if not sled:
        return tecNO_DST
    # TODO: implement dest tag check

    # TODO: implement deposit auth check

    nftoken_id = ctx.tx[sf_nftoken_id]
    token_and_page = find_token_and_page(ctx.view(), account, nftoken_id)

    if not token_and_page:
        return tecINTERNAL

    ret = remove_token(ctx.view(), account, nftoken_id, token_and_page.page)
    if not is_tes_success(ret):
        return ret

    keylet = nftoken_escrow_keylet(account, ctx.tx.get_seq_proxy().value())
    slep = make_sle(keylet)

    array = STArray()
    array.append(token_and_page.token)

    slep[sf_account] = account
    slep.set_field_array(sf_nftokens, array)
    slep[sf_destination] = ctx.tx[sf_destination]
    slep[sf_finish_after] = ctx.tx[sf_finish_after]
    ctx.view().insert(slep)

    page = ctx.view().dir_insert(account, keylet)
    if page is None:
        return tecDIR_FULL
    slep[sf_owner_node] = page

    if dest_acct != account:
        page2 = ctx.view().dir_insert(dest_acct, keylet)
        if page2 is None:
            return tecDIR_FULL
        slep[sf_owner_node] = page2

    adjust_owner_count(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)

    return tesSUCCESS


transactors = [
    Transactor(
        name="NFTokenEscrowCreate",
        tx_type=47,
        tx_format=[
            (sf_destination, soeREQUIRED),
            (sf_nftoken_id, soeREQUIRED),
            (sf_cancel_after, soeOPTIONAL),
            (sf_finish_after, soeOPTIONAL),
            (sf_destination_tag, soeOPTIONAL),
            (sf_ticket_sequence, soeOPTIONAL),
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        preclaim=preclaim,
        do_apply=do_apply,
    )
]
