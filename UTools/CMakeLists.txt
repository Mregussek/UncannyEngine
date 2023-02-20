
set(UTOOLS_PATH ${PROJECT_SOURCE_DIR}/UTools)

# GLFW
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
#set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
#set(ENV{GLFW_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/glfw")
#add_subdirectory($ENV{GLFW_3RDPARTY_DIR})
set(ENV{GLFW_PRECOMPILED_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/glfw_win64")
set(GLFW_INCLUDE_DIR "$ENV{GLFW_PRECOMPILED_3RDPARTY_DIR}/include")
set(GLFW_LIBRARY "$ENV{GLFW_PRECOMPILED_3RDPARTY_DIR}/lib-vc2022/glfw3.lib")

# spdlog
set(ENV{SPDLOG_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/spdlog")
file(GLOB_RECURSE SPDLOG_LIBRARY_SOURCES
        $ENV{SPDLOG_3RDPARTY_DIR}/include/spdlog/*.h
        )

file(GLOB_RECURSE TOOLS_LIBRARY_SOURCES
        ${UTOOLS_PATH}/*.cpp
        ${UTOOLS_PATH}/*.inl
        ${UTOOLS_PATH}/*.h
        )
list(FILTER TOOLS_LIBRARY_SOURCES EXCLUDE REGEX "${UTOOLS_PATH}/3rdparty/.*" )
add_library(UncannyTools
        ${TOOLS_LIBRARY_SOURCES}
        ${SPDLOG_LIBRARY_SOURCES}
        )
set_target_properties(UncannyTools
        PROPERTIES
        LINKER_LANGUAGE C++
        )
target_include_directories(UncannyTools
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        $ENV{SPDLOG_3RDPARTY_DIR}/include
        ${GLFW_INCLUDE_DIR}
        )
target_link_libraries(UncannyTools
        PRIVATE
        ${GLFW_LIBRARY}
        )
target_compile_features(UncannyTools
        PUBLIC
        cxx_std_20
        )
