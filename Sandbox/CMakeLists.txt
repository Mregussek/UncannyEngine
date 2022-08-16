
add_executable(UncannySandbox main.cpp)
target_include_directories(UncannySandbox PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
target_link_libraries(UncannySandbox PUBLIC UncannyLibrary)
target_compile_features(UncannySandbox PUBLIC cxx_std_20)

file(GLOB_RECURSE SPDLOG_LIBRARY_SOURCES
        $ENV{SPDLOG_3RDPARTY_DIR}/include/spdlog/*.h)

file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        ${PROJECT_SOURCE_DIR}/Engine/*.cpp
        ${PROJECT_SOURCE_DIR}/Engine/*.inl
        ${PROJECT_SOURCE_DIR}/Engine/*.h)

add_executable(UncannySandboxOnlySources
        main.cpp
        ${ENGINE_LIBRARY_SOURCES}
        ${SPDLOG_LIBRARY_SOURCES})
target_include_directories(UncannySandboxOnlySources
        PUBLIC
        ${PROJECT_SOURCE_DIR}/Engine
        ${OPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS}
        $ENV{SPDLOG_3RDPARTY_DIR}/include
        $ENV{VOLK_3RDPARTY_DIR}
        $ENV{VULKAN_SDK}/include
        $ENV{GLFW_3RDPARTY_DIR}/include)
target_link_libraries(UncannySandboxOnlySources PRIVATE ${OPENGL_LIBRARIES} glfw volk_headers)
target_compile_features(UncannySandboxOnlySources PUBLIC cxx_std_20)
