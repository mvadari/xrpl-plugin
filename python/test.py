import plugin_transactor
from plugin_transactor import accountKeylet

print("IMPORTEDDDD")
print(dir(plugin_transactor))

def preflight(ctx):
    print("in preflight")
    print(ctx.tx)


def doApply(ctx):
    account = ctx.tx.Account
    view = ctx.view()
    keylet = accountKeylet(account)
    account_root = view.peek(keylet)
    try:
        account_root.setRegularKey()
        print(account_root)
        view.update(account_root)
    except Exception as ex:
        print("ERRORRRRRRRRRRR")
        print(ex)
        raise ex
    print("after update")
