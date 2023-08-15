cmake_minimum_required(VERSION 3.22)
project(xrpl_plugin_js)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)
find_package(pybind11 REQUIRED)


add_subdirectory(../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
add_subdirectory(../external/v8-cmake subproject/v8-cmake EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)

add_library(xrpl_plugin_js SHARED)
target_sources(xrpl_plugin_js PRIVATE plugin_src/transactor.cpp)
target_include_directories(xrpl_plugin_js
        PUBLIC
        ../external/v8-cmake/v8/include
        PRIVATE
        ../external/v8-cmake/v8
#         /Users/mvadari/Documents/xrpl-plugin/external/rippled/src/
#         /Users/mvadari/.conan/data/boost/1.77.0/_/_/source/src
#         /Users/mvadari/.conan/data/date/3.0.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include
        )
target_link_libraries(xrpl_plugin_js
        PRIVATE
        v8_base_without_compiler
        v8_compiler
        v8_snapshot
        v8_inspector
        v8_libplatform
        xrpl_plugin
        )
set_target_properties(xrpl_plugin_js PROPERTIES PREFIX "" SUFFIX ".xrplugin")