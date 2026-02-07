# ~~~
# Try to find GLad library.
# Once done this will define
#
# GLad_FOUND
# GLad_LIBRARY
#
# And the following imported target:
# switch::glad
# ~~~

# Search for the library in the portlibs path defined in your toolchain
find_library(GLad_LIBRARY NAMES glad)

include(FindPackageHandleStandardArgs)
# We only require the library since glapi is a link-time dependency for Mesa
find_package_handle_standard_args(GLad REQUIRED_VARS GLad_LIBRARY)

if(GLad_FOUND AND NOT TARGET switch::glad)
  add_library(switch::glad STATIC IMPORTED GLOBAL)
  set_target_properties(
    switch::glad PROPERTIES IMPORTED_LOCATION "${GLad_LIBRARY}"
  )
endif()

mark_as_advanced(GLad_LIBRARY)
