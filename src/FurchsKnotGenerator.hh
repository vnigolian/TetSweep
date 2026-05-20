#pragma once

#include "includes.hh"

#if 0



class FurchsKnotGenerator
{
public:

    static TetrahedralMesh generate_mesh(int hole_length);

    static const int WIDTH  = 9;
    static const int HEIGHT = 7;
    static const int DEPTH  = 5;


    static TetrahedralMesh generate_voxel_grid_mesh(int width, int height, int depth);


private:

    FurchsKnotGenerator(int width = WIDTH,
                        int height = HEIGHT,
                        int depth = DEPTH);

    /* NOTE: those are made variable in case I need this method somewhere else */
    void generate_five_tet_voxel();

    void generate_vertices();

    /* (x,y,z) = voxel coordinates */
    void generate_cube(int x, int y, int z,
                       bool orientation);

    void dig_hole(int start_x, int start_y, int start_z,
                  int end_x,   int end_y,   int end_z);


    bool is_digged(int x, int y, int z);

    int coordinates_to_corner_vertex_idx(int x, int y, int z);

    int coordinates_to_voxel_idx(int x, int y, int z);



    TetrahedralMesh mesh_;
    std::vector<int> digged_voxels_;
    const int width_;
    const int height_;
    const int depth_;

};

#endif


