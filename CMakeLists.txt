cmake_minimum_required(VERSION 3.22)
project(plugin_transactor)

find_package(SOCI REQUIRED)
find_package(pybind11 REQUIRED)


add_subdirectory(external/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_core PUBLIC soci::soci)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_library(lib)
target_sources(lib PRIVATE 
  src/lib.cpp
  ${PYTHON_LIBRARIES}
)
target_link_libraries(lib PUBLIC
  xrpl_core
  ${Python3_LIBRARIES}
)

target_include_directories(lib PRIVATE ${Python3_INCLUDE_DIRS})

pybind11_add_module(plugin_transactor src/main.cpp)
target_compile_definitions(plugin_transactor
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})