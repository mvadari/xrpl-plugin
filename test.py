from plugin_transactor import STTx


def preflight(tx):
    print(tx)
    print(dir(tx))
    print(tx.getTxnType())
