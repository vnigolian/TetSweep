add_executable(tetweave_wasm
        main_wasm.cc
        ${CMAKE_SOURCE_DIR}/cli/ArgsDispatch.cc
)

target_include_directories(tetweave_wasm PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cli
)

target_link_libraries(tetweave_wasm PRIVATE argparse)

set_target_properties(tetweave_wasm PROPERTIES
        SUFFIX ".js"
        LINK_FLAGS "\
        -s EXPORTED_FUNCTIONS='[\"_generate_mesh_str\",\"_free_string\"]' \
        -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\",\"UTF8ToString\"]' \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s MODULARIZE=1 \
        -s EXPORT_NAME='TetWeaveModule' \
        -s ENVIRONMENT='web,node' \
        --bind \
    "
)

set_target_properties(tetweave_wasm PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
)