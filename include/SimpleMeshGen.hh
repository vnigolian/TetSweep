
#pragma once

#include "TetMesh.hh"

namespace tet_sweep {

    namespace SimpleMeshGen {

        TetMesh generate_minimal_non_star_shaped_mesh(){

            TetMesh mesh;

            auto vi = mesh.add_vertex({0,0,0});
            auto vb = mesh.add_vertex({0,0,-1});
            auto v1 = mesh.add_vertex({-1,-1,0});
            auto v2 = mesh.add_vertex({-1,-3,0});
            auto v3 = mesh.add_vertex({-2,1, 0});
            auto v4 = mesh.add_vertex({2,1,  0});
            auto v5 = mesh.add_vertex({1,-3, 0});
            auto v6 = mesh.add_vertex({1,-1, 0});
            auto vt = mesh.add_vertex({0,0, 1});

            mesh.add_cell({v2, v1, vb, vi});
            mesh.add_cell({v3, v2, vb, vi});
            mesh.add_cell({v4, v3, vb, vi});
            mesh.add_cell({v5, v4, vb, vi});
            mesh.add_cell({v6, v5, vb, vi});
            mesh.add_cell({v1, v6, vb, vi});

            mesh.add_cell({v1, v2, vt, vi});
            mesh.add_cell({v2, v3, vt, vi});
            mesh.add_cell({v3, v4, vt, vi});
            mesh.add_cell({v4, v5, vt, vi});
            mesh.add_cell({v5, v6, vt, vi});
            mesh.add_cell({v6, v1, vt, vi});

            mesh.mark_as_boundary(vi, false);

            return mesh;
        }


        TetMesh generate_minimal_non_star_shaped_domain_mesh() {

            auto mesh = generate_minimal_non_star_shaped_mesh();

            mesh.set_vertex(VertexHandle(2), {-1, -3,0});
            mesh.set_vertex(VertexHandle(3), {-2, -3,0});
            mesh.set_vertex(VertexHandle(6), {2, -3, 0});
            mesh.set_vertex(VertexHandle(7), {1, -3, 0});

            return mesh;
        }


