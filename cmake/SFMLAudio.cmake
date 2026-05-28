# ##############################################################################
# SFMLAudio (ruby bindings for SFML Audio module)
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  sfmlaudio
  GIT_REPOSITORY git@github.com:maxmarsc/switch-sfmlaudio.git
  GIT_TAG "feature/switch_support"
  GIT_SHALLOW ON
  SOURCE_SUBDIR switch
)
FetchContent_MakeAvailable(sfmlaudio)