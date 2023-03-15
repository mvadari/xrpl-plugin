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
)


def preflight(ctx):
    preflight1ret = preflight1(ctx)
    if preflight1ret != tesSUCCESS:
        return preflight1ret

    if ctx.tx.getFlags() & tfUniversalMask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if ctx.rules.enabled(fixMasterKeyAsRegularKey) and \
        ctx.tx.isFieldPresent_sfRegularKey() and \
            ctx.tx.getAccountID_sfRegularKey() == ctx.tx.getAccountID_sfAccount():
        return temBAD_REGKEY

    return preflight2(ctx)

def preclaim(ctx):
    return tesSUCCESS

def doApply(ctx):
    account = ctx.tx.getAccountID_sfAccount()
    sle = ctx.view().peek(accountKeylet(account))
    # skip weird fee stuff
    if ctx.tx.isFieldPresent_sfRegularKey():
        sle.setAccountID_sfRegularKey(ctx.tx.getAccountID_sfRegularKey())
    else:
        # Account has disabled master key and no multi-signer signer list.
        if sle.isFlag(lsfDisableMaster) and not ctx.view().peek(signersKeylet(account)):
            return tecNO_ALTERNATIVE_KEY

        sle.makeFieldAbsent_sfRegularKey()
    return tesSUCCESS
