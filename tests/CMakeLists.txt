include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0)

FetchContent_MakeAvailable(googletest)

add_library(counters counters.cpp)
target_link_libraries(counters simply)

add_executable(stream stream.cpp)
target_link_libraries(stream simply GTest::gtest_main)
