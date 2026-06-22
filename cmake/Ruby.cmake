# ##############################################################################
# Ruby
# ##############################################################################
include(FetchContent)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#   set(SWITCH_RUBY_DEBUG ON)
# else()
#   set(SWITCH_RUBY_DEBUG OFF)
# endif()

FetchContent_Declare(switch_ruby
    GIT_REPOSITORY git@github.com:maxmarsc/switch-ruby
    GIT_TAG        fix/debugger-prism-bleed
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(switch_ruby)