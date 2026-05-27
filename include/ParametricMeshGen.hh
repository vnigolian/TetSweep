
#pragma once

#include "TetMesh.hh"

namespace tet_weave {

    namespace ParametricMeshGen {


        #if 0

        Eigen::Vector3d parametric_spiral_3D(argparse::ArgumentParser& parser, double t){
            double center_radius = parser.get<double>("--paraspiral-center-radius");
            int branches = parser.get<int>("--paraspiral-branches");
            double branch_length = parser.get<double>("--paraspiral-branch-len");
            double z_scale = parser.get<double>("--parametric-z-scale");

            double radius = (std::cos(branches * t) + 1.0 + center_radius);
            double angle = radius * branch_length;

            Eigen::Matrix2d rotation;
            rotation << std::cos(angle), -std::sin(angle),
                    std::sin(angle),  std::cos(angle);
            Eigen::Vector2d v({std::cos(t), std::sin(t)});

            Eigen::Vector2d point = radius * rotation * v;
            Eigen::Vector3d point_3d;
            point_3d << point[0], point[1], z_scale * t;

            return point_3d;
        }

        Eigen::Vector3d parametric_trefoil_knot(argparse::ArgumentParser& parser, double t){
            return { std::sin(t) + 2 * std::sin(2 * t),
                     std::cos(t) - 2 * std::cos(2 * t),
                     -std::sin(3 * t)};
        }


        Eigen::Vector3d parametric_complex_spiral(argparse::ArgumentParser& parser, double t){
            double a = parser.get<double>("--complex-spiral-a");
            double b = parser.get<double>("--complex-spiral-b");
            double c = parser.get<double>("--complex-spiral-c");
            double z_scale = parser.get<double>("--parametric-z-scale");

            return {std::cos(a * t) + std::cos(b * t) / 2.0 + std::sin(c * t) / 3.0,
                    std::sin(a * t) + std::sin(b * t) / 2.0 + std::cos(c * t) / 3.0,
                    z_scale * t};
        }


        void setup_parametric_mesh_with_ref(TriangleMesh& ref_mesh,
                                            TriangleMesh& mesh,
                                            ParametricCurve2D curve,
                                            argparse::ArgumentParser& parser){

            setup_parametric_mesh(ref_mesh, parametric_circle, parser);

            setup_parametric_mesh(mesh, curve, parser);

        }


        void setup_parametric_mesh(TriangleMesh& mesh,
                                   ParametricCurve2D curve,
                                   argparse::ArgumentParser& parser){

            int v_count = parser.get<int>("--mesh-len");

            double dt = 2.0 * M_PI / v_count;

            for(int i(0); i<v_count; i++){
                //std::cout<<" - t = "<<(i * dt)<<std::endl;
                auto pos = curve(parser, i * dt);
                auto v = mesh.add_vertex({pos[0], pos[1]});
                //std::cout<<" - added vertex "<<v<<" at "<<pos.transpose()<<std::endl;
            }

            auto center_v = mesh.add_vertex({0, 0});

            for(int i(0); i < v_count; i++){
                int a = i;
                int b = (i+1) % v_count;
                //std::cout<<" - adding face "<<a<<", "<<b<<", "<<center_v<<std::endl;
                mesh.add_face({VertexHandle(a), VertexHandle(b), center_v});
            }

            return; //below is to debug shape
            mesh.delete_vertex(center_v);
            mesh.collect_garbage();
        }


        void setup_parametric_mesh_with_ref(TetMesh& ref_mesh,
                                            TetMesh& mesh,
                                            ParametricCurve3D curve,
                                            argparse::ArgumentParser& parser){

            generate_rod_mesh(ref_mesh, mesh, parser.get<int>("--mesh-len"), 1.0, parser.get<double>("--torsion"));

            setup_parametric_mesh(mesh, curve, parser);

        }


        void setup_parametric_mesh(TetMesh& mesh,
                                   ParametricCurve3D curve,
                                   argparse::ArgumentParser& parser){


            int length = parser.get<int>("--mesh-len");

            double range = parser.get<double>("--parametric-range");
            //std::cout<<" - curve points: "<<std::endl;
            std::vector<Eigen::Vector3d> curve_points;
            for(int i(0); i <= length; i++){

                double t = range * (2.0 * M_PI * i) / (length);

                curve_points.push_back(curve(parser, t));
                //std::cout<<" - point "<<i<<" for t = "<<t<<" : "<<curve_pos<<std::endl;
            }

            double orthogonal_scaling = parser.get<double>("--parametric-width");
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

                Eigen::Vector3d curve_pos = curve_points[point_idx];
                Eigen::Vector3d next_curve_pos = curve_points[next_point_idx];
                //std::cout<<" - next curve point: "<<next_curve_pos<<std::endl;

                //tangent
                Eigen::Vector3d t = -(next_curve_pos - curve_pos).normalized();
                if(last_point){
                    curve_pos = next_curve_pos;
                }
                //std::cout<<" -      curve point: "<<curve_pos<<std::endl;
                //std::cout<<" - tangent: "<<t<<std::endl;

                // first perpendicular vector (with projection)
                Eigen::Vector3d t_proj = t;
                double sign = t_proj[2] == 0 ? 0 :(t_proj[2] > 0 ? 1 : -1);
                if(sign){
                    t_proj[2] = 0;
                }else{
                    t_proj = {0,0,1};
                    sign = 1;
                }
                Eigen::Vector3d tp1 = sign * t_proj.cross(t);
                tp1.normalize();
                //std::cout<<" - orthogonal axis 1: "<<tp1.transpose()<<std::endl;

                Eigen::Vector3d tp2 = tp1.cross(t);
                tp2.normalize();

                //std::cout<<" - orthogonal axis 2: "<<tp2.transpose()<<std::endl;
                //std::cout<<" - dot prod check 1: "<<tp1.dot(t)<<std::endl;
                //std::cout<<" - dot prod check 2: "<<tp2.dot(t)<<std::endl;

                Eigen::Vector3d new_pos = curve_pos + orthogonal_scaling * x * tp1 + orthogonal_scaling * y * tp2;
                //std::cout<<" - new pos: "<<new_pos<<std::endl;

                //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<new_pos<<std::endl;

                mesh.set_vertex(v, vec2vec(new_pos));
                i++;
            }
            return; //below is to debug shape
            mesh.delete_vertex(VertexHandle(0));
            mesh.collect_garbage();
        }
    #endif

    }
}



