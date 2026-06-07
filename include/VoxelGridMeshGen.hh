#pragma once

#include "TetMesh.hh"


namespace tet_weave{


    class VoxelGridMeshGen
    {
    public:

        // NOTE: full KH by default
        static TetMesh generate_Furchs_knotted_hole(int hole_length = 10) {

            //hard-coded values for this specific example
            VoxelGridMeshGen generator(9,7,5);

            //std::cout<<" pre-digging holes..."<<std::endl;
            switch(hole_length){

                //NOTE: no breaks because it's additive.
                case 10:{
                    generator.dig_hole(5,3,1,
                                       5,3,3);

                }
                case 9:{
                    generator.dig_hole(5,1,1,
                                       5,3,1);

                }
                case 8:{
                    generator.dig_hole(1,1,1,
                                       5,1,1);

                }
                case 7:{
                    generator.dig_hole(1,5,1,
                                       1,1,1);

                }
                case 6:{
                    generator.dig_hole(7,5,1,
                                       1,5,1);

                }
                case 5:{
                    generator.dig_hole(7,1,1,
                                       7,5,1);

                }
                case 4:{
                    generator.dig_hole(7,1,3,
                                       7,1,1);

                }
                case 3:{
                    generator.dig_hole(3,1,3,
                                       7,1,3);

                }
                case 2:{
                    generator.dig_hole(3,3,3,
                                       3,1,3);
                }
                case 1:{
                    generator.dig_hole(3,3,0,
                                       3,3,3);
                }
                case 0:{

                    break;
                }
                default:{
                    assert(false && "unhandled Furch's knotted hole length");
                }
            }

            //std::cout<<" generating block..."<<std::endl;
            generator.tetrahedralize_voxels();

            //std::cout<<" generated Furch's ball."<<std::endl;
            //std::cout<<" -> vertices count = "<<generator.mesh_.n_vertices()<<std::endl;
            //std::cout<<" ->    cells count = "<<generator.mesh_.n_cells()<<std::endl;

            return generator.mesh_;
        }


        static TetMesh generate_voxel_grid_mesh(int width, int height, int depth) {

            VoxelGridMeshGen generator(width, height, depth);

            //std::cout<<" generating block..."<<std::endl;
            generator.tetrahedralize_voxels();

            //std::cout<<" generated voxel grid"<<std::endl;
            //std::cout<<" -> vertices count = "<<generator.mesh_.n_vertices()<<std::endl;
            //std::cout<<" ->    edges count = "<<generator.mesh_.n_edges()<<std::endl;
            //std::cout<<" ->    faces count = "<<generator.mesh_.n_faces()<<std::endl;
            //std::cout<<" ->    cells count = "<<generator.mesh_.n_cells()<<std::endl;

            return generator.mesh_;
        }



    private:

        VoxelGridMeshGen(int width, int height, int depth) : width_(width), height_(height), depth_(depth){}

        void tetrahedralize_voxels(){

            //std::cout<<" - generating vertices..."<<std::endl;
            generate_vertices();

            //std::cout<<" - generating cubes..."<<std::endl;
            int voxel_count(0);
            for(auto x(0); x < width_; x++){
                for(auto y(0); y < height_; y++){
                    for(auto z(0); z < depth_; z++){
                        if(!is_digged(x,y,z)){
                            generate_five_tet_voxel(x,y,z, (x+y+z) % 2);
                            voxel_count++;
                        }
                    }
                }
            }
            //std::cout<<" -> generated "<<voxel_count<<" cubes"<<std::endl;
        }


        void generate_vertices(){

            for(double z(0); z <= depth_; z++){
                for(double y(0); y <= height_; y++){
                    for(double x(0); x <= width_; x++){
                        auto v = mesh_.add_vertex({x,y,-z});
                        //std::cout<<" -- added vertex "<<v<<" at "<<mesh_.vertex(v)<<std::endl;
                    }
                }
            }
        }

