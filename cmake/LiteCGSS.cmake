# ##############################################################################
# LiteCGSS
# ##############################################################################
include(FetchContent)

set(LITECGSS_NO_TEST ON)
set(SKALOG_NO_RLUTIL ON)
FetchContent_Declare(
  litecgss
  GIT_REPOSITORY https://gitlab.com/maxmarsc/litecgss.git
  GIT_TAG f28b60bf4a01ea0ec80a7f3cc00e976bb6a225d0
  GIT_SHALLOW ON
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(litecgss)
