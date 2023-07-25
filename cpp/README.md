# C++ Plugins

```bash
cd cpp
mkdir build && cd build
conan install .. --output-folder . --build missing --settings build_type=Debug
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH=build/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j 4
```
