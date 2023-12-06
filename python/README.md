# Python Plugins

This folder contains everything needed to build and run a Python plugin (other than the rippled code).

## Build & Run

```bash
pip install xrpl-plugin
plugin-build path/to/plugin.py
# edit rippled.cfg to include the plugin.xrplugin in a [plugins] stanza
# run rippled with the `plugin` branch and submit plugin transactions to it
```

## Concepts

### Keylet

A Keylet identifies a key in the state map and its ledger entry type. Keylet is a portmanteau of the key and LET, an acronym for LedgerEntryType.

### LedgerEntryType

Each ledger object requires a unique type identifier, which is stored within the object itself; this makes it possible to iterate the entire ledger, determine each object's type and verify that the object you retrieved from a given hash matches the expected type.

## Notes

pybind11 is a lightweight header-only library that exposes C++ types in Python and vice versa, mainly to create Python bindings of existing C++ code.
The plugin exposes the code in [main.cpp](./src/main.cpp) as python bindings.

Then we can use in our plugin the following:

- **models** (classes):

  - Transactor
  - SType (it has a parse_value method)
  - TERCode
  - LedgerObject
  - InvariantCheck
  - InnerObject

- **sfields** (ST fields, serialized fields that can be included in transactions and ledger objects):

  - SF_ACCOUNT
  - SF_AMOUNT
  - SF_UINT8
  - ...

- **stypes** (all STObjects, serialized types):

  - STAmount
  - STObject
  - STArray
  - ...

- **return_codes** (Existing TER return codes):

  - tesSUCCESS
  - temMALFORMED
  - ...

- **basic_types** (basic/fundamental types):

  - AccountID
  - XRPAmount
  - Seed
  - IOUAmount
  - ...

- **keylets** (as per definition above):

  - account_keylet
  - escrow_keylet
  - trustline_keylet
  - offer_keylet
  - ...

- **ledger_objects** (for ledger object manipulation):

  - LedgerEntryType
  - LedgerSpecificFlags

- **transactors** (contexts and other classes used in transaction processing):

  - Rules
  - Fees
  - TimePoint
  - LedgerInfo

- **build** (for the build process)
- **nfts** (utility methods for working with NFTs)
