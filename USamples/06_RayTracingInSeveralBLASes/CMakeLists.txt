
add_executable(06_RayTracingInSeveralBLASes main.cpp)
set_target_properties(06_RayTracingInSeveralBLASes PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(06_RayTracingInSeveralBLASes
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(06_RayTracingInSeveralBLASes
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(06_RayTracingInSeveralBLASes
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(06_RayTracingInSeveralBLASes
        PUBLIC
        cxx_std_20
        )
