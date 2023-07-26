cmake_minimum_required(VERSION 3.22)
project(xrpl_plugin_examples)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)


add_subdirectory(../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)

add_library(trustset2 SHARED)
target_sources(trustset2 PRIVATE 
  src/TrustSet.cpp
)
target_link_libraries(trustset2 PUBLIC
  xrpl_plugin
)

add_library(newescrowcreate SHARED)
target_sources(newescrowcreate PRIVATE 
  src/EscrowCreate.cpp
)
target_link_libraries(newescrowcreate PUBLIC
  xrpl_plugin
)

set_target_properties(trustset2 newescrowcreate PROPERTIES PREFIX "" SUFFIX ".xrplugin")

install(TARGETS trustset2)
install(TARGETS newescrowcreate)
