#include "ArgsDispatch.hh"

using namespace tet_sweep;

int main(int argc, char** argv) {
    argparse::ArgumentParser parser;
    if (parse_args(argc, argv, parser))
        return 1;

    const std::string output       = parser.get<std::string>("--output");
    const bool        boundary_only = parser.get<bool>("--boundary-only");

    TetMesh mesh = generate_mesh_from_args(parser);

    write_to_file(mesh, output, boundary_only);
    return 0;
}