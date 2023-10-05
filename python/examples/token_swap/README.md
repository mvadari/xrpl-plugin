> [!WARNING]  
> This example is not production ready and it is only for demonstration purposes.

# Token Swap

## Abstract

Token swaps are useful when two users agree on an exchange and remove the need to trade via a DEX or an AMM.

In this example, the swap is a 2-step process:

1. `UserA` and `UserB` agree on a swap by which `UserA` will give x `A` tokens to `UserB` and `UserB` will give y `B` tokens to `UserA`.
2. `UserA` initiates the trade on the ledger with the pre agreed conditions via a `TokenSwapPropose` transaction.
This creates a `TokenSwap` object in the ledger with a `TokenSwapId`.
3. `UserB` validates the trade and accepts it (if the conditions are the pre agreed ones) by sending a `TokwnSwapAccept` transaction with the given `TokenSwapId`.
4. If `TokwnSwapAccept` transaction is successful, the swap will be executed atomically.

## Overview

### New transactions and Object types

New ledger objects:

- `TokenSwap`

The new transactions are:

- `TokenSwapPropose`
- `TokenSwapAccept`

### TokenSwap ledger object

A **`TokenSwap`** object has the following fields:

| Field Name                 | Required? | JSON Type         | Internal Type   |
| -------------------------- | --------- | ----------------- | --------------- |
| `AccountOther`             | ✔️         | `string`          | `ACCOUNT`       |
| `Amount`                   | ✔️         | `Currency Amount` | `AMOUNT`        |
| `AmountOther`              | ✔️         | `Currency Amount` | `AMOUNT`        |
| `TokenSwapId`              | ✔️         | `string`          | `UINT64`        |

### TokenSwapPropose transaction

A **`TokenSwapPropose`** transaction may have the following fields:

| Field Name                 | Required? | JSON Type         | Internal Type   |
| -------------------------- | --------- | ----------------- | --------------- |
| `AccountOther`             | ✔️         | `string`          | `ACCOUNT`       |
| `Amount`                   | ✔️         | `Currency Amount` | `AMOUNT`        |
| `AmountOther`              | ✔️         | `Currency Amount` | `AMOUNT`        |

### TokenSwapAccept transaction

A **`TokenSwapAccept`** transaction may have the following fields:

| Field Name                 | Required? | JSON Type         | Internal Type   |
| -------------------------- | --------- | ----------------- | --------------- |
| `TokenSwapId`              | ✔️         | `string`          | `UINT64`        |
| `Amount`                   | ✔️         | `Currency Amount` | `AMOUNT`        |
| `AmountOther`              | ✔️         | `Currency Amount` | `AMOUNT`        |

## Example requests

### TokenSwapPropose

```
{
"Account": "ratB3Rp7pcid4hzwSYTWiqWXYXFmWUFDv1",
"TransactionType": "TokenSwapPropose",
"AccountOther": "rPTScb8m3wq6r3Ys93Ec5at7LYDmWrtndi",
"Amount": {
    "currency": "EUR",
    "issuer": "rnz5f1MFcgbVxzYhU5hUKbKquEJHJady5K",
    "value": "12"
},
"AmountOther": {
    "currency": "USD",
    "issuer": "rnz5f1MFcgbVxzYhU5hUKbKquEJHJady5K",
    "value": "33"
}
```

### TokenSwapAccept

```
{
    "Account": "rPTScb8m3wq6r3Ys93Ec5at7LYDmWrtndi",
    "TransactionType": "TokenSwapAccept",
    "AccountOther": "ratB3Rp7pcid4hzwSYTWiqWXYXFmWUFDv1",
    "TokenSwapId": "5",
    "Amount": {
        "currency": "EUR",
        "issuer": "rnz5f1MFcgbVxzYhU5hUKbKquEJHJady5K",
        "value": "12"
    },
    "AmountOther": {
        "currency": "USD",
        "issuer": "rnz5f1MFcgbVxzYhU5hUKbKquEJHJady5K",
        "value": "33"
    }
}
```