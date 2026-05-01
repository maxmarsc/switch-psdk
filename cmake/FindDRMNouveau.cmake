# ~~~
# Try to find DRMNouveau library.
# Once done this will define
#
# DRMNouveau_FOUND
# DRMNouveau_LIBRARY
#
# And the following imported target:
# switch::drm_nouveau
# ~~~

# Search for the library in the portlibs path defined in your toolchain
find_library(DRMNouveau_LIBRARY NAMES drm_nouveau)

include(FindPackageHandleStandardArgs)
# We only require the library since DRMNouveau is a link-time dependency
find_package_handle_standard_args(DRMNouveau REQUIRED_VARS DRMNouveau_LIBRARY)

if(DRMNouveau_FOUND AND NOT TARGET switch::drm_nouveau)
  add_library(switch::drm_nouveau STATIC IMPORTED GLOBAL)
  set_target_properties(
    switch::drm_nouveau PROPERTIES IMPORTED_LOCATION "${DRMNouveau_LIBRARY}"
  )
  target_link_libraries(switch::drm_nouveau INTERFACE switch::libnx)
endif()

mark_as_advanced(DRMNouveau_LIBRARY)
