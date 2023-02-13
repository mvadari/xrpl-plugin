import plugin_transactor
from plugin_transactor import STTx, getAccount


def preflight(tx):
    print("HIIIIIIIIII")
    print(tx)
    print(dir(tx))
    print(dir(plugin_transactor))
    print(tx.getTxnType())
    print("DANGER ZONE")
    print(getAccount(tx))
