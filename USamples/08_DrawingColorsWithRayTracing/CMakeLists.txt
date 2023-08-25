
add_executable(08_DrawingColorsWithRayTracing main.cpp)
set_target_properties(08_DrawingColorsWithRayTracing PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(08_DrawingColorsWithRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(08_DrawingColorsWithRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(08_DrawingColorsWithRayTracing
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(08_DrawingColorsWithRayTracing
        PUBLIC
        cxx_std_20
        )
