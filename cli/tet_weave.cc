
#include <argparse/argparse.hpp>

#include "VoxelGridMeshGen.hh"
#include "ParametricMeshGen.hh"

int parse_args(int argc, char** argv, argparse::ArgumentParser& parser);


int main(int argc, char** argv) {

    argparse::ArgumentParser parser;
    if(parse_args(argc, argv, parser)){
        std::cout<<" - arguments parsing failed"<<std::endl;
        return 1;
    }

    if(parser.get<bool>("--short-tests")) {

        exit(EXIT_FAILURE);
    }

std::cout<<" all good"<<std::endl;

    return 0;

}




int parse_args(int argc, char** argv, argparse::ArgumentParser& parser){


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
                  " -------------------------------------------------------------- \n"
                  " ------------------------------- 3D only options: \n"
                  " - 'trefoil-knot' a trefoil knot (shocking I know). Only available for tetrahedral meshes. \n"
                  " - 'complex-spiral' a crazy spiral taken from a youtube video (see implementation for more details) \n"
                  " - 'torsion-cube' a simple voxel cube, with a torsion given as parameter \n"
                  " -------------------------------------------------------------- \n"
                  " ------------------------------- 2D and 3D options: \n"
                  " - 'straight' a rectangular mesh of variable length \n"
                  " - 'minimal-non-star-shaped' five triangles forming a non-star-shaped space \n"
                  " - 'sine' the straight mesh shaped into a sine wave \n"
                  " - 'spiral' the straight mesh shaped into a spiral \n"
                  " - 'paraspiral' a parametric spiral with arbitrarily many branches.\n"
                  " -------------------------------------------------------------- \n"
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