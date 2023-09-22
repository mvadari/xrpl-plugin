cmake_minimum_required(VERSION 3.22)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)
find_package(pybind11 REQUIRED)

add_subdirectory(../rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_plugin PUBLIC soci::soci)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILE})
target_link_libraries(${PROJECT_NAME} PUBLIC
  xrpl_plugin
  PRIVATE pybind11::embed
)
target_compile_options (${PROJECT_NAME} PUBLIC -Wno-return-type-c-linkage)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".xrplugin")
install(TARGETS ${PROJECT_NAME})
