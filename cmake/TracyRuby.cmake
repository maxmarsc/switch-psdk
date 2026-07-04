include(FetchContent)
FetchContent_Declare(
  tracy_ruby
  GIT_REPOSITORY git@github.com:maxmarsc/tracy-ruby.git
  GIT_TAG "main"
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tracy_ruby)