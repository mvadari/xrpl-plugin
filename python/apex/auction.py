from xrpl_plugin.basic_types import (
    zero_amount,
    VoteBehavior,
    soeOPTIONAL,
    soeREQUIRED,
)
from xrpl_plugin.transactors import (
    preflight1,
    preflight2,
    tf_universal_mask,
)
from xrpl_plugin.sfields import (
    create_new_sfield,
    sf_account,
    sf_nftoken_id,
    sf_nftoken,
    sf_expiration,
    sf_balance,
    sf_owner_count,
    sf_owner_node,
    sf_previous_txn_id,
    sf_previous_txn_lgr_seq,
)
from xrpl_plugin.stypes import (
    STAmount,
    STAccount,
    index_hash,
)
from xrpl_plugin.return_codes import (
    tecDIR_FULL,
    tecINSUFFICIENT_RESERVE,
    tecINTERNAL,
    tecNO_PERMISSION,
    tecUNFUNDED,
    temBAD_AMOUNT,
    temBAD_EXPIRATION,
    temDISABLED,
    tesSUCCESS,
    temINVALID_FLAG,
    tecNO_ENTRY,
    tefNFTOKEN_IS_NOT_TRANSFERABLE,
    is_tes_success,
)
from xrpl_plugin.ledger_objects import (
    adjust_owner_count,
    make_sle,
)
from xrpl_plugin.models import Amendment, LedgerObject, Transactor, TERCode
from xrpl_plugin.nfts import (
    find_token,
    get_flags,
    flag_transferable,
    find_token_and_page,
    remove_token,
    insert_token,
)
from xrpl_plugin.keylets import Keylet, account_keylet, owner_dir_keylet


def after(now, mark):
    return now.time_since_epoch() > mark


sf_min_bid = create_new_sfield(STAmount, "MinBid", 32)
sf_highest_bid = create_new_sfield(STAmount, "HighestBid", 33)
sf_bid = create_new_sfield(STAmount, "Bid", 34)
sf_highest_bidder = create_new_sfield(STAccount, "HighestBidder", 24)

sfields = [sf_min_bid, sf_highest_bid, sf_bid, sf_highest_bidder]

lt_auction = 0x0041
auction_namespace = ord("U")


def visit_entry_xrp_change_auction(is_delete, entry, is_before):
    if not entry.is_field_present(sf_highest_bid):
        return 0
    if is_before:
        return -1 * entry[sf_highest_bid].xrp().drops
    if not is_delete:
        return entry[sf_highest_bid].xrp().drops
    return 0


ledger_objects = [
    LedgerObject(
        object_type=lt_auction,
        name="Auction",
        rpc_name="auction",
        object_format=[
            (sf_account, soeREQUIRED),
            (sf_nftoken, soeREQUIRED),
            (sf_min_bid, soeOPTIONAL),
            (sf_highest_bid, soeOPTIONAL),
            (sf_highest_bidder, soeOPTIONAL),
            (sf_expiration, soeREQUIRED),
            (sf_owner_node, soeREQUIRED),
            (sf_previous_txn_id, soeREQUIRED),
            (sf_previous_txn_lgr_seq, soeREQUIRED),
        ],
        is_deletion_blocker=True,
        visit_entry_xrp_change=visit_entry_xrp_change_auction,
    )
]

featureNFTAuction = Amendment("featureNFTAuction")


def auction_keylet(nftoken_id):
    return Keylet(lt_auction, index_hash(auction_namespace, nftoken_id))


tecBID_TOO_LOW = 196
tecCANT_BID_OWN_AUCTION = 197
tecNOT_HIGHEST_BID = 198
tecALREADY_BID = 199
ter_codes = [
    TERCode(
        tecBID_TOO_LOW,
        "tecBID_TOO_LOW",
        "The bid does not meet the threshold of the minimum bid.",
    ),
    TERCode(
        tecCANT_BID_OWN_AUCTION,
        "tecCANT_BID_OWN_AUCTION",
        "An auction cannot be bid on by its owner.",
    ),
    TERCode(
        tecNOT_HIGHEST_BID,
        "tecNOT_HIGHEST_BID",
        "The bid is not high enough.",
    ),
    TERCode(
        tecALREADY_BID,
        "tecALREADY_BID",
        "The account is already the highest bidder.",
    ),
]


