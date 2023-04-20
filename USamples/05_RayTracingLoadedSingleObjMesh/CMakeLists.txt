
add_executable(05_RayTracingTriangleLoadedSingleObjMesh main.cpp)
target_include_directories(05_RayTracingTriangleLoadedSingleObjMesh
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(05_RayTracingTriangleLoadedSingleObjMesh
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(05_RayTracingTriangleLoadedSingleObjMesh
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(05_RayTracingTriangleLoadedSingleObjMesh
        PUBLIC
        cxx_std_20
        )
