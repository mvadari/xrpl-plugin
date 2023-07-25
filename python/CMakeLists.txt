cmake_minimum_required(VERSION 3.22)
project(xrpl_plugin_py)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)
find_package(pybind11 REQUIRED)


add_subdirectory(../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)


pybind11_add_module(xrpl_plugin_py src/main.cpp)
target_compile_definitions(xrpl_plugin_py
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_link_libraries(xrpl_plugin_py PUBLIC xrpl_plugin)
