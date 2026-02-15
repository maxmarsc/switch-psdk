# ##############################################################################
# SFML itself
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY git@github.com:maxmarsc/switch-sfml.git
  GIT_TAG "master"
  GIT_SHALLOW ON
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SFML)

# ##############################################################################
# SFML portlibs dependencies
# ##############################################################################
find_package(GLapi REQUIRED)
find_package(DRMNouveau REQUIRED)
# find_package(GLad REQUIRED)

# SFML is missing some dependencies, so we need to link them manually.
target_link_libraries(
  sfml-window
  INTERFACE switch::glapi switch::drm_nouveau switch::libnx
)
