from plugin_transactor import STTx

print(dir(STTx))


def preflight(tx: STTx):
    print(tx.getTransactionID())