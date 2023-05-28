
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

# Dear ImGui
set(ENV{IMGUI_3RDPARTY_DIR} "${UGRAPHICSENGINE_PATH}/3rdparty/imgui")
set(IMGUI_SOURCE_FILES
        $ENV{IMGUI_3RDPARTY_DIR}/backends/imgui_impl_glfw.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/backends/imgui_impl_vulkan.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/imgui.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/imgui_demo.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/imgui_draw.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/imgui_widgets.cpp
        $ENV{IMGUI_3RDPARTY_DIR}/imgui_tables.cpp
        )
set(IMGUI_HEADER_FILES
        $ENV{IMGUI_3RDPARTY_DIR}/backends/imgui_impl_glfw.h
        $ENV{IMGUI_3RDPARTY_DIR}/backends/imgui_impl_vulkan.h
        $ENV{IMGUI_3RDPARTY_DIR}/imgui.h
        $ENV{IMGUI_3RDPARTY_DIR}/imconfig.h
        $ENV{IMGUI_3RDPARTY_DIR}/imgui_internal.h
        $ENV{IMGUI_3RDPARTY_DIR}/imstb_rectpack.h
        $ENV{IMGUI_3RDPARTY_DIR}/imstb_textedit.h
        $ENV{IMGUI_3RDPARTY_DIR}/imstb_truetype.h
        )
set(IMGUI_LIBRARY_FILES ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES})

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
        ${IMGUI_LIBRARY_FILES}
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
        $ENV{IMGUI_3RDPARTY_DIR}
        )
target_link_libraries(UncannyGraphicsEngine
        PRIVATE
        ${OPENGL_LIBRARIES}
        volk_headers
        UncannyTools
        UncannyMath
        ${GLSLANG_3RDPARTY_LIBS}
        )
target_compile_features(UncannyGraphicsEngine
        PUBLIC
        cxx_std_20
        )
