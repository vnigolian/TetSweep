

message("external dir: "${EXTERNAL_DIR})


#if(NOT TARGET acutest)
#    FetchContent_Declare(acutest
#            GIT_REPOSITORY  https://github.com/mity/acutest.git
#            SOURCE_DIR "${EXTERNAL_DIR}/acutest"
#    )
    #FetchContent_MakeAvailable(acutest)
#endif()


#include_directories("${EXTERNAL_DIR}/acutest/include")

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


enable_testing()

include_directories(../include)
add_executable(unit_tests unit_tests.cc)
target_link_libraries(unit_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

#include_directories("${PROJECT_SOURCE_DIR}/include")

#add_executable(c-example c-example.c ../include/acutest.h)
#add_executable(cpp-example cpp-example.cc ../include/acutest.h)