        TetMesh generate_rod_mesh(int length,
                                  double axial_scaling = 1.0,
                                  double torsion_rad = 0.0){

            if(length < 1) {
                //std::cout<<" - warning, clamped mesh length to 1"<<std::endl;
                length = 1;
            }

            TetMesh mesh;
            //the only interior vertex
            auto v0 = mesh.add_vertex({0.0, 0.0, length * 0.5});
            mesh.mark_as_boundary(v0, false);

            //add vertices
            /* b---c
             * | / |
             * a---d /*/
            for(int i(0); i <= length; i++){
                mesh.add_vertex({-0.5, -0.5, i});
                mesh.add_vertex({ 0.5, -0.5, i});
                mesh.add_vertex({ 0.5,  0.5, i});
                mesh.add_vertex({-0.5,  0.5, i});
            }
            //std::cout<<" - added "<<mesh.n_vertices()<<" vertices"<<std::endl;

            //add the first cells
            mesh.add_cell({VertexHandle(1), VertexHandle(2), VertexHandle(3), v0});
            mesh.add_cell({VertexHandle(1), VertexHandle(3), VertexHandle(4), v0});


            //std::cout<<" - added first face"<<std::endl;
            for(int i(0); i < length; i++){

                int c = i * 4;
                //right
                mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 3 + 1), VertexHandle(c + 4 + 1), v0});
                mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 3 + 1), VertexHandle(c + 7 + 1), v0});

                //top
                mesh.add_cell({VertexHandle(c + 3 + 1), VertexHandle(c + 2 + 1), VertexHandle(c + 6 + 1), v0});
                mesh.add_cell({VertexHandle(c + 3 + 1), VertexHandle(c + 6 + 1), VertexHandle(c + 7 + 1), v0});

                //left
                mesh.add_cell({VertexHandle(c + 2 + 1), VertexHandle(c + 1 + 1), VertexHandle(c + 5 + 1), v0});
                mesh.add_cell({VertexHandle(c + 2 + 1), VertexHandle(c + 5 + 1), VertexHandle(c + 6 + 1), v0});

                //top
                mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 5 + 1), VertexHandle(c + 1 + 1), v0});
                mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 4 + 1), VertexHandle(c + 5 + 1), v0});
            }

            int c = (length -1) * 4;
            //std::cout<<" c = "<<c<<std::endl;
            mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 7 + 1), VertexHandle(c + 6 + 1), v0});
            mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 6 + 1), VertexHandle(c + 5 + 1), v0});

            if (torsion_rad != 0.0) {
                double d_rot = torsion_rad / length;

                for(int i(0); i <= length; i++){
                    int start_idx = 4 * i + 1;
                    double theta = d_rot * i;
                    //std::cout<<" theta_"<<i<<" = "<<theta<<std::endl;
                    for(int j(0); j<4; j++){
                        VertexHandle v(start_idx + j);
                        auto pos = mesh.vertex(v);

                        auto rot_pos = pos.rotate({0,0,1}, theta);
                        //std::cout<<" rotated vertex "<<v.idx()<<" from "<<pos<<" to "<<rot_pos<<std::endl;
                        mesh.set_vertex(v, rot_pos);
                    }
                }
            }

            if (axial_scaling != 1.0) {
                for (auto vh: mesh.vertices()) {
                    auto pos = mesh.vertex(vh);
                    pos[2] *= axial_scaling;
                    mesh.set_vertex(vh, pos);
                }
            }

            //std::cout<<" - added last face"<<std::endl;
            return mesh;
        }



        TetMesh generate_sine_mesh(int length,
                                   double axial_scaling = 1.0,
                                   double torsion_rad=0.0,
                                   double period=1.0,
                                   double amplitude=1.0){

            TetMesh mesh = generate_rod_mesh(length, axial_scaling, torsion_rad);

            //z_scale = z_scale / length;
            //double x_period = 0.0;

            for(auto v: mesh.vertices()){
                auto pos = mesh.vertex(v);
                //std::cout<<" - v"<<v<<" at "<<pos[0]<<std::endl;
                auto x = pos[0];
                auto y = pos[1];
                auto z = pos[2];

                auto theta = 2 * M_PI * z / (length * axial_scaling);

                //std::cout<<" - x = "<<x<<std::endl;
                //y += y_scale * std::sin(period * sin_z);
                //std::cout<<" - sine(T sin_z) = "<<std::sin(period * sin_z)<<std::endl;
                y += amplitude * std::sin(period * theta);
                //std::cout<<" - x + sine(T sin_z) = "<<x<<std::endl;
                //std::cout<<" ------------------------- "<<std::endl;

                //auto scaled_z = sin_z * z_scale;
                mesh.set_vertex(v, {x, y, z});
                //std::cout<<" -- scaled x: "<<scaled_x<<std::endl;
            }

            return mesh;
        }




        TetMesh generate_spiral_mesh(int length,
                                     double torsion_rad = 0.0,
                                     double x_scale = 1.0,
                                     double turn_count = 1.0){

            double length_by_turn = (double)length / turn_count;

            //std::cout<<" len by turn: "<<length_by_turn<<", turn count: "<<turn_count<<std::endl;

            auto mesh = generate_rod_mesh(length, 1.0, torsion_rad);

            double dr = 2.0 * (double)turn_count / (double)length;
            //dr = 2.0 / length_by_turn;

            double dx = x_scale / mesh.n_vertices();
            int i(0);
            for(auto v: mesh.vertices()){
                auto pos = mesh.vertex(v);
                auto x = pos[0];
                auto y = pos[1];
                auto z = pos[2];

                double t =  (z * 2 * M_PI) / length_by_turn;
                double r = 1.0 + y + z * dr;
                Vec3d spos = {x + i * dx, r * std::cos(t), r * std::sin(t)};
                //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<spos<<std::endl;
                /*if(!y) {
                    std::cout << " - x = " << x << ", r = " << r << std::endl;
                }*/

                mesh.set_vertex(v, spos);
                i++;
            }
            return mesh;
        }


        /** @arg range: the full trefoil knot goes from t=0 to 1 */
        TetMesh generate_trefoil_knot_mesh(int length,
                                           double torsion_rad,
                                           double range){


            //double length = length_by_turn * turn_count;
            auto mesh = generate_rod_mesh(length, 1.0, torsion_rad);

            //double dr = 2.0 * (double)turn_count / (double)length;
            //dr = 2.0 / length_by_turn;

            //std::cout<<" - curve points: "<<std::endl;
            std::vector<Vec3d> curve_points;
            for(int i(0); i <= length; i++){

                double t = range * (2.0 * M_PI * i) / (length);

                Vec3d curve_pos = { std::sin(t) + 2 * std::sin(2 * t),
                                         std::cos(t) - 2 * std::cos(2 * t),
                                         -std::sin(3 * t)};

                curve_points.push_back(curve_pos);
                //std::cout<<" - point "<<i<<" for t = "<<t<<" : "<<curve_pos<<std::endl;
            }

            double orthogonal_scaling(0.5);
            int i(0);
            for(auto v: mesh.vertices()){
                if(!i){
                    //std::cout<<" - skipping interior vertex"<<std::endl;
                    i++;
                    continue;
                }
                //std::cout<<" ---------------------------------------------------------- "<<std::endl;
                //std::cout<<" - handling vertex "<<v<<std::endl;
                int point_idx = (i - 1) / 4;
                int next_point_idx = point_idx + 1;
                bool last_point(false);
                if(next_point_idx >= curve_points.size()){
                    last_point = true;
                    //std::cout<<" -> last point"<<std::endl;
                    next_point_idx = point_idx;
                    point_idx = next_point_idx - 1;
                }

                //std::cout<<" -      point idx: "<<point_idx<<std::endl;
                //std::cout<<" - next point idx: "<<next_point_idx<<std::endl;

                auto pos = mesh.vertex(v);
                auto x = pos[0];
                auto y = pos[1];
                auto z = pos[2];

                auto curve_pos = curve_points[point_idx];
                auto next_curve_pos = curve_points[next_point_idx];
                //std::cout<<" - next curve point: "<<next_curve_pos<<std::endl;

                //tangent
                auto t = -(next_curve_pos - curve_pos).normalized();
                if(last_point){
                    curve_pos = next_curve_pos;
                }
                //std::cout<<" -      curve point: "<<curve_pos<<std::endl;
                //std::cout<<" - tangent: "<<t<<std::endl;

                // first perpendicular vector (with projection)
                auto t_proj = t;
                double sign = t_proj[2] > 0 ? 1 : -1;
                t_proj[2] = 0;
                auto tp1 = sign * t_proj.cross(t);
                tp1.normalize();
                //std::cout<<" - orthogonal axis 1: "<<tp1<<std::endl;

                auto tp2 = tp1.cross(t);
                tp2.normalize();
                /*std::cout<<" - orthogonal axis 2: "<<tp2<<std::endl;

                std::cout<<" - dot prod check 1: "<<tp1.dot(t)<<std::endl;
                std::cout<<" - dot prod check 2: "<<tp2.dot(t)<<std::endl;*/

                Vec3d new_pos = curve_pos + orthogonal_scaling * x * tp1 + orthogonal_scaling * y * tp2;
                //std::cout<<" - new pos: "<<new_pos<<std::endl;

                //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<new_pos<<std::endl;

                mesh.set_vertex(v, new_pos);
                i++;
            }
            //mesh.delete_vertex(VertexHandle(0));
            //mesh.collect_garbage();

            return mesh;
        }



            // Helper: global handle for boundary vertex (i,j,k)
            VertexHandle layer_vertex_id(int u_dim, int v_dim, 
                                         int i, int j, int k) {
                return VertexHandle(k * ((u_dim+1) * (v_dim+1)) + i * (v_dim + 1) + j + 1);
            };

        TetMesh generate_layer_mesh(int u_steps, int v_steps) {
            assert(u_steps >= 1 && "generate_layer_mesh: u_steps must be >= 1");
            assert(v_steps >= 1 && "generate_layer_mesh: v_steps must be >= 1");

            TetMesh mesh;

            // Interior vertex at centroid of the grid — added first so it gets index 0.
            // Grid spans [0, u_steps] × [0, v_steps] with z=0.
            auto vi = mesh.add_vertex(u_steps * 0.5, v_steps * 0.5, 0.5);
            mesh.mark_as_boundary(vi, false);

            // Boundary vertices — row-major: vertex(i,j) = 1 + i*(v_steps+1) + j
            for(int k(0); k<2;k++)
                for (int i = 0; i <= u_steps; ++i)
                    for (int j = 0; j <= v_steps; ++j)
                        mesh.add_vertex(i, j, k);
            


            // Each quad cell → 2 tets via the interior vertex.
            // Quad corners: v00=(i,j), v10=(i+1,j), v01=(i,j+1), v11=(i+1,j+1)
            // Triangle split: (v00,v10,v11) and (v00,v11,v01)
            // Winding: interior vertex vi is at z=0 (same plane), but after
            // deformation it will be offset. We order so that (vi, tri) gives
            // a positive determinant when vi is below the surface.
            for(int k (0); k<2;k++){
                for (int i = 0; i < u_steps; ++i) {
                    for (int j = 0; j < v_steps; ++j) {
                        auto v00 = layer_vertex_id(u_steps, v_steps, i,   j  , k);
                        auto v10 = layer_vertex_id(u_steps, v_steps, i+1, j  , k);
                        auto v01 = layer_vertex_id(u_steps, v_steps, i,   j+1, k);
                        auto v11 = layer_vertex_id(u_steps, v_steps, i+1, j+1, k);

                        // tet0: vi, v00, v11, v10  (right-hand rule: face v00,v11,v10 points toward vi)
                        if(k){
                            mesh.add_cell({vi, v00, v11, v10});
                            mesh.add_cell({vi, v00, v01, v11});
                        }else{
                            mesh.add_cell({vi, v00, v10, v11});
                            mesh.add_cell({vi, v00, v11, v01});
                        }
                    }
                }
            }
            //std::cout<<" - layer mesh size step 1: "<<mesh.n_cells()<<std::endl;

            for(int j: {0, v_steps}){
                for(int i(0); i<u_steps; i++){

                    auto v00 = layer_vertex_id(u_steps, v_steps, i,   j, 0);
                    auto v10 = layer_vertex_id(u_steps, v_steps, i+1, j, 0);
                    auto v01 = layer_vertex_id(u_steps, v_steps, i,   j, 1);
                    auto v11 = layer_vertex_id(u_steps, v_steps, i+1, j, 1);

                    if(j){
                        mesh.add_cell({vi, v00, v11, v10});
                        mesh.add_cell({vi, v00, v01, v11});
                    }else{
                        mesh.add_cell({vi, v00, v10, v11});
                        mesh.add_cell({vi, v00, v11, v01});
                    }
                }
            }
            //std::cout<<" - layer mesh size step 2: "<<mesh.n_cells()<<std::endl;

            for(int i: {0, u_steps}){
                    //std::cout<<" ------- adding cells on side "<<i<<std::endl;
                for(int j(0); j<v_steps; j++){

                    //std::cout<<" ------ j = "<<j<<std::endl;

                    auto v00 = layer_vertex_id(u_steps, v_steps, i, j,   0);
                    auto v10 = layer_vertex_id(u_steps, v_steps, i, j+1, 0);
                    auto v01 = layer_vertex_id(u_steps, v_steps, i, j,   1);
                    auto v11 = layer_vertex_id(u_steps, v_steps, i, j+1, 1);

                    //std::cout<<" - indices: {"<<v00<<", "<<v10<<", "<<v01<<","<<v11<<"}"<<std::endl;

                    if(i){
                        mesh.add_cell({vi, v00, v10, v11});
                        mesh.add_cell({vi, v00, v11, v01});
                    }else{
                        mesh.add_cell({vi, v00, v11, v10});
                        mesh.add_cell({vi, v00, v01, v11});
                    }
                }
            }
            //std::cout<<" - layer mesh size step 3: "<<mesh.n_cells()<<std::endl;

            return mesh;
        }
    }
}