
# ------------------- Default Setup

cmake_minimum_required(VERSION 3.22)
project(UncannyEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_subdirectory(UMath)
add_subdirectory(UGraphicsEngine)
add_subdirectory(UTools)
add_subdirectory(USamples)
