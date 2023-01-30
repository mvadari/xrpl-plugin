# plugin-transactor


Setup:

```bash
git clone --recurse-submodules https://github.com/mvadari/plugin-transactor.git
mkdir build && cd build
conan install .. --output-folder . --build missing --settings build_type=Release
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=build/generators/conan_toolchain.cmake -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
python ../test.py
```