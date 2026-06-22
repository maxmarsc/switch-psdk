# ##############################################################################
# LiteRGSS2
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  litergss2
  GIT_REPOSITORY git@github.com:maxmarsc/switch-litergss2.git
  GIT_TAG "feature/switch_support--keyboard"
  GIT_SHALLOW ON
  SOURCE_SUBDIR switch
)
FetchContent_MakeAvailable(litergss2)