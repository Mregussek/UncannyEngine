
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

function(add_test_file TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_include_directories(${TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
    target_link_libraries(${TEST_NAME} PRIVATE GTest::GTest UncannyLibrary)
    add_test(${TEST_NAME}_GTest FirstTest)
endfunction()

add_test_file(Vec3Test math/vec3_tests.cpp)
add_test_file(Vec4Test math/vec4_tests.cpp)
add_test_file(Mat4Test math/mat4_tests.cpp)
