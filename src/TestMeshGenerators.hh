
#pragma once

#include "includes.hh"

#if 0

#include "Helpers.hh"

void setup_minimal_non_star_shaped_mesh(TetrahedralMesh& ref_mesh,
                                        TetrahedralMesh& mesh);



/* ref mesh and mesh will look like this:
 * x--x-...-x-...-x
 * |        x     |
 * x--x-...-x-...-x
 *
 */
void setup_straight_mesh(TriangleMesh& mesh,
                         int length,
                         double axial_scaling);

void setup_straight_mesh(TetrahedralMesh& mesh,
                         int length,
                         double axial_scaling,
                         double torsion);


/** using the straight mesh as reference */
void setup_sine_mesh(TriangleMesh& mesh,
                     int length,
                     double period = 1.0,
                     double x_scale = 1.0,
                     double y_scale = 1.0);

void setup_sine_mesh(TetrahedralMesh& mesh,
                     int length,
                     double torsion,
                     double period = 1.0,
                     double x_scale = 1.0,
                     double y_scale = 1.0,
                     double z_scale = 1.0);


void setup_spiral_mesh(TetrahedralMesh& ref_mesh,
                       TetrahedralMesh& mesh,
                       int length,
                       bool compress_ref_mesh_to_cube,
                       double torsion,
                       double x_scale,
                       double turn_count);


void setup_trefoil_knot_mesh(TetrahedralMesh& mesh,
                             int length,
                             double torsion,
                             double range);


// ----------------------------------------------------------------------------- parametric-curve-based meshes

//NOTE: for 2d meshes, the parametric curve defines the boundary of the mesh, with a single interior vertex.
//For 3d meshes, this generates "straight" meshes following the given parametric curve

typedef Eigen::Vector3d (*ParametricCurve3D)(argparse::ArgumentParser& parser, double t);

Eigen::Vector3d parametric_spiral_3D(argparse::ArgumentParser& parser, double t);

Eigen::Vector3d parametric_trefoil_knot(argparse::ArgumentParser& parser, double t);

// taken from https://www.youtube.com/watch?v=Y2T31OQ-cWA
Eigen::Vector3d parametric_complex_spiral(argparse::ArgumentParser& parser, double t);

#endif



