import plugin_transactor
from plugin_transactor import (
    TER,
    NotTEC,
    tesSUCCESS,
    temINVALID_FLAG,
    preflight1,
    preflight2,
    tfUniversalMask,
    # temBAD_REGKEY,
    # fixMasterKeyAsRegularKey
)

print("IMPORTEDDDD")

def preflight(ctx):
    print("in preflight")
    print(ctx.tx)
    preflight1ret = preflight1(ctx)
    if preflight1ret != NotTEC(tesSUCCESS):
        return preflight1ret

    if ctx.tx.getFlags() & tfUniversalMask:
        print("Malformed transaction: Invalid flags set.")
        return NotTEC(temINVALID_FLAG)

    # if (ctx.rules.enabled(fixMasterKeyAsRegularKey) and
    #     ctx.tx.isFieldPresent(sfRegularKey) and
    #     (ctx.tx.getAccountID(sfRegularKey) == ctx.tx.getAccountID(sfAccount))):
    #     return temBAD_REGKEY

    return preflight2(ctx)

def preclaim(ctx):
    print("in preclaim")
    print(ctx.tx)
    return TER(tesSUCCESS)

def doApply(ctx):
    print("in doApply")
    return TER(tesSUCCESS)
