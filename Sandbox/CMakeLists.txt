
add_executable(UncannySandbox main.cpp)
target_include_directories(UncannySandbox PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
target_link_libraries(UncannySandbox PUBLIC UncannyLibrary)
target_compile_features(UncannySandbox PUBLIC cxx_std_20)

function(CopyDirToBuildDirectory AbsoluteDirPath DirName)
    add_custom_command(
            TARGET UncannySandbox POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${AbsoluteDirPath} $<TARGET_FILE_DIR:UncannySandbox>/${DirName}
            COMMENT "Copying ${AbsoluteDirPath} => $<TARGET_FILE_DIR:UncannySandbox>/${DirName}"
    )
endfunction()

set(SHADERS_DIR ${PROJECT_SOURCE_DIR}/Engine/renderer/vulkan/shaders)
set(SHADERS_DIR_NAME shaders)

CopyDirToBuildDirectory(${SHADERS_DIR} ${SHADERS_DIR_NAME})
