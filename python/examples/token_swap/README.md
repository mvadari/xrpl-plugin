# Cross-Chain Bridge

## Abstract

TBD

## 1. Introduction

### 1.1. Design Overview

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
