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
)

# from xrpl_plugin.nfts import (
# )
from xrpl_plugin.keylets import Keylet, account_keylet
from xrpl_plugin.return_codes import (
    temDISABLED,
    tesSUCCESS,
    temINVALID_FLAG,
    tecINTERNAL,
)
from xrpl_plugin.sfields import (
    sf_account,
)
from xrpl_plugin.stypes import (
    index_hash,
)


ltNFTOKEN_ESCROW = 0x74
NFTOKEN_ESCROW_NAMESPACE = ord("t")

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

    # TODO: fill in

    return preflight2(ctx)


def preclaim(ctx):
    # TODO: fill in

    return tesSUCCESS


def do_apply(ctx, _m_prior_balance, _m_source_balance):
    # TODO: additional checks when the transaction is applied

    # Check account
    account = ctx.tx[sf_account]
    sleAccount = ctx.view().peek(account_keylet(account))
    if not sleAccount:
        return tecINTERNAL

    # 3. check that the account has enough funds to pay the owner reserve

    # 4. get destination + check that the destination exists

    # 5. get NFToken from ID, along with the NFTokenPage it's in

    # 6. remove the NFToken from the page, in preparation for adding it to the escrow

    # Create the escrow
    keylet = nftoken_escrow_keylet(account, ctx.tx.get_seq_proxy().value())
    sleEscrow = make_sle(keylet)

    # 7. fill in all the info that the NFTokenEscrow ledger object needs in the `sleEscrow`

    # 8. Insert the `sleEscrow` into the view, so the changes are committed

    # 9. insert escrow into owner's directory

    # 10. insert escrow into destination's directory (if the destination is different than the account)

    # commit changes
    adjust_owner_count(ctx.view(), sleAccount, 1, ctx.journal)
    ctx.view().update(
        sleAccount
    )  # This is what actually commits the change to the SLE to the ledger

    return tesSUCCESS


ledger_objects = [
    LedgerObject(
        object_type=ltNFTOKEN_ESCROW,
        name="NFTokenEscrow",
        rpc_name="nftoken_escrow",
        object_format=[
            # 1. fill in
        ],
        is_deletion_blocker=True,
    )
]

transactors = [
    Transactor(
        name="NFTokenEscrowCreate",
        tx_type=47,
        tx_format=[
            # 2. fill in
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        preclaim=preclaim,
        do_apply=do_apply,
    )
]
