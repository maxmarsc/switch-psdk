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
# ~~~
# # First copy the source ROMFS to the build dir
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/romfs
#     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
# )

# # Then add the ruby stdlib to the ROMFS
# ruby_switch_setup_romfs(litergss2_app ${CMAKE_CURRENT_BINARY_DIR}/romfs)
# ~~~

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/romfs")

# Create a stamp file to track romfs sync
set(_romfs_stamp ${CMAKE_CURRENT_BINARY_DIR}/romfs/.base_copied)

add_custom_command(
    OUTPUT ${_romfs_stamp}
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
            "${CMAKE_CURRENT_BINARY_DIR}/romfs"
    COMMAND ${CMAKE_COMMAND} -E touch "${_romfs_stamp}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
    COMMENT "Syncing base romfs"
)

add_custom_target(litergss2_app_base_romfs
    DEPENDS ${_romfs_stamp}
)

# Now setup ruby stdlib - it will layer on top
ruby_switch_setup_romfs(litergss2_app ${CMAKE_CURRENT_BINARY_DIR}/romfs)

# Make ruby stdlib sync wait for base romfs
add_dependencies(litergss2_app_ruby_stdlib_romfs litergss2_app_base_romfs)

# ##############################################################################
# Final setup
# ##############################################################################
add_nro_target(litergss2_app)