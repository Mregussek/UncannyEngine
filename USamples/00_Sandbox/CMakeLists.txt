
add_executable(00_Sandbox main.cpp)
target_include_directories(00_Sandbox
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(00_Sandbox
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(00_Sandbox
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(00_Sandbox
        PUBLIC
        cxx_std_20
        )