amendments = [featureNFTAuction]

############################################################################
# AuctionCreate
############################################################################


def create_preflight(ctx):
    print("This is a Python plugin")
    # Check if the amendment is enabled
    if not ctx.rules.enabled(featureNFTAuction):
        return temDISABLED

    # Ensure the transaction has no flags
    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    # Basic checks like sequence number
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    # Ensure that the minimum bid (if included) is a valid value
    if ctx.tx.is_field_present(sf_min_bid):
        min_bid = ctx.tx[sf_min_bid]
        if not min_bid.is_xrp():
            return temBAD_AMOUNT
        if min_bid < zero_amount:
            return temBAD_AMOUNT

    # Ensure the expiration is valid
    if ctx.tx[sf_expiration] == 0:
        return temBAD_EXPIRATION

    return preflight2(ctx)


def create_preclaim(ctx):
    nftoken_id = ctx.tx[sf_nftoken_id]
    # Ensure that the NFT exists
    if not find_token(ctx.view, ctx.tx[sf_account], nftoken_id):
        return tecNO_ENTRY

    # Ensure that the NFT is transferable
    if not get_flags(nftoken_id) & flag_transferable:
        return tefNFTOKEN_IS_NOT_TRANSFERABLE

    return tesSUCCESS


def create_doapply(ctx, _m_prior_balance, _m_source_balance):
    close_time = ctx.view().info().parent_close_time

    # Ensure that the expiration time hasn't already passed
    if after(close_time, ctx.tx[sf_expiration]):
        print("Expiration is not after the last ledger close")
        return tecNO_PERMISSION

    # Fetch the account (double check that the account still exists)
    account = ctx.tx[sf_account]
    sle_account = ctx.view().peek(account_keylet(account))
    if not sle_account:
        return tecINTERNAL

    # Ensure the account has enough XRP to cover the auction's reserve
    balance = sle_account[sf_balance].xrp()
    reserve = ctx.view().fees.account_reserve(sle_account[sf_owner_count] + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    # Get the NFT
    nftoken_id = ctx.tx[sf_nftoken_id]
    token_and_page = find_token_and_page(ctx.view(), account, nftoken_id)
    if not token_and_page:
        return tecINTERNAL

    # Remove the NFT from the owner's directories
    ret = remove_token(ctx.view(), account, nftoken_id, token_and_page.page)
    if not is_tes_success(ret):
        return ret

    # Create + populate the auction object
    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = make_sle(keylet)

    sle_auction[sf_account] = account
    sle_auction[sf_nftoken] = token_and_page.token
    if ctx.tx.is_field_present(sf_min_bid):
        sle_auction[sf_min_bid] = ctx.tx[sf_min_bid]
    sle_auction[sf_expiration] = ctx.tx[sf_expiration]

    # Insert the auction object into the ledger
    ctx.view().insert(sle_auction)
    page = ctx.view().dir_insert(account, keylet)
    if page is None:
        return tecDIR_FULL
    sle_auction[sf_owner_node] = page

    # Adjust the OwnerCount of the owner
    adjust_owner_count(ctx.view(), sle_account, 1, ctx.journal)

    return tesSUCCESS


############################################################################
# AuctionBid
############################################################################


def bid_preflight(ctx):
    print("AuctionBid")
    # Check if the amendment is enabled
    if not ctx.rules.enabled(featureNFTAuction):
        return temDISABLED

    # Ensure the transaction has no flags
    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    # Basic checks like sequence number
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    # Ensure that the bid is a valid value
    bid = ctx.tx[sf_bid]
    if not bid.is_xrp():
        return temBAD_AMOUNT
    if bid < zero_amount:
        return temBAD_AMOUNT

    return preflight2(ctx)


def bid_preclaim(ctx):
    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = ctx.view.read(keylet)
    if sle_auction is None:
        return tecNO_ENTRY

    # Ensure that the bidder isn't the seller
    if ctx.tx[sf_account] == sle_auction[sf_account]:
        return tecCANT_BID_OWN_AUCTION

    # Ensure that the bid is above the minimum bid
    if (
        sle_auction.is_field_present(sf_min_bid)
        and sle_auction[sf_min_bid] > ctx.tx[sf_bid]
    ):
        return tecBID_TOO_LOW

    return tesSUCCESS


def bid_doapply(ctx, _m_prior_balance, _m_source_balance):
    account = ctx.tx[sf_account]
    sle_account = ctx.view().peek(account_keylet(account))

    # Ensure that the bidder has enough money to place the bid
    balance = STAmount(sle_account[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle_account[sf_owner_count])
    if balance < reserve + STAmount(ctx.tx[sf_bid]).xrp():
        return tecUNFUNDED

    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = ctx.view().peek(keylet)

    # Ensure that the expiration time hasn't already passed
    close_time = ctx.view().info().parent_close_time
    if after(close_time, sle_auction[sf_expiration]):
        print("Expiration is not after the last ledger close")
        return tecNO_PERMISSION

    # Ensure that the bid is the highest bid
    if (
        sle_auction.is_field_present(sf_highest_bid)
        and ctx.tx[sf_bid] <= sle_auction[sf_highest_bid]
    ):
        return tecNOT_HIGHEST_BID

    # Ensure that the bidder is not already the highest bidder
    if (
        sle_auction.is_field_present(sf_highest_bidder)
        and ctx.tx[sf_account] == sle_auction[sf_highest_bidder]
    ):
        return tecALREADY_BID

    # Refund the previous highest bidder
    if sle_auction.is_field_present(sf_highest_bidder):
        sle_prev_bidder = ctx.view().peek(
            account_keylet(sle_auction[sf_highest_bidder])
        )
        sle_prev_bidder[sf_balance] = (
            sle_prev_bidder[sf_balance] + sle_auction[sf_highest_bid]
        )
        ctx.view().update(sle_prev_bidder)

    # Set the account as the highest bidder, Store their bid in the auction object
    sle_auction[sf_highest_bidder] = account
    sle_auction[sf_highest_bid] = ctx.tx[sf_bid]
    sle_account[sf_balance] = sle_account[sf_balance] - ctx.tx[sf_bid]
    ctx.view().update(sle_auction)
    ctx.view().update(sle_account)

    return tesSUCCESS


############################################################################
# AuctionCancel
############################################################################


def cancel_preflight(ctx):
    print("AuctionCancel")
    # Check if the amendment is enabled
    if not ctx.rules.enabled(featureNFTAuction):
        return temDISABLED

    # Ensure the transaction has no flags
    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    # Basic checks like sequence number
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    return preflight2(ctx)


def cancel_preclaim(ctx):
    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = ctx.view.read(keylet)
    if sle_auction is None:
        return tecNO_ENTRY

    # Ensure that the canceler is the bidder
    if ctx.tx[sf_account] != sle_auction[sf_account]:
        return tecNO_PERMISSION

    return tesSUCCESS


def cancel_doapply(ctx, _m_prior_balance, _m_source_balance):
    account = ctx.tx[sf_account]
    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = ctx.view().peek(keylet)

    # Return the highest bidder's money (if applicable)
    if sle_auction.is_field_present(sf_highest_bidder):
        sle_highest_bidder = ctx.view().peek(
            account_keylet(sle_auction[sf_highest_bidder])
        )
        sle_highest_bidder[sf_balance] = (
            sle_highest_bidder[sf_balance] + sle_auction[sf_highest_bid]
        )
        ctx.view().update(sle_highest_bidder)

    # Return the NFT
    # TODO: add a reserve check
    insert_ret = insert_token(ctx.view(), account, sle_auction[sf_nftoken])
    if not is_tes_success(insert_ret):
        return insert_ret

    # Delete auction
    page = sle_auction[sf_owner_node]
    ctx.view().dir_remove(owner_dir_keylet(account), page, keylet, True)
    ctx.view().erase(sle_auction)

    # Adjust reserve count
    sle_account = ctx.view().peek(account_keylet(account))
    adjust_owner_count(ctx.view(), sle_account, -1, ctx.journal)
    ctx.view().update(sle_account)

    return tesSUCCESS


############################################################################
# AuctionFinish
############################################################################


def finish_preflight(ctx):
    print("AuctionFinish")
    # Check if the amendment is enabled
    if not ctx.rules.enabled(featureNFTAuction):
        return temDISABLED

    # Ensure the transaction has no flags
    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    # Basic checks like sequence number
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    return preflight2(ctx)


def finish_doapply(ctx, _m_prior_balance, _m_source_balance):
    keylet = auction_keylet(ctx.tx[sf_nftoken_id])
    sle_auction = ctx.view().peek(keylet)
    if sle_auction is None:
        return tecNO_ENTRY

    close_time = ctx.view().info().parent_close_time

    # Ensure that the expiration time has already passed
    if not after(close_time, sle_auction[sf_expiration]):
        print("Expiration has not yet passed")
        return tecNO_PERMISSION

    # Transfer NFT to the new owner
    # (Or back to the original owner, if no one bid)
    # TODO: add reserve check
    seller = sle_auction[sf_account]
    if sle_auction.is_field_present(sf_highest_bidder):
        destination = sle_auction[sf_highest_bidder]
    else:
        destination = seller

    insert_ret = insert_token(ctx.view(), destination, sle_auction[sf_nftoken])
    if not is_tes_success(insert_ret):
        return insert_ret

    # Transfer highest bid to the seller
    if sle_auction.is_field_present(sf_highest_bid):
        sle_account = ctx.view().peek(account_keylet(seller))
        sle_account[sf_balance] += sle_auction[sf_highest_bid]
        ctx.view().update(sle_account)

    # Delete auction
    page = sle_auction[sf_owner_node]
    ctx.view().dir_remove(owner_dir_keylet(seller), page, keylet, True)
    ctx.view().erase(sle_auction)

    # Adjust reserve count
    sle_account = ctx.view().peek(account_keylet(seller))
    adjust_owner_count(ctx.view(), sle_account, -1, ctx.journal)
    ctx.view().update(sle_account)

    return tesSUCCESS


transactors = [
    Transactor(
        name="AuctionCreate",
        tx_type=95,
        tx_format=[
            (sf_nftoken_id, soeREQUIRED),
            (sf_min_bid, soeOPTIONAL),
            (sf_expiration, soeREQUIRED),
        ],
        preflight=create_preflight,
        preclaim=create_preclaim,
        do_apply=create_doapply,
    ),
    Transactor(
        name="AuctionBid",
        tx_type=96,
        tx_format=[
            (sf_nftoken_id, soeREQUIRED),
            (sf_bid, soeOPTIONAL),
        ],
        preflight=bid_preflight,
        preclaim=bid_preclaim,
        do_apply=bid_doapply,
    ),
    Transactor(
        name="AuctionCancel",
        tx_type=97,
        tx_format=[
            (sf_nftoken_id, soeREQUIRED),
        ],
        preflight=cancel_preflight,
        preclaim=cancel_preclaim,
        do_apply=cancel_doapply,
    ),
    Transactor(
        name="AuctionFinish",
        tx_type=98,
        tx_format=[
            (sf_nftoken_id, soeREQUIRED),
        ],
        preflight=finish_preflight,
        do_apply=finish_doapply,
    ),
]
