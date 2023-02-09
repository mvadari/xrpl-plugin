import plugin_transactor
from plugin_transactor import accountKeylet

def preflight(ctx):
    tx = ctx.tx
    print(tx)
    print(tx.getTxnType())
    print(ctx.rules)



def doApply(ctx):
    print("In doApply")
    print(ctx.tx, ctx.tx.Account)
    view = ctx.view()
    print(view)
    keylet = accountKeylet(account)
    print(keylet, keylet.type, keylet.key)
    account_root = view.peek(keylet)
    print(account_root)
    view.update(account_root)

