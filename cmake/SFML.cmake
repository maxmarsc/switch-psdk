# ##############################################################################
# SFML itself
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY git@github.com:Scorbutics/SFML-opengl-es2.git
  GIT_TAG 2061f9163f5919208590dddf037671467990e540
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
