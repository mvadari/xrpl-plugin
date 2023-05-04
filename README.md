# plugin-transactor


Setup:

```bash
git clone --recurse-submodules https://github.com/mvadari/plugin-transactor.git
cd plugin-transactor
```

# Python

## C++:
```bash
cd cpp
mkdir .build && cd .build
conan install .. --output-folder . --build missing --settings build_type=Debug
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -Dtests=Off .. 
make -j 4
```

## Python

```bash
pip install ./python
# run rippled with the same branch and submit `DummyTx` transactions to it
```

# Rust

## Build Rust

```bash
cd rust
cargo build
```

## Execute Rust Transaction in rippled

1. Build `rippled` per the instructions in [BUILD.md](./external/rippled/BUILD.md).
2. Build `rust` project per the above instructions.
3. Create a `rippled.cfg` file with the contents of [this rippled.cfg]()https://raw.githubusercontent.com/XRPLF/xrpl4j/main/xrpl4j-integration-tests/src/test/resources/rippled/rippled.cfg
4. Select a directory for the rippled database (e.g., `../db` and `../db/nudb`).

In `rippled.cfg`, change the change `[node_db]` stanza's path value to the path to your `db/nudb` directory,
like this:

```yaml
[node_db]
type=nudb
path=../db/nudb
```

And change the `[database_path]` stanza's path value to the path to your `db` directory, like this:
```yaml
[database_path]
  ../db
```

5. Start rippled using this command:

```bash
cd ./external/rippled/.build
./rippled --conf ../rippled.cfg -a --start
```

6. Submit a `DummyTx2` transaction using this command:

```bash
./rippled --conf ../rippled.cfg submit snoPBrXtMeMyMHUVTgbuqAfg1SUTb '{"TransactionType": "DummyTx2", "Account": "rHb9CJAWyB4rj91VRWn96DkukG4bwdtyTh", "RegularKey": "rKyudGBFnT6N5fJdaHnRqLpWM1CD8oFrBZ"}'
```

This utilizes the rippled [submit] JSON RPC endpoint to submit the JSON transaction.