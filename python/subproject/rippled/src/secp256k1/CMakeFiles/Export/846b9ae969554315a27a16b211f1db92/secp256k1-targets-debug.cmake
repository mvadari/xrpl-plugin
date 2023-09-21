#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "secp256k1::secp256k1" for configuration "Debug"
set_property(TARGET secp256k1::secp256k1 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(secp256k1::secp256k1 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libsecp256k1.a"
  )

list(APPEND _cmake_import_check_targets secp256k1::secp256k1 )
list(APPEND _cmake_import_check_files_for_secp256k1::secp256k1 "${_IMPORT_PREFIX}/lib/libsecp256k1.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
