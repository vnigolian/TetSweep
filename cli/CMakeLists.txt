

cmake_minimum_required(VERSION 3.19)


if(NOT TARGET argparse)
    FetchContent_Declare(argparse
            GIT_REPOSITORY  https://github.com/p-ranav/argparse.git
            #SOURCE_DIR "${EXTERNAL_DIR}/argparse"
    )
    FetchContent_MakeAvailable(argparse)
endif()

include_directories(../include)
#include_directories(${EXTERNAL_DIR}/argparse/include)

add_executable (tet_weave tet_weave.cc)
target_link_libraries(tet_weave  argparse)
