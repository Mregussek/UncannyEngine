
add_executable(13_KajiyaStylePathTracing main.cpp App.cpp App.h)
set_target_properties(13_KajiyaStylePathTracing PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(13_KajiyaStylePathTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(13_KajiyaStylePathTracing
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(13_KajiyaStylePathTracing
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(13_KajiyaStylePathTracing
        PUBLIC
        cxx_std_20
        )
