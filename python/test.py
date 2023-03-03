import plugin_transactor
from plugin_transactor import tesSUCCESS

print("IMPORTEDDDD")
print(dir(plugin_transactor))

def preflight(ctx):
    print("in preflight")
    print(ctx.tx)
    return tesSUCCESS

def preclaim(ctx):
    print("in preclaim")
    print(ctx.tx)
    return tesSUCCESS

def calculateBaseFee(view, ctx):
    print("in calculateBaseFee")
    return view.fees.base

def doApply(ctx):
    print("in doApply")
    return tesSUCCESS
