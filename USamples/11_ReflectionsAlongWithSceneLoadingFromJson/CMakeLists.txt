
add_executable(11_ReflectionsAlongWithSceneLoadingFromJson main.cpp)
target_include_directories(11_ReflectionsAlongWithSceneLoadingFromJson
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(11_ReflectionsAlongWithSceneLoadingFromJson
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(11_ReflectionsAlongWithSceneLoadingFromJson
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(11_ReflectionsAlongWithSceneLoadingFromJson
        PUBLIC
        cxx_std_20
        )
