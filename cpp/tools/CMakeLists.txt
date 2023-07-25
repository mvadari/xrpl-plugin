cmake_minimum_required(VERSION 3.22)
project(helper)
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)


add_subdirectory(../../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)

add_executable(stype_debugger)
target_sources(stype_debugger PRIVATE 
  DebugSTypes.cpp
)
target_link_libraries(stype_debugger PUBLIC
  xrpl_plugin
)
install(TARGETS stype_debugger)
