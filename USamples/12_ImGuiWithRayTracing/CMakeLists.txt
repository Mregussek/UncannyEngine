
add_executable(12_ImGuiWithRayTracing main.cpp)
target_include_directories(12_ImGuiWithRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(12_ImGuiWithRayTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(12_ImGuiWithRayTracing
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(12_ImGuiWithRayTracing
        PUBLIC
        cxx_std_20
        )
