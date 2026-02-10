# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  cgss_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  cgss_app
  PROPERTIES APP_TITLE "LiteCGSS Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
)

target_link_libraries(
  cgss_app
  PRIVATE arm_neon_clangd_fix LiteCGSS_engine
)

add_nro_target(cgss_app)