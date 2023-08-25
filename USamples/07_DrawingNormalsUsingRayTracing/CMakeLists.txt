
add_executable(07_DrawingNormalsUsingRayTracing main.cpp)
set_target_properties(07_DrawingNormalsUsingRayTracing PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(07_DrawingNormalsUsingRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(07_DrawingNormalsUsingRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(07_DrawingNormalsUsingRayTracing
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(07_DrawingNormalsUsingRayTracing
        PUBLIC
        cxx_std_20
        )
