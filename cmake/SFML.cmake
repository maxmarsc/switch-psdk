# ##############################################################################
# SFML itself
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  switch_sfml
  GIT_REPOSITORY git@github.com:maxmarsc/switch-sfml.git
  GIT_TAG "switch/fix-fetch-content"
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(switch_sfml)

# ##############################################################################
# SFML portlibs dependencies
# ##############################################################################
find_package(GLapi REQUIRED)
find_package(DRMNouveau REQUIRED)
find_package(GLad REQUIRED)

# SFML is missing some dependencies, so we need to link them manually.
target_link_libraries(
  sfml-window
  INTERFACE switch::glad switch::glapi switch::drm_nouveau switch::libnx
)
