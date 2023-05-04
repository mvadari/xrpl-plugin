# plugin-transactor


Setup:

```bash
git clone --recurse-submodules https://github.com/mvadari/plugin-transactor.git
cd plugin-transactor
```

C++:
```bash
cd cpp
mkdir .build && cd .build
conan install .. --output-folder . --build missing --settings build_type=Debug
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug Dtests=Off .. 
make -j 4
```

Python:
```bash
pip install ./python
# run rippled with the same branch and submit `DummyTx` transactions to it
```

Rust:
```bash
cd rust
cargo build
```