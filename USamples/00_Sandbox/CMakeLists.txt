
add_executable(Sandbox main.cpp)
target_include_directories(Sandbox
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_directories(Sandbox
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(Sandbox
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        )
target_compile_features(Sandbox
        PUBLIC
        cxx_std_20
        )

function(CopyDirToBuildDirectory TargetName AbsoluteDirPath DirName)
    add_custom_command(
            TARGET ${TargetName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${AbsoluteDirPath} $<TARGET_FILE_DIR:${TargetName}>/${DirName}
            COMMENT "Copying ${AbsoluteDirPath} => $<TARGET_FILE_DIR:${TargetName}>/${DirName}"
    )
endfunction()

#set(SHADERS_DIR ${PROJECT_SOURCE_DIR}/Engine/renderer/vulkan/vulkan_renderer/graphics_pipelines/shaders)
#set(SHADERS_DIR_NAME shaders)

#CopyDirToBuildDirectory(Sandbox ${SHADERS_DIR} ${SHADERS_DIR_NAME})
