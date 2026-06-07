
#include <argparse/argparse.hpp>

#include "VoxelGridMeshGen.hh"
#include "ParametricMeshGen.hh"
#include "SimpleMeshGen.hh"

using namespace tet_weave;

int parse_args(int argc, char** argv, argparse::ArgumentParser& parser);


int main(int argc, char** argv) {

    argparse::ArgumentParser parser;
    if (parse_args(argc, argv, parser))
        return 1;

    const std::string mesh_type     = parser.get<std::string>("--mesh-type");
    const std::string output        = parser.get<std::string>("--output");
    const bool        boundary_only = parser.get<bool>("--boundary-only");
    const int         length        = parser.get<int>("--length");
    const int         width         = parser.get<int>("--width");
    const int         height        = parser.get<int>("--height");
    const double      axial_scaling = parser.get<double>("--axial-scaling");
    const double      torsion_rad   = parser.get<double>("--torsion-rad");

    TetMesh mesh;

    // -------------------------------------------------------------------------
    // Voxel grid meshes
    // -------------------------------------------------------------------------

    if (mesh_type == "voxel-grid") {
        const int vg_width  = parser.get<int>("--voxel-grid-width");
        const int vg_height = parser.get<int>("--voxel-grid-height");
        const int vg_depth  = parser.get<int>("--voxel-grid-depth");
        mesh = VoxelGridMeshGen::generate_voxel_grid_mesh(vg_width, vg_height, vg_depth);

    } else if (mesh_type == "knotted-hole") {
        const int segments = parser.get<int>("--knotted-hole-segments");
        mesh = VoxelGridMeshGen::generate_Furchs_knotted_hole(segments);

        // -------------------------------------------------------------------------
        // Simple meshes
        // -------------------------------------------------------------------------

    } else if (mesh_type == "minimal-non-star-shaped-domain") {
        mesh = SimpleMeshGen::generate_minimal_non_star_shaped_domain_mesh();

    } else if (mesh_type == "minimal-star-shaped-domain") {
        mesh = SimpleMeshGen::generate_minimal_non_star_shaped_mesh();

    } else if (mesh_type == "rod") {
        mesh = SimpleMeshGen::generate_rod_mesh(length, axial_scaling, torsion_rad);

    } else if (mesh_type == "sine") {
        const double period  = parser.get<double>("--sine-period");
        const double amplitude = parser.get<double>("--sine-amplitude");
        mesh = SimpleMeshGen::generate_sine_mesh(length, axial_scaling, torsion_rad, period, amplitude);

    } else if (mesh_type == "spiral") {
        const double x_scale    = parser.get<double>("--spiral-x-scale");
        const double turn_count = parser.get<double>("--spiral-turn-count");
        mesh = SimpleMeshGen::generate_spiral_mesh(length, torsion_rad,
                                                   x_scale, turn_count);

    } else if (mesh_type == "trefoil-knot") {
        const double range = parser.get<double>("--trefoil-range");
        mesh = SimpleMeshGen::generate_trefoil_knot_mesh(length, torsion_rad, range);

    } else if (mesh_type == "layer") {
        mesh = SimpleMeshGen::generate_layer_mesh(width, height);

        // -------------------------------------------------------------------------
        // Parametric meshes
        // -------------------------------------------------------------------------

    } else if (mesh_type.rfind("para-", 0) == 0) {
        const auto   para_args = parser.get<std::vector<double>>("--para-args");
        const double thickness = parser.get<std::vector<double>>("--para-thickness")[0];
        const auto   t_range   = parser.get<std::vector<double>>("--para-t");
        const auto   u_range   = parser.get<std::vector<double>>("--para-u");
        const auto   v_range   = parser.get<std::vector<double>>("--para-v");

        const std::string para_type = mesh_type.substr(5); // strip "para-"

        ParametricCurve curve = nullptr;

        if      (para_type == "circle")               curve = circle;
        else if (para_type == "trefoil-knot")         curve = trefoil_knot;
        else if (para_type == "multi-branch-spiral")  curve = multi_branch_spiral;
        else if (para_type == "funny-spiral")         curve = funny_spiral;

        if (curve) {
            mesh = ParametricMeshGen::generate_parametric_mesh(
                    curve, para_args,
                    t_range[0], t_range[1], length,
                    thickness);

        } else {
            ParametricSurface surface = nullptr;

            if      (para_type == "sphere")           surface = sphere;
            else if (para_type == "torus")            surface = torus;
            else if (para_type == "sine3d")           surface = sine3d;
            else if (para_type == "helicoidal-ring")  surface = helicoidal_ring;

            if (!surface) {
                std::cerr << " - error: unknown parametric mesh type '" << mesh_type << "'\n";
                return 1;
            }

            mesh = ParametricMeshGen::generate_parametric_mesh(
                    surface, para_args,
                    u_range[0], u_range[1], width,
                    v_range[0], v_range[1], height,
                    thickness);
        }

    } else {
        std::cerr << " - error: unknown mesh type '" << mesh_type << "'\n";
        return 1;
    }

    // -------------------------------------------------------------------------
    // Export
    // -------------------------------------------------------------------------

    std::cout << " - generated mesh: "
              << mesh.n_vertices() << " vertices, "
              << mesh.n_cells()    << " cells\n";

    write_to_file(mesh, output, boundary_only);
    std::cout << " - written to '" << output << "'\n";

    return 0;
}



