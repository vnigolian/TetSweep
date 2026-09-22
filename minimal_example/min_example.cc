#include <iostream>

#include "VoxelGridMeshGen.hh"
#include "ParametricMeshGen.hh"
#include "SimpleMeshGen.hh"

int main(){

    auto m1 = tet_sweep::SimpleMeshGen::generate_spiral_mesh(30);
    auto m2 = tet_sweep::ParametricMeshGen::generate_parametric_mesh(tet_sweep::circle, {2.0}, 0.0, 0.5, 20, 2.0);
    auto m3 = tet_sweep::VoxelGridMeshGen::generate_Furchs_knotted_hole();

    //export a spiral surface mesh
    tet_sweep::write_to_file(m1, "spiral.obj", true);

    //export the knotted hole mesh
    tet_sweep::write_to_file(m3, "knotted_hole.ovm");

    return 0;
}