# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  tracy_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  tracy_app
  PROPERTIES APP_TITLE "Tracy Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
)

if(NOT TARGET TracyClient)
  set(TRACY_ENABLE ON)
  set(TRACY_ON_DEMAND ON)
  set(TRACY_STATIC ON)
  set(TRACY_DELAYED_INIT ON)
  set(TRACY_MANUAL_LIFETIME ON)
  # network & discovery
  set(TRACY_ONLY_IPV4 ON)
  set(TRACY_NO_BROADCAST ON)
  # symbols & callstack
  set(TRACY_NO_CALLSTACK OFF) # maybe we want this ?
  # automatic sampling
  set(TRACY_NO_SAMPLING ON)
  set(TRACY_NO_CONTEXT_SWITCH ON)
  set(TRACY_NO_SYSTEM_TRACING ON)
  set(TRACY_NO_VSYNC_CAPTURE ON)
  set(TRACY_NO_VSYNC_CAPTURE ON)
  set(TRACY_NO_FRAME_IMAGE ON)
  # Misc
  set(TRACY_NO_CRASH_HANDLER ON)
  set(CMAKE_DL_LIBS "")

  include(FetchContent)
  FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/cybervisi0n/tracy
    GIT_TAG "nx_support"
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(tracy)
endif()

target_link_libraries(
  tracy_app
  PRIVATE arm_neon_clangd_fix lognx TracyClient
)

add_nro_target(tracy_app)