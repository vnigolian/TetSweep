#pragma once

#include <argparse/argparse.hpp>
#include "TetMesh.hh"

namespace tet_sweep {

    int parse_args(int argc, char** argv, argparse::ArgumentParser& parser);
    TetMesh generate_mesh_from_args(argparse::ArgumentParser& parser);

} // namespace tet_sweep