        /* (x,y,z) = voxel coordinates */
        void generate_five_tet_voxel(int x, int y, int z,
                           bool orientation){

    //std::cout<<" -- generating cube at "<<x<<", "<<y<<", "<<z<<" with orientation "<<orientation<<std::endl;
    const int i = coordinates_to_corner_vertex_idx(x,y,z);
    //std::cout<<" -- corner vertex idx = "<<i<<std::endl;

    const int w(width_ + 1), h(height_ + 1), wh(w * h);

    if(orientation){
        mesh_.add_cell(VertexHandle(     w     + i),
                       VertexHandle(wh + w + 1 + i),
                       VertexHandle(wh         + i),
                       VertexHandle(wh + w     + i));

        mesh_.add_cell(VertexHandle(     w     + i),
                       VertexHandle(         1 + i),
                       VertexHandle(wh + w + 1 + i),
                       VertexHandle(     w + 1 + i));

        mesh_.add_cell(VertexHandle(             i),
                       VertexHandle(         1 + i),
                       VertexHandle(wh         + i),
                       VertexHandle(     w     + i));

        mesh_.add_cell(VertexHandle(     w     + i),
                       VertexHandle(         1 + i),
                       VertexHandle(wh         + i),
                       VertexHandle(wh + w + 1 + i));

        mesh_.add_cell(VertexHandle(         1 + i),
                       VertexHandle(wh     + 1 + i),
                       VertexHandle(wh         + i),
                       VertexHandle(wh + w + 1 + i));
    }else{

        mesh_.add_cell(VertexHandle(           + i),
                       VertexHandle(     w + 1 + i),
                       VertexHandle(wh + w     + i),
                       VertexHandle(     w     + i));

        mesh_.add_cell(VertexHandle(             i),
                       VertexHandle(         1 + i),
                       VertexHandle(wh     + 1 + i),
                       VertexHandle(     w + 1 + i));

        mesh_.add_cell(VertexHandle(             i),
                       VertexHandle(wh     + 1 + i),
                       VertexHandle(wh         + i),
                       VertexHandle(wh + w     + i));

        mesh_.add_cell(VertexHandle(     w + 1 + i),
                       VertexHandle(wh     + 1 + i),
                       VertexHandle(wh + w     + i),
                       VertexHandle(wh + w + 1 + i));

        mesh_.add_cell(VertexHandle(             i),
                       VertexHandle(     w + 1 + i),
                       VertexHandle(wh     + 1 + i),
                       VertexHandle(wh + w     + i));
    }
}


        void dig_hole(int start_x, int start_y, int start_z,
                      int end_x,   int end_y,   int end_z){

            /*std::cout<<" - digging hole from "<<start_x<<", "<<start_y<<", "<<start_z<<
                       " to "<<end_x<<", "<<end_y<<", "<<end_z<<std::endl;*/

            int from_x = std::min(start_x, end_x);
            int to_x   = std::max(start_x, end_x);

            int from_y = std::min(start_y, end_y);
            int to_y   = std::max(start_y, end_y);

            int from_z = std::min(start_z, end_z);
            int to_z   = std::max(start_z, end_z);

            for(auto x(from_x); x <= to_x; x++){
                for(auto y(from_y); y <= to_y; y++){
                    for(auto z(from_z); z <= to_z; z++){
                        digged_voxels_.push_back(coordinates_to_voxel_idx(x,y,z));
                        //std::cout<<" -- digged voxel "<<coordinates_to_voxel_idx(x,y,z)<<
                        //           " at "<<x<<", "<<y<<", "<<z<<std::endl;
                    }
                }
            }
        }


        bool is_digged(int x, int y, int z){
            auto idx = coordinates_to_voxel_idx(x,y,z);
            for(auto vx: digged_voxels_){
                if(vx == idx){
                    return true;
                }
            }
            return false;
        }

        int coordinates_to_corner_vertex_idx(int x, int y, int z){

            return x + y * (width_ + 1) + z * (width_ + 1) * (height_ + 1);
        }


        int coordinates_to_voxel_idx(int x, int y, int z){
            return x + y * width_ + z * width_ * height_;
        }


        TetMesh mesh_;
        std::vector<int> digged_voxels_;
        const int width_;
        const int height_;
        const int depth_;
    };
}




