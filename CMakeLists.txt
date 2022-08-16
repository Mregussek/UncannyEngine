
# ------------------- Default Setup

cmake_minimum_required(VERSION 3.22)
project(UncannyEngine)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.216.0")
set(CMAKE_CXX_STANDARD 20)

# ------------------- Loading dependencies

# Vulkan
find_package( Vulkan REQUIRED )

# Volk
set(ENV{VOLK_3RDPARTY_DIR} "${PROJECT_SOURCE_DIR}/3rdparty/volk")
add_subdirectory($ENV{VOLK_3RDPARTY_DIR})

# OpenGL
find_package( OpenGL REQUIRED )

# GLFW
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set(ENV{GLFW_3RDPARTY_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/glfw)
add_subdirectory($ENV{GLFW_3RDPARTY_DIR})

# spdlog
set(ENV{SPDLOG_3RDPARTY_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/spdlog)

# ------------------- Some Info display

message(STATUS "Binary Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project Dir: ${PROJECT_SOURCE_DIR}")
message(STATUS "Vulkan Dir: $ENV{VULKAN_SDK}")
message(STATUS "Volk Dir: $ENV{VOLK_3RDPARTY_DIR}")
message(STATUS "spdlog Dir: $ENV{SPDLOG_3RDPARTY_DIR}")
message(STATUS "GLFW Dir: $ENV{GLFW_3RDPARTY_DIR}")

# ------------------- Loading all projects for UncannyEngine

# Engine itself as library
add_subdirectory(Engine)

# Sandbox as a executable game
add_subdirectory(Sandbox)

# Unit Tests
add_subdirectory(tests)
