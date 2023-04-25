
add_executable(09_RayTracedShadow main.cpp)
target_include_directories(09_RayTracedShadow
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(09_RayTracedShadow
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(09_RayTracedShadow
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(09_RayTracedShadow
        PUBLIC
        cxx_std_20
        )
