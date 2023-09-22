cmake_minimum_required(VERSION 3.22)
project(rippled_py)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)
find_package(pybind11 REQUIRED)


add_subdirectory(xrpl_plugin/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)


pybind11_add_module(rippled_py src/main.cpp)
target_compile_definitions(rippled_py
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_link_libraries(rippled_py PUBLIC xrpl_plugin)
target_compile_options (rippled_py PUBLIC -Wno-return-type-c-linkage)
