cmake_minimum_required(VERSION 3.22)
project(plugin_transactor)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)


add_subdirectory(../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_core PUBLIC soci::soci)

add_library(plugin_transactor SHARED)
target_sources(plugin_transactor PRIVATE 
  src/EscrowCreate.cpp
)
target_link_libraries(plugin_transactor PUBLIC
  xrpl_core
)
install(TARGETS plugin_transactor)
