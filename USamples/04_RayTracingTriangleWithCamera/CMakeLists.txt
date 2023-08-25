
add_executable(04_RayTracingTriangleWithCamera main.cpp)
set_target_properties(04_RayTracingTriangleWithCamera PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(04_RayTracingTriangleWithCamera
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(04_RayTracingTriangleWithCamera
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(04_RayTracingTriangleWithCamera
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(04_RayTracingTriangleWithCamera
        PUBLIC
        cxx_std_20
        )
