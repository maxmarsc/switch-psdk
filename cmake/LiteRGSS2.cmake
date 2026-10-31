# ##############################################################################
# LiteRGSS2
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  litergss2
  GIT_REPOSITORY https://gitlab.com/maxmarsc/litergss2.git
  GIT_TAG c4be1dad9a3a420c4fe177beda98a3e8d6f23d95
  GIT_SHALLOW ON
  SOURCE_SUBDIR switch
)
FetchContent_MakeAvailable(litergss2)
