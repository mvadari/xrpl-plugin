#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ed25519::ed25519" for configuration "Debug"
set_property(TARGET ed25519::ed25519 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ed25519::ed25519 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libed25519.a"
  )

list(APPEND _cmake_import_check_targets ed25519::ed25519 )
list(APPEND _cmake_import_check_files_for_ed25519::ed25519 "${_IMPORT_PREFIX}/lib/libed25519.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
