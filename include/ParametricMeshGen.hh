
#pragma once

#include "TetMesh.hh"
#include "ParametricFunctions.hh"

namespace tet_weave {


    namespace ParametricMeshGen {






        TetMesh generate_parametric_mesh(ParametricCurve curve,
                                         std::vector<double>& curve_params,
                                         double t_min,
                                         double t_max,
                                         int length,
                                         double thickness){


            //std::cout<<" - curve points: "<<std::endl;
            std::vector<Vec3d> curve_points;
            for(int i(0); i <= length; i++){

                double t = t_min + (t_max - t_min) * (2.0 * M_PI * i) / (length);

                curve_points.push_back(curve(curve_params, t));
                //std::cout<<" - point "<<i<<" for t = "<<t<<" : "<<curve_pos<<std::endl;
            }

            
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
    }
}



