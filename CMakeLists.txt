# Declare project
cmake_minimum_required(VERSION 3.28)
project(switch-psdk)

# We set the C standard to 11.
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable C++20 module dependency scanning
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# CMake modules

# We have to add the cmake directory to have access to the Switch tools and also
# find_package for libnx.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Required libraries and utilities

# SwitchTools provide various helper functions for building Switch file formats.
include(SwitchTools)
include(SFML)
include(LiteCGSS)

# Find libnx, which is required by our application.
find_package(Libnx REQUIRED)
# Force the variables into the CACHE so SFML sees them as already found
set(LIBNX_FOUND
    TRUE
    CACHE BOOL "Forcing Libnx found for dependencies" FORCE
)

# ##############################################################################
# CLANGD
# ##############################################################################
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  include(FetchContent)
  FetchContent_Declare(
    arm_neon_clangd_fix
    GIT_REPOSITORY git@github.com:maxmarsc/arm_neon_clangd_fix.git
    GIT_TAG "feature/arm-acle"
    GIT_SHALLOW ON
  )
  FetchContent_MakeAvailable(arm_neon_clangd_fix)
endif()

add_subdirectory(templates)
add_subdirectory(src)
