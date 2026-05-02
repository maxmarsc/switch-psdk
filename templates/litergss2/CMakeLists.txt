# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  litergss2_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  litergss2_app
  PROPERTIES APP_TITLE "LiteRGSS2 Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
             ROMFS "${CMAKE_CURRENT_BINARY_DIR}/romfs"
)

target_link_libraries(
  litergss2_app
  PRIVATE arm_neon_clangd_fix 
  LiteRGSS2
  Ruby::ruby
  lognx
)

target_compile_definitions(
  litergss2_app
  PRIVATE
    RUBY_TMP_DIR="sdmc:/switch/.tmp_ruby"
    RUBY_STDLIB_DIR="romfs:/ruby/3.4.0"
    RUBY_MAIN_APP="romfs:/ruby/app/main.rb"
)

# ##############################################################################
# ROMFS setup
# ##############################################################################
# First copy the source ROMFS to the build dir
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/romfs
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Then add the ruby stdlib to the ROMFS
ruby_switch_setup_romfs(litergss2_app ${CMAKE_CURRENT_BINARY_DIR}/romfs)

# ##############################################################################
# Final setup
# ##############################################################################
add_nro_target(litergss2_app)