# ##############################################################################
# SFMLAudio (ruby bindings for SFML Audio module)
# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  sfmlaudio
  GIT_REPOSITORY git@gitlab.com:maxmarsc/sfmlaudio.git
  GIT_TAG 93d508e8028fe57dba0afbd674f91ecc9391f31b
  GIT_SHALLOW ON
  SOURCE_SUBDIR switch
)
FetchContent_MakeAvailable(sfmlaudio)
