
set(UGRAPHICSENGINE_PATH ${PROJECT_SOURCE_DIR}/UGraphicsEngine/)

#file(GLOB_RECURSE SPDLOG_LIBRARY_SOURCES
#        $ENV{SPDLOG_3RDPARTY_DIR}/include/spdlog/*.h
#        )
file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        ${UGRAPHICSENGINE_PATH}/*.cpp
        ${UGRAPHICSENGINE_PATH}/*.inl
        ${UGRAPHICSENGINE_PATH}/*.h
        )
add_library(UncannyGraphicsEngine
        ${ENGINE_LIBRARY_SOURCES}
        #${SPDLOG_LIBRARY_SOURCES}
        )
set_target_properties(UncannyGraphicsEngine
        PROPERTIES
        LINKER_LANGUAGE C++)
target_include_directories(UncannyGraphicsEngine
        PUBLIC
        ${UGRAPHICSENGINE_PATH}
        ${OPENGL_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        $ENV{SPDLOG_3RDPARTY_DIR}/include
        $ENV{VOLK_3RDPARTY_DIR}
        $ENV{VULKAN_SDK}/include
        $ENV{GLFW_3RDPARTY_DIR}/include
        )
target_link_libraries(UncannyGraphicsEngine
        PRIVATE
        ${OPENGL_LIBRARIES}
        #glfw
        #volk_headers
        )
target_compile_features(UncannyGraphicsEngine
        PUBLIC
        cxx_std_20
        )
