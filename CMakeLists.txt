cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(naive-string)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test-string
  test_string.cpp
)

target_link_libraries(
  test-string
  GTest::gtest_main
)

add_executable(
    test-stl-string
    test_string.cpp
)
target_compile_definitions(test-stl-string PRIVATE TEST_STL_STRING)

target_link_libraries(
  test-stl-string
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test-string)
gtest_discover_tests(test-stl-string)

add_subdirectory(benchmark)

add_executable(
    benchmark-string
    benchmark_string.cpp
)
target_link_libraries(benchmark-string benchmark::benchmark)

add_executable(
    benchmark-stl-string
    benchmark_string.cpp
)
target_compile_definitions(benchmark-stl-string PRIVATE TEST_STL_STRING)
target_link_libraries(benchmark-stl-string benchmark::benchmark)
