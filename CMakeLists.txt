include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

project(TetWeave)
cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

include_directories(include/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(" enabling sanitizers")
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)

endif()

set(CLI_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/cli/tet_weave")

include(CTest)
enable_testing()

add_subdirectory(cli)
add_subdirectory(unit_tests)


