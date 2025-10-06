cmake_minimum_required(VERSION 3.14)

set(BUILD_NUMBER $ENV{BUILD_NUMBER})
if(NOT BUILD_NUMBER)
    set(BUILD_NUMBER "SNAPSHOT")
endif()

project(pipo-hse 
    VERSION 0.0.${BUILD_NUMBER}
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(pipo-hse src/main.cpp)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
# For Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests_run tests/test_main.cpp)
target_link_libraries(tests_run GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests_run)
