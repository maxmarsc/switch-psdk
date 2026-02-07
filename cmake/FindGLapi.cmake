# ~~~
# Try to find GLapi library.
# Once done this will define
#
# GLapi_FOUND
# GLapi_LIBRARY
#
# And the following imported target:
# switch::glapi
# ~~~

# Search for the library in the portlibs path defined in your toolchain
find_library(GLapi_LIBRARY NAMES glapi)

include(FindPackageHandleStandardArgs)
# We only require the library since glapi is a link-time dependency for Mesa
find_package_handle_standard_args(GLapi REQUIRED_VARS GLapi_LIBRARY)

if(GLapi_FOUND AND NOT TARGET switch::glapi)
  add_library(switch::glapi STATIC IMPORTED GLOBAL)
  set_target_properties(
    switch::glapi PROPERTIES IMPORTED_LOCATION "${GLapi_LIBRARY}"
  )
endif()

mark_as_advanced(GLapi_LIBRARY)
