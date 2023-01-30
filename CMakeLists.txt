cmake_minimum_required(VERSION 3.22)
project(try-xrpl)

find_package(SOCI REQUIRED)


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

PYTHON3_ADD_LIBRARY(plugin_lib src/lib.cpp)
target_link_libraries(plugin_lib PUBLIC lib)
