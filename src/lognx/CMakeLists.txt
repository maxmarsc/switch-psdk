add_library(
  lognx STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lognx.cpp
)

target_include_directories(
  lognx
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
  lognx
  PUBLIC arm_neon_clangd_fix switch::libnx
)