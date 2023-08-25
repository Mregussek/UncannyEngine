
add_executable(02_CopyRenderTargetImageIntoSwapchainImage main.cpp)
set_target_properties(02_CopyRenderTargetImageIntoSwapchainImage PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(02_CopyRenderTargetImageIntoSwapchainImage
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(02_CopyRenderTargetImageIntoSwapchainImage
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(02_CopyRenderTargetImageIntoSwapchainImage
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        UncannyMath
        )
target_compile_features(02_CopyRenderTargetImageIntoSwapchainImage
        PUBLIC
        cxx_std_20
        )
