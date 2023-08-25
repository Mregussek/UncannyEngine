
add_executable(00_Sandbox main.cpp App.cpp App.h)
set_target_properties(00_Sandbox PROPERTIES LINKER_LANGUAGE CXX)
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
