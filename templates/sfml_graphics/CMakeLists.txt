# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  sfml_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  sfml_app
  PROPERTIES APP_TITLE "SFML Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
)

target_link_libraries(
  sfml_app
  PRIVATE arm_neon_clangd_fix sfml-graphics sfml-window lognx
)

add_nro_target(sfml_app)
