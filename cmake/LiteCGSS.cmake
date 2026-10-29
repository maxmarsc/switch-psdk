# ##############################################################################
# LiteCGSS
# ##############################################################################
include(FetchContent)

set(LITECGSS_NO_TEST ON)
set(SKALOG_NO_RLUTIL ON)
FetchContent_Declare(
  litecgss
  GIT_REPOSITORY git@github.com:maxmarsc/switch-litecgss.git
  GIT_TAG 27151729bf86748fd7baf9faf1953515595ad14a
  GIT_SHALLOW ON
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(litecgss)
