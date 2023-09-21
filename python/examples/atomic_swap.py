from xrpl_plugin.transactors import (
    ConsequencesFactoryType,
    preflight2,
)
from xrpl_plugin.rippled_py.stypes.parser_errors import bad_type, invalid_data
from xrpl_plugin.ledger_objects import lsfDisableMaster
from xrpl_plugin.models import Amendment, Transactor, SType, InnerObject
from xrpl_plugin.basic_types import (
    soeREQUIRED,
)
from xrpl_plugin.keylets import (
    account_keylet,
    trustline_keylet,
)
from xrpl_plugin.return_codes import (
    tecINSUFFICIENT_RESERVE,
    tecUNFUNDED,
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
)

sf_amount_other = create_new_sfield(STAmount, "AmountOther", 33)
sf_account_other = create_new_sfield(STAccount, "AccountOther", 35)

sfields = [sf_amount_other, sf_account_other]

def preflight(ctx):
    print("Atomic Swap test preflight...")
    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    if amount.is_xrp() or amountOther.is_xrp():
        return temBAD_AMOUNT
    return preflight2(ctx)

def do_apply(ctx, _mPriorBalance, _mSourceBalance):
    print("Atomic Swap test doApply...")
    amount = ctx.tx[sf_amount]
    amountOther = ctx.tx[sf_amount_other]
    account = ctx.tx[sf_account]
    accountOther = ctx.tx[sf_account_other]
    sle = ctx.view().peek(account_keylet(account))
    sleAA = ctx.view().peek(trustline_keylet(account, amount.issuer()))
    sleAB = ctx.view().peek(trustline_keylet(account, amountOther.issuer()))
    sleBB = ctx.view().peek(trustline_keylet(accountOther, amountOther.issuer()))
    sleBA = ctx.view().peek(trustline_keylet(accountOther, amount.issuer()))

    # Check trustlines exist
    if not sleAA or not sleAB or not sleBB or not sleBA:
        return tecINTERNAL
    
    # Check reserves
    balance = STAmount(sle[sf_balance]).xrp()
    reserve = ctx.view().fees.account_reserve(sle[sf_owner_count])
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    # TODO: Check signatures for accountA and accountB instructions
    
    # TODO: A lot more
    
    sleAA[sf_balance] = sleAA[sf_balance] + amount
    sleAB[sf_balance] = sleAB[sf_balance] - amountOther
    sleBB[sf_balance] = sleBB[sf_balance] + amountOther
    sleBA[sf_balance] = sleBA[sf_balance] - amount
    ctx.view().update(sleAA)
    ctx.view().update(sleAB)
    ctx.view().update(sleBB)
    ctx.view().update(sleBA)
    return tesSUCCESS

transactors = [
    Transactor(
        name="AtomicSwap",
        tx_type=51,
        tx_format=[
            (sf_amount, soeREQUIRED),
            (sf_amount_other, soeREQUIRED),
            (sf_account_other, soeREQUIRED)
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        do_apply=do_apply,
    )
]

