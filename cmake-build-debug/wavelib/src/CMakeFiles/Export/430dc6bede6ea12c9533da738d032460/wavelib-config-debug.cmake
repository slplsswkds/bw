#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "wavelib::wavelib" for configuration "Debug"
set_property(TARGET wavelib::wavelib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(wavelib::wavelib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "m"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libwavelib.a"
  )

list(APPEND _cmake_import_check_targets wavelib::wavelib )
list(APPEND _cmake_import_check_files_for_wavelib::wavelib "${_IMPORT_PREFIX}/lib/libwavelib.a" )

# Import target "wavelib::wauxlib" for configuration "Debug"
set_property(TARGET wavelib::wauxlib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(wavelib::wauxlib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "wavelib::wavelib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libwauxlib.a"
  )

list(APPEND _cmake_import_check_targets wavelib::wauxlib )
list(APPEND _cmake_import_check_files_for_wavelib::wauxlib "${_IMPORT_PREFIX}/lib/libwauxlib.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
