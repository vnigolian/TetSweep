

cmake_minimum_required(VERSION 3.19)


include_directories(../include)
#include_directories(${EXTERNAL_DIR}/argparse/include)

add_executable (tet_weave tet_weave.cc ArgsDispatch.cc)
target_link_libraries(tet_weave  argparse)
