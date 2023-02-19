
add_executable(FunZone main.cpp)
target_include_directories(FunZone
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )
target_link_libraries(FunZone
        PUBLIC
        UncannyGraphicsEngine
        UncannyTools
        )
target_compile_features(FunZone
        PUBLIC
        cxx_std_20
        )

function(CopyDirToBuildDirectory AbsoluteDirPath DirName)
    add_custom_command(
            TARGET FunZone POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${AbsoluteDirPath} $<TARGET_FILE_DIR:FunZone>/${DirName}
            COMMENT "Copying ${AbsoluteDirPath} => $<TARGET_FILE_DIR:FunZone>/${DirName}"
    )
endfunction()

#set(SHADERS_DIR ${PROJECT_SOURCE_DIR}/Engine/renderer/vulkan/vulkan_renderer/graphics_pipelines/shaders)
#set(SHADERS_DIR_NAME shaders)

#CopyDirToBuildDirectory(${SHADERS_DIR} ${SHADERS_DIR_NAME})
