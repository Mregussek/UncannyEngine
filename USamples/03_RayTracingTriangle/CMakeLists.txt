
add_executable(03_RayTracingTriangle main.cpp)
set_target_properties(03_RayTracingTriangle PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(03_RayTracingTriangle
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(03_RayTracingTriangle
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(03_RayTracingTriangle
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(03_RayTracingTriangle
        PUBLIC
        cxx_std_20
        )
