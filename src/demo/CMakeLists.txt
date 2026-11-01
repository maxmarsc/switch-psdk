# ##############################################################################
# Executable
# ##############################################################################
add_executable(
  psdk_demo
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set(ROMFS_DIR ${CMAKE_CURRENT_BINARY_DIR}/romfs)

set_target_properties(
  psdk_demo
  PROPERTIES APP_TITLE "PSDK Demo"
             APP_AUTHOR "maxmarsc"
             APP_VERSION "0.1.0"
             ROMFS "${ROMFS_DIR}"
)

target_link_libraries(
  psdk_demo
  PRIVATE arm_neon_clangd_fix
          LiteRGSS2
          SFMLAudio
          tracy_ruby
          Ruby::ruby
          lognx
)

target_compile_definitions(
  psdk_demo
  PRIVATE RUBY_TMP_DIR="sdmc:/switch/.tmp_ruby"
          PSDK_HOME_DIR="sdmc:/switch/.psdk_home"
          RUBY_STDLIB_DIR="romfs:/ruby/3.4.0"
          RUBY_MAIN_APP="romfs:/ruby/app/Game.rb"
)

# ##############################################################################
# ROMFS setup
# ##############################################################################
file(MAKE_DIRECTORY "${ROMFS_DIR}")

# Create a stamp file to track romfs sync
set(_romfs_stamp ${ROMFS_DIR}/.base_copied)

add_custom_command(
  OUTPUT ${_romfs_stamp}
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${ROMFS_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
          "${ROMFS_DIR}"
  COMMAND ${CMAKE_COMMAND} -E touch "${_romfs_stamp}"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/romfs/ruby/app/main.rb" # re-run if
                                                               # main.rb changes
  COMMENT "Syncing base romfs"
)

add_custom_target(psdk_demo_base_romfs DEPENDS ${_romfs_stamp})

# ##############################################################################
# PSDK demo setup
# ##############################################################################
if(LOAD_COMPILED_PROJECT)
  add_custom_command(
    OUTPUT ${ROMFS_DIR}/ruby/app/.demo_copied
    # ~~~
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/audio" "${ROMFS_DIR}/ruby/app/audio"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/Data" "${ROMFS_DIR}/ruby/app/Data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/Fonts" "${ROMFS_DIR}/ruby/app/Fonts"
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory_if_different
      "${PROJECT_FOLDER}/Release/graphics" "${ROMFS_DIR}/ruby/app/graphics"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/psdk" "${ROMFS_DIR}/ruby/app/psdk"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/Saves" "${ROMFS_DIR}/ruby/app/Saves"
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_FOLDER}/Release/Game.rb"
            "${ROMFS_DIR}/ruby/app/"
    # ~~~
    COMMAND ${CMAKE_COMMAND} -E touch ${ROMFS_DIR}/ruby/app/.demo_copied
    COMMENT "Populating romfs with the compiled project"
  )
else()
  # !!! Caution: this block is about loading the uncompiled project, which is an
  # experimental dev feature !!! The goal is to allow editing the ruby files
  # directly in the romfs folder to experiment, use with care It stills requires
  # the `Release` folder to exists
  add_custom_command(
    OUTPUT ${ROMFS_DIR}/ruby/app/.demo_copied
    # ~~~
    # Project files (Data, graphics, audio, scripts, etc.)
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/audio"
            "${ROMFS_DIR}/ruby/app/audio"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/Data"
            "${ROMFS_DIR}/ruby/app/Data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/Fonts"
            "${ROMFS_DIR}/ruby/app/Fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/graphics"
            "${ROMFS_DIR}/ruby/app/graphics"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/plugins"
            "${ROMFS_DIR}/ruby/app/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/scripts"
            "${ROMFS_DIR}/ruby/app/Scripts"
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_FOLDER}/Game.rb"
            "${ROMFS_DIR}/ruby/app/"
    # ~~~
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/audio" "${ROMFS_DIR}/ruby/app/audio"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/Data" "${ROMFS_DIR}/ruby/app/Data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/Fonts" "${ROMFS_DIR}/ruby/app/Fonts"
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory_if_different
      "${PROJECT_FOLDER}/Release/graphics" "${ROMFS_DIR}/ruby/app/graphics"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${PROJECT_FOLDER}/Release/psdk" "${ROMFS_DIR}/ruby/app/psdk"
    # ~~~
    # pokemonsdk scripts
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_FOLDER}/pokemonsdk"
            "${ROMFS_DIR}/ruby/app/pokemonsdk"
    # ~~~
    COMMAND ${CMAKE_COMMAND} -E touch ${ROMFS_DIR}/ruby/app/.demo_copied
    COMMENT "Populating romfs with the compiled project"
  )
endif()

add_custom_target(psdk_demo_romfs DEPENDS ${ROMFS_DIR}/ruby/app/.demo_copied)
add_dependencies(
  psdk_demo_romfs psdk_demo_base_romfs
) # Make sure base romfs is synced before demo files
add_dependencies(psdk_demo psdk_demo_romfs)

# ##############################################################################
# CSV gem
# ##############################################################################
FetchContent_Declare(
  csv_gem
  URL https://github.com/ruby/csv/archive/refs/tags/v3.3.5.tar.gz
  URL_HASH
    SHA256=545a68c50e4afc71930ec31b557ea9e1667c634fe708354b886c4d766b0c57a8
)
FetchContent_GetProperties(csv_gem)
if(NOT csv_gem_POPULATED)
  FetchContent_Populate(csv_gem)
endif()

add_custom_command(
  OUTPUT ${ROMFS_DIR}/ruby/site_ruby/csv.rb
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${csv_gem_SOURCE_DIR}/lib"
          "${ROMFS_DIR}/ruby/site_ruby"
  COMMENT "Populating romfs with csv gem"
)

add_custom_target(csv_gem_romfs DEPENDS ${ROMFS_DIR}/ruby/site_ruby/csv.rb)
add_dependencies(
  csv_gem_romfs psdk_demo_base_romfs
) # Make sure base romfs is synced before adding csv gem
add_dependencies(psdk_demo csv_gem_romfs)

# ##############################################################################
# Ruby stdlib setup
# ##############################################################################

# Now setup ruby stdlib - it will layer on top
ruby_switch_setup_romfs(psdk_demo ${ROMFS_DIR})

# Make ruby stdlib sync wait for base romfs
add_dependencies(psdk_demo_ruby_stdlib_romfs psdk_demo_base_romfs)

# ##############################################################################
# Final setup
# ##############################################################################

add_nro_target(psdk_demo)
