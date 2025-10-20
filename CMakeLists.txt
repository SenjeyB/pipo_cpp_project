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

find_package(Boost 1.70 REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(pipo-hse 
    src/main.cpp
    src/server.cpp
    src/handlers/user_handler.cpp
    src/db/database.cpp
)

target_link_libraries(pipo-hse 
    ${Boost_LIBRARIES}
    nlohmann_json::nlohmann_json
    pqxx
)

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
