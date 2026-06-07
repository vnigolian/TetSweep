#include <emscripten/emscripten.h>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>

#include "TetMesh.hh"
#include "ArgsDispatch.hh"

using namespace tet_weave;

extern "C" {


/// Generate a mesh from a CLI-style argument string and return it as an OBJ string.
/// e.g. "--mesh-type rod --length 10"
/// The caller must free the returned string with free_string().
EMSCRIPTEN_KEEPALIVE
char* generate_mesh_str(const char* args_c) {

    // Split the args string into tokens
    std::vector<std::string> tokens;
    std::istringstream ss(args_c);
    std::string token;
    while (ss >> token)
        tokens.push_back(token);

    // Build argc/argv
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>("tet_weave")); // argv[0]
    for (auto& t : tokens)
        argv.push_back(const_cast<char*>(t.c_str()));
    int argc = static_cast<int>(argv.size());

    // Parse arguments
    argparse::ArgumentParser parser;
    if (parse_args(argc, argv.data(), parser)) {
        char* buf = static_cast<char*>(std::malloc(1));
        buf[0] = '\0';
        return buf;
    }

    tet_weave::TetMesh mesh = generate_mesh_from_args(parser);
    const bool boundary_only = parser.get<bool>("--boundary-only");

    std::ostringstream out;
    mesh.write_obj_to_stream(out, boundary_only);

    const std::string result = out.str();
    char* buf = static_cast<char*>(std::malloc(result.size() + 1));
    std::memcpy(buf, result.c_str(), result.size() + 1);
    return buf;
}

/// Free a string returned by generate_mesh_str().
EMSCRIPTEN_KEEPALIVE
void free_string(char* ptr) {
    std::free(ptr);
}

} // extern "C"