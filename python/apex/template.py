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

    # TODO: get the NFToken from the owner and remove it from their ownership

    # Check account
    account = ctx.tx[sf_account]
    sle = ctx.view().peek(account_keylet(account))
    if not sle:
        return tecINTERNAL

    # Create the escrow
    keylet = nftoken_escrow_keylet(account, ctx.tx.get_seq_proxy().value())
    slep = make_sle(keylet)

    # TODO: fill in all the info that the NFTokenEscrow ledger object needs in the `slep`

    # TODO: insert escrow into owner's directory

    # TODO: insert escrow into destination's directory

    # commit changes
    adjust_owner_count(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)  # This is what actually commits the change to the ledger

    return tesSUCCESS


ledger_objects = [
    LedgerObject(
        object_type=ltNFTOKEN_ESCROW,
        name="NFTokenEscrow",
        rpc_name="nftoken_escrow",
        object_format=[
            # TODO: fill in
        ],
        is_deletion_blocker=True,
    )
]

transactors = [
    Transactor(
        name="NFTokenEscrowCreate",
        tx_type=47,
        tx_format=[
            # TODO: fill in
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        preclaim=preclaim,
        do_apply=do_apply,
    )
]
