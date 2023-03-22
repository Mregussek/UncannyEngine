
add_executable(02_CopyRenderTargetImageIntoSwapchainImage main.cpp)
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
        )
target_compile_features(02_CopyRenderTargetImageIntoSwapchainImage
        PUBLIC
        cxx_std_20
        )
target_compile_options(02_CopyRenderTargetImageIntoSwapchainImage
        PRIVATE
        /NODEFAULTLIB
        )
