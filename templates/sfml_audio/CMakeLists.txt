# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  sfml_audio_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  sfml_audio_app
  PROPERTIES APP_TITLE "SFML-Audio Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
             ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
)

target_link_libraries(
  sfml_audio_app
  PRIVATE arm_neon_clangd_fix sfml-audio lognx
)

add_nro_target(sfml_audio_app)
