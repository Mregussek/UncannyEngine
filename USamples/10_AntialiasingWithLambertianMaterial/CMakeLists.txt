
add_executable(10_AntialiasingWithLambertianMaterial main.cpp)
target_include_directories(10_AntialiasingWithLambertianMaterial
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(10_AntialiasingWithLambertianMaterial
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(10_AntialiasingWithLambertianMaterial
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(10_AntialiasingWithLambertianMaterial
        PUBLIC
        cxx_std_20
        )
