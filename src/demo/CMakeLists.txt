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
  Ruby::ruby
  lognx
)

target_compile_definitions(
  psdk_demo
  PRIVATE
    RUBY_TMP_DIR="sdmc:/switch/.tmp_ruby"
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
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
            "${ROMFS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${_romfs_stamp}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/romfs/ruby/app/main.rb" # re-run if main.rb changes
    COMMENT "Syncing base romfs"
)

add_custom_target(psdk_demo_base_romfs
    DEPENDS ${_romfs_stamp}
)

# ##############################################################################
# PSDK demo setup
# ##############################################################################
FetchContent_Declare(pokemonsdk
    GIT_REPOSITORY git@github.com:maxmarsc/switch-pokemonsdk.git
    GIT_TAG switch-support--no-yarb-rebase
    GIT_SHALLOW OFF
)
FetchContent_GetProperties(pokemonsdk)
if(NOT pokemonsdk_POPULATED)
    FetchContent_Populate(pokemonsdk)
endif()

FetchContent_Declare(psdk_demo_src
    URL https://github.com/PokemonWorkshop/PSDKTechnicalDemo/releases/download/1.0.24/PSDK_Technical_Demo_1-0-24.zip
    URL_HASH SHA256=894a49dbd3bbae17c28033c8527bb07f4bbcd50aa79bc615641c3f210650dbc4
)
FetchContent_GetProperties(psdk_demo_src)
if(NOT psdk_demo_src_POPULATED)
    FetchContent_Populate(psdk_demo_src)
endif()

add_custom_command(
    OUTPUT ${ROMFS_DIR}/ruby/app/.demo_copied
    # ~~~
    # Demo files (Data, graphics, audio, scripts, etc.)
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/audio" "${ROMFS_DIR}/ruby/app/audio"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/Data" "${ROMFS_DIR}/ruby/app/Data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/Fonts" "${ROMFS_DIR}/ruby/app/Fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/graphics" "${ROMFS_DIR}/ruby/app/graphics"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/plugins" "${ROMFS_DIR}/ruby/app/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/Saves" "${ROMFS_DIR}/ruby/app/Saves"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${COMPILED_DEMO_FOLDER}/scripts" "${ROMFS_DIR}/ruby/app/Scripts"
    # COMMAND ${CMAKE_COMMAND} -E copy
    #         "${COMPILED_DEMO_FOLDER}/Game.rb" "${ROMFS_DIR}/ruby/app/"
    # ~~~
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${COMPILED_DEMO_FOLDER}/Release/audio" "${ROMFS_DIR}/ruby/app/audio"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${COMPILED_DEMO_FOLDER}/Release/Data" "${ROMFS_DIR}/ruby/app/Data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${COMPILED_DEMO_FOLDER}/Release/Fonts" "${ROMFS_DIR}/ruby/app/Fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${COMPILED_DEMO_FOLDER}/Release/graphics" "${ROMFS_DIR}/ruby/app/graphics"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${COMPILED_DEMO_FOLDER}/Release/Saves" "${ROMFS_DIR}/ruby/app/Saves"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${COMPILED_DEMO_FOLDER}/Release/Game.rb" "${ROMFS_DIR}/ruby/app/"
    # ~~~
    # Demo files (Data, graphics, audio, scripts, etc.)
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    #         "${psdk_demo_src_SOURCE_DIR}"
    #         "${ROMFS_DIR}/ruby/app"
    # ~~~
    # pokemonsdk scripts
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${pokemonsdk_SOURCE_DIR}"
            "${ROMFS_DIR}/ruby/app/pokemonsdk"
    COMMAND ${CMAKE_COMMAND} -E touch ${ROMFS_DIR}/ruby/app/.demo_copied
    COMMENT "Populating romfs with demo and pokemonsdk"
)

add_custom_target(psdk_demo_romfs DEPENDS ${ROMFS_DIR}/ruby/app/.demo_copied)
add_dependencies(psdk_demo_romfs psdk_demo_base_romfs) # Make sure base romfs is synced before demo files
add_dependencies(psdk_demo psdk_demo_romfs)

# ##############################################################################
# CSV gem
# ##############################################################################
FetchContent_Declare(csv_gem
    URL https://github.com/ruby/csv/archive/refs/tags/v3.3.5.tar.gz
    URL_HASH SHA256=545a68c50e4afc71930ec31b557ea9e1667c634fe708354b886c4d766b0c57a8
)
FetchContent_GetProperties(csv_gem)
if(NOT csv_gem_POPULATED)
    FetchContent_Populate(csv_gem)
endif()

add_custom_command(
    OUTPUT ${ROMFS_DIR}/ruby/site_ruby/csv.rb
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${csv_gem_SOURCE_DIR}/lib"
            "${ROMFS_DIR}/ruby/site_ruby"
    COMMENT "Populating romfs with csv gem"
)

add_custom_target(csv_gem_romfs DEPENDS ${ROMFS_DIR}/ruby/site_ruby/csv.rb)
add_dependencies(csv_gem_romfs psdk_demo_base_romfs) # Make sure base romfs is synced before adding csv gem
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