int parse_args(int argc, char** argv, argparse::ArgumentParser& parser) {

    parser.add_argument("--mesh-type")
            .help("-------------------------------------------------------------------------------------------------- \n"
                  " type of mesh to generate. \n"
                  "-------------------------------------------------------------------------------------------------- \n"
                  "Voxel Grid meshes: \n"
                  "  voxel-grid:              a width x height x depth tetrahedralised voxel grid\n"
                  "  knotted-hole:            a voxel grid with a hole dug into it that forms a knot (following Furch's work from 1924)\n"
                  "-------------------------------------------------------------------------------------------------- \n"
                  "Simple meshes:\n"
                  "  minimal-non-star-shaped-domain:  a minimal non-star-shaped domain mesh\n"
                  "  minimal-star-shaped-domain:      a minimal star-shaped domain mesh\n"
                  "  rod:                             a straight rod mesh\n"
                  "  sine:                            a rod mesh deformed following a sine wave\n"
                  "  spiral:                          a rod mesh deformed into a spiral\n"
                  "  trefoil-knot:                    a rod mesh deformed into a trefoil knot\n"
                  "  layer:                           a flat width x height layer mesh\n"
                  "-------------------------------------------------------------------------------------------------- \n"
                  "Parametric meshes (following curves or surfaces), which use the 'para-' prefix, e.g.:\n"
                  "  para-circle, para-trefoil-knot, para-multi-branch-spiral.\n"
                  " Look at the ParametricFunctions.hh file for a full list of what's available (or to add your own!)\n"
                  "-------------------------------------------------------------------------------------------------- \n")
            .required();

    parser.add_argument("--para-args")
            .help("parameters for the parametric function, as a space-separated list of doubles.\n"
                  "Required for all para- mesh types. Number of arguments depends on the function:\n"
                  "  para-circle:              radius z_scale\n"
                  "  para-trefoil-knot:        (none)\n"
                  "  para-multi-branch-spiral: center_radius branches branch_length z_scale\n"
                  "  para-funny-spiral:        a b c z_scale\n"
                  "  para-sphere:              radius\n"
                  "  para-torus:               major_radius minor_radius\n"
                  "  para-sine3d:              amplitude period\n"
                  "  para-helicoidal-ring:     loops\n"
                  " -> have a look at the ParametricFunction.hh file for more details on what these do.")
            .nargs(argparse::nargs_pattern::at_least_one)
            .scan<'g', double>();

    parser.add_argument("--para-thickness")
            .help("thickness of the rod around the parametric curve or thickness of the layer following the parametric surface.\n"
                  "Required for all para- mesh types.")
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--para-t")
            .help("parameter range [t_min t_max] for parametric curve meshes.")
            .nargs(2)
            .default_value(std::vector<double>{0.0, 1.0})
            .scan<'g', double>();

    parser.add_argument("--para-u")
            .help("parameter range [u_min u_max] for parametric surface meshes.")
            .nargs(2)
            .default_value(std::vector<double>{0.0, 1.0})
            .scan<'g', double>();

    parser.add_argument("--para-v")
            .help("parameter range [v_min v_max] for parametric surface meshes.")
            .nargs(2)
            .default_value(std::vector<double>{0.0, 1.0})
            .scan<'g', double>();

    parser.add_argument("--length")
            .help("integer length of the mesh (number of segments along the main axis).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--width")
            .help("integer width of the mesh (number of segments along the u axis).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--height")
            .help("integer height of the mesh (number of segments along the v axis).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--output")
            .help("output file path. Format is inferred from the extension (.ovm or .obj).")
            .default_value(std::string("output.ovm"))
            .nargs(1);

    parser.add_argument("--boundary-only")
            .help("if set, only export the boundary surface of the mesh.")
            .default_value(false)
            .implicit_value(true);

    // -------------------------------------------------------------------------
    // Simple mesh arguments
    // -------------------------------------------------------------------------

    parser.add_argument("--axial-scaling")
            .help("axial scaling factor for rod-based meshes (rod, sine, spiral, etc.).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--torsion-rad")
            .help("torsion in radians for rod-based meshes (rod, sine, spiral, etc.).")
            .default_value(0.0)
            .nargs(1)
            .scan<'g', double>();

    // sine mesh
    parser.add_argument("--sine-period")
            .help("period of the sine deformation (mesh-type: sine).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--sine-amplitude")
            .help("amplitude of the sine deformation along the y axis (mesh-type: sine).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    // spiral mesh
    parser.add_argument("--spiral-x-scale")
            .help("scaling factor along the x axis (mesh-type: spiral).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--spiral-turn-count")
            .help("number of turns of the spiral (mesh-type: spiral).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    // trefoil knot mesh
    parser.add_argument("--trefoil-range")
            .help("range of the trefoil knot curve, as a fraction of the full knot [0..1].\n"
                  "1.0 gives the full knot (mesh-type: trefoil-knot).")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    // knotted hole
    parser.add_argument("--knotted-hole-segments")
            .help("number of segments for each hole in the knotted hole mesh (mesh-type: knotted-hole).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    // voxel grid
    parser.add_argument("--voxel-grid-width")
            .help("width of the voxel grid (mesh-type: voxel-grid).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--voxel-grid-height")
            .help("height of the voxel grid (mesh-type: voxel-grid).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--voxel-grid-depth")
            .help("depth of the voxel grid (mesh-type: voxel-grid).")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << " - argument parsing error: " << e.what() << "\n"
                  << parser << "\n";
        return 1;
    }

    // Validate: para- mesh types require --para-args and --para-thickness
    const std::string mesh_type = parser.get<std::string>("--mesh-type");
    if (mesh_type.rfind("para-", 0) == 0) {
        if (!parser.is_used("--para-args")) {
            std::cerr << " - error: --para-args is required for parametric mesh type '"
                      << mesh_type << "'\n";
            return 1;
        }
        if (!parser.is_used("--para-thickness")) {
            std::cerr << " - error: --para-thickness is required for parametric mesh type '"
                      << mesh_type << "'\n";
            return 1;
        }
    }

    return 0;
}


int parse_args_old(int argc, char** argv, argparse::ArgumentParser& parser){


    parser.add_argument("-h", "--help")
            .help("prints this help message")
            .flag();

    parser.add_argument("--short-tests")
            .help("runs the current short tests (see remeshing_maps.cc)")
            .default_value(false)
            .flag();

    parser.add_argument("--domain-mesh")
            .help("specify the mesh to use as domain. If this is set, it will superseed the `--test-mesh` mode (see below).")
            .default_value("");


    parser.add_argument("--run-positional-opti")
            .help("instead of the virtual-elements based deformation, this runs the boundary positions, equality-constrained version")
            .default_value(false)
            .flag();


    parser.add_argument("--test-mesh")
            .help("specify the test mesh boundary conditions. All options have a single interior vertex. Options are:\n"
                  " -------------------------------------------------------------- \n"
                  " ------------------------------- 2D only options: \n"
                  " - 'minimal': three triangles \n"
                  " - 'clover' something that looks more or less like a clover, with a parametrizable horizontal shift (to make it non-star-shaped) \n"
                  " - 'mouth' something that looks more or less like a mouth with spiky teeth \n"
                  "    As a rule of thumb, the number of vertices should be greater than #branches * branch length * 8 \n"
                  " - 'compression-disc' a disc filled with a CDT, then compressed. (should be compressed enough that there's no trivial map with the given energy barrier \n"
                  " ------------------------------------------------------------------------------------------- \n"
                  " ------------------------------- 3D only options: \n"
                  " - 'trefoil-knot' a trefoil knot (shocking I know). Only available for tetrahedral meshes. \n"
                  " - 'complex-spiral' a crazy spiral taken from a youtube video (see implementation for more details) \n"
                  " - 'torsion-cube' a simple voxel cube, with a torsion given as parameter \n"
                  " ------------------------------------------------------------------------------------------- \n"
                  " ------------------------------- 2D and 3D options: \n"
                  " - 'straight' a rectangular mesh of variable length \n"
                  " - 'minimal-non-star-shaped' five triangles forming a non-star-shaped space \n"
                  " - 'sine' the straight mesh shaped into a sine wave \n"
                  " - 'spiral' the straight mesh shaped into a spiral \n"
                  " - 'paraspiral' a parametric spiral with arbitrarily many branches.\n"
                  " ------------------------------------------------------------------------------------------- \n"
            )
            .default_value("");


    parser.add_argument("--mesh-len")
            .help("specify the test mesh integer length")
            .default_value(10)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--torsion")
            .help("specify the test mesh z-axis torsion (in degrees)")
            .default_value(0.0)
            .nargs(1)
            .scan<'g', double>();

    //------------------------------------------------------------------------------------ test mesh-specific parameters

    parser.add_argument("--sine-x-scale")
            .help("specify the sine test mesh horizontal scaling")
            .default_value(10.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--sine-y-scale")
            .help("specify the sine test mesh vertical scaling")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--sine-z-scale")
            .help("specify the sine test mesh depth scaling")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--sine-period")
            .help("specify the sine test mesh period")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();


    parser.add_argument("--spiral-turns")
            .help("specify the spiral test mesh number of turns")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--spiral-x-scale")
            .help("specify the spiral test mesh x scaling (only for tet meshes)")
            .default_value(0.0)
            .nargs(1)
            .scan<'g', double>();


    parser.add_argument("--clover-center-radius")
            .help("specify the clover test mesh center radius")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--clover-shift")
            .help("specify the clover test mesh shift between the bottom and top (mesh will be star-shaped if set to 0)")
            .default_value(0.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--clover-leaves")
            .help("specify the clover test mesh number of 'leaves'")
            .default_value(2)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--mouth-teeth-count")
            .help("specify the (rough) number of 'teeth' in the mouth test mesh")
            .default_value(5.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--mouth-opening")
            .help("specify how much the mouth test mesh should be 'open' (capped at 1)")
            .default_value(0.1)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--paraspiral-center-radius")
            .help("specify the radius of the center of the parametric spiral test mesh")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--paraspiral-branches")
            .help("specify the number of branches of the parametric spiral test mesh")
            .default_value(3)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--paraspiral-branch-len")
            .help("specify the (conceptual) length of the parametric spiral test mesh")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--compression-mode")
            .help("specify the number of branches of the parametric spiral test mesh")
            .default_value(0)
            .nargs(1)
            .scan<'i', int>();

    parser.add_argument("--i-to-b-v-count-ratio")
            .help("specify the ratio between interior to boundary vertices (for all compression tests)")
            .default_value(2.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--compression-disc-ratio")
            .help("specify the horizontal scaling for the compression disc test mesh")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();


    parser.add_argument("--parametric-range")
            .help("specify the the range of the parametric curve mesh should be generated, in [0, 1]")
            .default_value(0.9)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--parametric-width")
            .help("specify the the width of the parametric curve mesh. 0 will give a fully degenerate mesh, while values around 1 will probably generate some self-intersections. The best value here probably depends on the curve.")
            .default_value(0.5)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--parametric-z-scale")
            .help("specify the depth scaling for the parametric tet meshes. Setting it to 0 will leave it in the XY plane.")
            .default_value(1.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--complex-spiral-a")
            .help("specify the a parameter for the complex spiral (see curve function for details).")
            .default_value(5.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--complex-spiral-b")
            .help("specify the b parameter for the complex spiral (see curve function for details).")
            .default_value(60.0)
            .nargs(1)
            .scan<'g', double>();

    parser.add_argument("--complex-spiral-c")
            .help("specify the c parameter for the complex spiral (see curve function for details).")
            .default_value(20.0)
            .nargs(1)
            .scan<'g', double>();


    //------------------------------------------------------------------------------------------------- other parameters

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    if (parser["--help"] == true) {
        std::cerr<<parser;
        exit(1);
    }

    return 0;
}

#if 0

void generate_knotted_hole_meshes() {

    for (int i(1); i<=10; i++) {
        auto domain_mesh = FurchsKnotGenerator::generate_mesh(i);
        split_cord_edges_and_faces(domain_mesh);

        TetrahedralMesh codomain_mesh = domain_mesh;

        TutteCubeMapping::TutteCubeMapper cube_mapper(codomain_mesh);

        //home-made one
        cube_mapper.setAsCorner(VertexHandle(0));
        cube_mapper.setAsCorner(VertexHandle(9));
        cube_mapper.setAsCorner(VertexHandle(79));
        cube_mapper.setAsCorner(VertexHandle(70));

        cube_mapper.setAsCorner(VertexHandle(400));
        cube_mapper.setAsCorner(VertexHandle(409));
        cube_mapper.setAsCorner(VertexHandle(479));
        cube_mapper.setAsCorner(VertexHandle(470));

        std::cout<<" set corners"<<std::endl;

        cube_mapper.mapBoundaryToCube();

        for (auto vh: codomain_mesh.vertices()) {
            if (!codomain_mesh.is_boundary(vh)) {
                codomain_mesh.set_vertex(vh, {0,0,0});
            }
        }
        std::cout<<" mapped interior to origin"<<std::endl;

        auto bad_tets = CGAL_wrapper::find_degenerate_or_flipped_tets(codomain_mesh);
        std::cout<<" #degenerate tets: "<<bad_tets.degenerate.size()<<std::endl;
        std::cout<<"    #flipped tets: "<<bad_tets.flipped.size()<<std::endl;

        if (!bad_tets.flipped.empty()) {
            std::cout<<" ERROR - codomain mesh contains "<<bad_tets.flipped.size()<<" flipped elements"<<std::endl;
            continue;
        }

        domain_mesh.collect_garbage();
        codomain_mesh.collect_garbage();

        IO::FileManager fm;
        fm.writeFile("knotted_hole_" + std::to_string(i) + "_domain_mesh.ovm",     domain_mesh);
        fm.writeFile("knotted_hole_" + std::to_string(i) + "_codomain_mesh.ovm", codomain_mesh);

        export_boundary_conditions_from_mesh(codomain_mesh, "knotted_hole_" + std::to_string(i) + "_boundary_conditions.txt");
    }
}

#endif