#pragma once

#include "Vec3d.hh"

namespace tet_weave{


    using ParametricCurve = Vec3d(*)(const std::vector<double>&, double);


    Vec3d circle(const std::vector<double>& params, double t){
        assert(params.size() == 2 && "parametric circle takes exactly 2 argument");

        double radius = params[0];
        double z_scale = params[1];

        return params[0] * Vec3d(std::cos(t), std::sin(t), z_scale * t);
    }



    Vec3d multi_branch_spiral(const std::vector<double>& params, double t){
        assert(params.size() == 4 && "parametric spiral takes exactly 4 arguments");

        double center_radius = params[0];
        int branches         = params[1];
        double branch_length = params[2];
        double z_scale       = params[3];

        double radius = (std::cos(branches * t) + 1.0 + center_radius);
        double angle = radius * branch_length;

        /*Eigen::Matrix2d rotation;
        rotation << std::cos(angle), -std::sin(angle),
                std::sin(angle),  std::cos(angle);
        Eigen::Vector2d v({std::cos(t), std::sin(t)});

        Eigen::Vector2d point = radius * rotation * v;
        Eigen::Vector3d point_3d;
        point_3d << point[0], point[1], z_scale * t;*/

        Vec3d p = {std::cos(t), std::sin(t), z_scale * t};
        return p.rotate({0,0,1}, angle);
    }


    Vec3d trefoil_knot(const std::vector<double>& params, double t){
        return { std::sin(t) + 2 * std::sin(2 * t),
                    std::cos(t) - 2 * std::cos(2 * t),
                    -std::sin(3 * t)};
    }


    Vec3d funny_spiral(const std::vector<double>& params, double t){
        assert(params.size() == 4 && "parametric complex spiral takes exactly 4 arguments");

        double a       = params[0];
        double b       = params[1];
        double c       = params[2];
        double z_scale = params[3];

        return {std::cos(a * t) + std::cos(b * t) / 2.0 + std::sin(c * t) / 3.0,
                std::sin(a * t) + std::sin(b * t) / 2.0 + std::cos(c * t) / 3.0,
                z_scale * t};
    }

}