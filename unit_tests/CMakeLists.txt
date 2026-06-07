

message("external dir: "${EXTERNAL_DIR})

include(FetchContent)

if(NOT TARGET googletest)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include_directories(../include)
add_executable(unit_tests unit_tests.cc)
target_link_libraries(unit_tests
        GTest::gtest_main
)

add_test(NAME unit_tests COMMAND $<TARGET_FILE:unit_tests>)

set(REF_MESH_DIR "${CMAKE_SOURCE_DIR}/unit_tests/ref_meshes")
target_compile_definitions(unit_tests PRIVATE
        REF_MESH_DIR="${REF_MESH_DIR}"
        CLI_BINARY_PATH="${CLI_BINARY_PATH}"
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

