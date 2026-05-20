

cmake_minimum_required(VERSION 3.19)


add_executable (tet_mesh_generator tet_mesh_generator.cc)
target_link_libraries(tet_mesh_generator  TetMeshGenerator)
