
set(UTOOLS_PATH ${PROJECT_SOURCE_DIR}/UTools)

# GLFW
set(ENV{GLFW_PRECOMPILED_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/glfw_win64")
set(GLFW_INCLUDE_DIR "$ENV{GLFW_PRECOMPILED_3RDPARTY_DIR}/include")
set(GLFW_LIBRARY "$ENV{GLFW_PRECOMPILED_3RDPARTY_DIR}/lib-vc2022/glfw3.lib")

# assimp
set(ENV{ASSIMP_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/assimp")
add_subdirectory($ENV{ASSIMP_3RDPARTY_DIR})

# spdlog
set(ENV{SPDLOG_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/spdlog")
file(GLOB_RECURSE SPDLOG_LIBRARY_SOURCES
        $ENV{SPDLOG_3RDPARTY_DIR}/include/spdlog/*.h
        )

# entt
set(ENV{ENTT_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/entt/single_include")
set(ENTT_LIBRARY_SOURCES $ENV{ENTT_3RDPARTY_DIR}/entt/entt.hpp)

# nlohmann json
set(ENV{NLOHMANN_JSON_3RDPARTY_DIR} "${UTOOLS_PATH}/3rdparty/json/include")
set(NLOHMANN_JSON_LIBRARY_SOURCES "$ENV{NLOHMANN_JSON_3RDPARTY_DIR}/nlohmann/json.hpp")

# --------------------------------------------------------------------------------
file(GLOB_RECURSE TOOLS_LIBRARY_SOURCES
        ${UTOOLS_PATH}/*.cpp
        ${UTOOLS_PATH}/*.inl
        ${UTOOLS_PATH}/*.h
        )
list(FILTER TOOLS_LIBRARY_SOURCES EXCLUDE REGEX "${UTOOLS_PATH}/3rdparty/.*" )

add_library(UncannyTools
        ${TOOLS_LIBRARY_SOURCES}
        ${SPDLOG_LIBRARY_SOURCES}
        ${ENTT_LIBRARY_SOURCES}
        ${NLOHMANN_JSON_LIBRARY_SOURCES}
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
        $ENV{ENTT_3RDPARTY_DIR}
        $ENV{ASSIMP_3RDPARTY_DIR}/include
        $ENV{NLOHMANN_JSON_3RDPARTY_DIR}
        )
target_link_libraries(UncannyTools
        PRIVATE
        ${GLFW_LIBRARY}
        assimp
        )
target_compile_features(UncannyTools
        PUBLIC
        cxx_std_20
        )
