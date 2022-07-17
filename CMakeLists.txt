cmake_minimum_required(VERSION 3.23)
project(multithreaded-kiwi)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(src/engine)
add_subdirectory(src/bench)
add_subdirectory(src/tests)