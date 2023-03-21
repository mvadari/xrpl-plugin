cmake_minimum_required(VERSION 3.22)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

find_package(SOCI REQUIRED)


add_subdirectory(../external/rippled subproject/rippled EXCLUDE_FROM_ALL)
target_link_libraries(xrpl_core PUBLIC soci::soci)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE 
  ${SOURCE_FILE}
)
target_link_libraries(${PROJECT_NAME} PUBLIC
  xrpl_core
)
install(TARGETS ${PROJECT_NAME})
