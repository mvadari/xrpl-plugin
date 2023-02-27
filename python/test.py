import plugin_transactor
from plugin_transactor import accountKeylet

print("IMPORTEDDDD")
print(dir(plugin_transactor))

def preflight(ctx):
    print("HIIIIIIIIII")
    tx = ctx.tx
    print(tx)
    print(dir(tx))
    print(dir(plugin_transactor))
    print(tx.getTxnType())
    # print(getAccount(tx))
    # print(getAccount(tx).toBase58())

    print("DANGER ZONE")
    print(ctx.rules)



def doApply(ctx):
    print("DOAPPLYYYYYYYYYYYYY")
    print(dir(ctx))
    print(ctx.tx, ctx.tx.Account)
    account = ctx.tx.Account
    view = ctx.view()
    print(view, dir(view))
    keylet = accountKeylet(account)
    print(keylet, dir(keylet))
    print(keylet.type)
    print(keylet.key)
    account_root = view.peek(keylet)
    print(account_root)
    print("before update")
    print(view.update)
    view.update(account_root)
    print("after update")

