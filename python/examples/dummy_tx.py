from plugin_transactor import (
    tesSUCCESS,
    temINVALID_FLAG,
    preflight1,
    preflight2,
    tfUniversalMask,
    temBAD_REGKEY,
    fixMasterKeyAsRegularKey,
    accountKeylet,
    signersKeylet,
    lsfDisableMaster,
    tecNO_ALTERNATIVE_KEY,
    sfRegularKey,
    sfAccount,
)

tx_name = "DummyTx"
tx_type = 30

def preflight(ctx):
    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    if ctx.tx.getFlags() & tfUniversalMask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if ctx.rules.enabled(fixMasterKeyAsRegularKey) and \
        ctx.tx.isFieldPresent(sfRegularKey) and \
            ctx.tx.getAccountID(sfRegularKey) == ctx.tx.getAccountID(sfAccount):
        return temBAD_REGKEY

    return preflight2(ctx)

def preclaim(ctx):
    return tesSUCCESS

def doApply(ctx, _mPriorBalance, _mSourceBalance):
    account = ctx.tx.getAccountID(sfAccount)
    sle = ctx.view().peek(accountKeylet(account))
    # skip weird fee stuff
    if ctx.tx.isFieldPresent(sfRegularKey):
        sle.setAccountID(sfRegularKey, ctx.tx.getAccountID(sfRegularKey))
    else:
        # Account has disabled master key and no multi-signer signer list.
        if sle.isFlag(lsfDisableMaster) and not ctx.view().peek(signersKeylet(account)):
            return tecNO_ALTERNATIVE_KEY

        sle.makeFieldAbsent(sfRegularKey)
    return tesSUCCESS
