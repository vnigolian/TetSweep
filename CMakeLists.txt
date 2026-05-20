include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

project(TetMeshGenerator)
cmake_minimum_required(VERSION 3.19)


set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
#set (CMAKE_BUILD_TYPE Release)

#set(CHOLMOD_INCLUDE_DIR /opt/local/include/)
#include(${CMAKE_SOURCE_DIR}/cmake/FindSUITESPARSE.cmake)

#message(CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH})
#set(CMAKE_PREFIX_PATH "/opt/local/include/")
#message(CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH})

#NOTE: if you get CHOLMOD issues again, do this:
# export OpenMP_ROOT=$(brew --prefix)/opt/libomp

if(NOT TARGET argparse)
    FetchContent_Declare(argparse
            GIT_REPOSITORY  https://github.com/p-ranav/argparse.git
            SOURCE_DIR "${EXTERNAL_DIR}/argparse"
    )
    FetchContent_MakeAvailable(argparse)
endif()


#message("Copying parameters file to build folder...")
#configure_file("SAE-parameters-brutus.json" ${CMAKE_CURRENT_BINARY_DIR}/SAE-parameters.json COPYONLY)

FILE(GLOB SOURCE_FILES
        src/*.cc
        )

include_directories(src/)


add_library (TetMeshGenerator ${SOURCE_FILES})
target_link_libraries (TetMeshGenerator PUBLIC
        argparse
       # acutest
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(" enabling sanitizers")
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)

    target_compile_options(TetMeshGenerator PRIVATE -fsanitize=address -fsanitize=undefined)
    target_link_options(TetMeshGenerator PRIVATE -fsanitize=address -fsanitize=undefined)
endif()

add_subdirectory(cli)
add_subdirectory(unit-tests)


