
add_executable(01_ClearColorImage_via_vkCmdClearColorImage main.cpp)
set_target_properties(01_ClearColorImage_via_vkCmdClearColorImage PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(01_ClearColorImage_via_vkCmdClearColorImage
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(01_ClearColorImage_via_vkCmdClearColorImage
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(01_ClearColorImage_via_vkCmdClearColorImage
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(01_ClearColorImage_via_vkCmdClearColorImage
        PUBLIC
        cxx_std_20
        )
