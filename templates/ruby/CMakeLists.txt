# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  ruby_app
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_target_properties(
  ruby_app
  PROPERTIES APP_TITLE "Ruby Application"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
             ROMFS "${CMAKE_CURRENT_BINARY_DIR}/romfs"
)

target_link_libraries(
  ruby_app
  PRIVATE arm_neon_clangd_fix Ruby::ruby
)

target_compile_definitions(
  ruby_app
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
ruby_switch_setup_romfs(ruby_app ${CMAKE_CURRENT_BINARY_DIR}/romfs)

# ##############################################################################
# Final setup
# ##############################################################################
add_nro_target(ruby_app)