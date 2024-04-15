"""
This file contains a Python implementation of `SetRegularKey`.
"""

from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    fixMasterKeyAsRegularKey,
    preflight1,
    preflight2,
    tf_universal_mask,
)
from xrpl_plugin.ledger_objects import lsfDisableMaster
from xrpl_plugin.models import Amendment, Transactor
from xrpl_plugin.basic_types import VoteBehavior, soeOPTIONAL
from xrpl_plugin.keylets import account_keylet, signers_keylet
from xrpl_plugin.return_codes import (
    tecNO_ALTERNATIVE_KEY,
    temBAD_REGKEY,
    temINVALID_FLAG,
    tesSUCCESS,
)
from xrpl_plugin.sfields import sf_account, sf_regular_key


# Checks that are done without access to ledger state, just the transaction
# These checks can be performed on the submission node without broadcasting
def preflight(ctx):
    print("This is a Python plugin")
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    if ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if (
        ctx.rules.enabled(fixMasterKeyAsRegularKey)
        and ctx.tx.is_field_present(sf_regular_key)
        and ctx.tx[sf_regular_key] == ctx.tx[sf_account]
    ):
        return temBAD_REGKEY

    return preflight2(ctx)


def do_apply(ctx, _mPriorBalance, _mSourceBalance):
    account = ctx.tx[sf_account]
    sle = ctx.view().peek(account_keylet(account))
    # skip weird fee stuff
    if ctx.tx.is_field_present(sf_regular_key):
        sle[sf_regular_key] = ctx.tx[sf_regular_key]
    else:
        # Account has disabled master key and no multi-signer signer list.
        if sle.is_flag(lsfDisableMaster) and not ctx.view().peek(
            signers_keylet(account)
        ):
            return tecNO_ALTERNATIVE_KEY

        del sle[sf_regular_key]
    return tesSUCCESS


transactors = [
    Transactor(
        name="SetRegularKey2",
        tx_type=97,
        tx_format=[
            (sf_regular_key, soeOPTIONAL),
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        do_apply=do_apply,
    )
]

amendment = Amendment("featurePluginTest", True, VoteBehavior.DEFAULT_NO)

amendments = [amendment]
