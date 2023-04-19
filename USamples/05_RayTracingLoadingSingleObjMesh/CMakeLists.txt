
add_executable(05_RayTracingTriangleLoadingSingleObjMesh main.cpp)
target_include_directories(05_RayTracingTriangleLoadingSingleObjMesh
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(05_RayTracingTriangleLoadingSingleObjMesh
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(05_RayTracingTriangleLoadingSingleObjMesh
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(05_RayTracingTriangleLoadingSingleObjMesh
        PUBLIC
        cxx_std_20
        )
target_compile_options(05_RayTracingTriangleLoadingSingleObjMesh
        PRIVATE
        /NODEFAULTLIB
        )
