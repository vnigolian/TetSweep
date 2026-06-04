#pragma once

#include "Vec3d.hh"

namespace tet_weave{


    using ParametricCurve = Vec3d(*)(const std::vector<double>&, double);

    using ParametricSurface = Vec3d(*)(const std::vector<double>&, double, double);


// -----------------------------------------------------------------------------
// curves
// -----------------------------------------------------------------------------


    Vec3d circle(const std::vector<double>& params, double t){
        assert(params.size() <= 2 && "parametric circle takes at most 2 arguments (radius and z-max)");

        double radius = 1.0;
        double z_max = 0.0;

        if(params.size() >= 1) {
            radius = params[0];
        }
        if(params.size() >= 2) {
            z_max = params[1];
        }

        return radius * Vec3d(std::cos(t), std::sin(t), z_max * t);
    }



    Vec3d multi_branch_spiral(const std::vector<double>& params, double t){
        assert(params.size() == 4 && "parametric spiral takes exactly 4 arguments");

        double center_radius = params[0];
        int branches         = params[1];
        double branch_length = params[2];
        double z_scale       = params[3];

        double radius = (std::cos(branches * t) + 1.0 + center_radius);
        double angle = radius * branch_length;


        Vec3d p = {std::cos(t), std::sin(t), z_scale * t};
        return radius * p.rotate({0,0,1}, angle);
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


// -----------------------------------------------------------------------------
// surfaces
// -----------------------------------------------------------------------------


    Vec3d sphere(const std::vector<double>& params, double u, double v){
        assert(params.size() == 1 && "parametric sphere takes exactly 1 argument");

        double r = params[0];
        return r * Vec3d(std::sin(u) * std::cos(v),
                                 std::sin(u) * std::sin(v), 
                                 std::cos(u));
    }


    Vec3d sine3d(const std::vector<double>& params, double u, double v){
                assert(params.size() <= 2 && "parametric sine 3d takes at most 2 argument");

                double amp = 1.0;
                double period = 1.0;

                if(params.size() >= 1){
                    amp = params[0];
                }

                if(params.size() >= 2){
                    period = params[1];
                }

                return amp * Vec3d(u,v, std::sin(period * u) * std::sin(period * v));

    }

        Vec3d helicoidal_ring(const std::vector<double>& params, double u, double v){

            assert(params.size() == 1 && "helicoidal rings surface takes exactly 1 argument");

            const double tau = 2 * M_PI;
            double loops = params[0];

            return Vec3d(
                std::cos(tau * v) * std::cos(tau * u) + (std::cos(loops * tau * v) * (std::sin(tau * u) +3) + 7) * (-sin(tau * v)),
                std::sin(tau * u) * std::cos(tau * u) + (std::cos(loops * tau * v) * (std::sin(tau * u) +3) + 7) * ( cos(tau * v)),
                std::sin(loops * tau * v) * (std::sin(tau * u) + 3)
            );
        }


    Vec3d torus(const std::vector<double>& params, double u, double v) {
        assert(params.size() == 2 && "torus surface takes exactly 2 arguments (major and minor radii)");

        double R = params[0];
        double r = params[1];

        return {(R + r * std::cos(v)) * std::cos(u),
                (R + r * std::cos(v)) * std::sin(u),
                r * std::sin(v)};
    }
}