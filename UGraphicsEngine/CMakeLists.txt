
set(UGRAPHICSENGINE_PATH ${PROJECT_SOURCE_DIR}/UGraphicsEngine/)

# Vulkan
find_package(Vulkan REQUIRED)

# Volk
set(ENV{VOLK_3RDPARTY_DIR} "${UGRAPHICSENGINE_PATH}/3rdparty/volk")
add_subdirectory($ENV{VOLK_3RDPARTY_DIR})

# glslang
set(ENV{GLSLANG_3RDPARTY_DIR} "${UGRAPHICSENGINE_PATH}/3rdparty/glslang")
add_subdirectory($ENV{GLSLANG_3RDPARTY_DIR})
set(GLSLANG_3RDPARTY_LIBS glslang SPIRV)

# OpenGL
find_package(OpenGL REQUIRED)

# Get all engine files without 3rdparty ones
file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        ${UGRAPHICSENGINE_PATH}/*.cpp
        ${UGRAPHICSENGINE_PATH}/*.inl
        ${UGRAPHICSENGINE_PATH}/*.h
        )
list(FILTER ENGINE_LIBRARY_SOURCES EXCLUDE REGEX "${UGRAPHICSENGINE_PATH}/3rdparty/.*" )

# Create a library for UncannyGraphicsEngine
add_library(UncannyGraphicsEngine
        ${ENGINE_LIBRARY_SOURCES}
        )
set_target_properties(UncannyGraphicsEngine
        PROPERTIES
        LINKER_LANGUAGE C++)
target_include_directories(UncannyGraphicsEngine
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        $ENV{VOLK_3RDPARTY_DIR}
        $ENV{GLSLANG_3RDPARTY_DIR}
        $ENV{VULKAN_SDK}/include
        )
target_link_libraries(UncannyGraphicsEngine
        PRIVATE
        ${OPENGL_LIBRARIES}
        volk_headers
        UncannyTools
        ${GLSLANG_3RDPARTY_LIBS}
        )
target_compile_features(UncannyGraphicsEngine
        PUBLIC
        cxx_std_20
        )
