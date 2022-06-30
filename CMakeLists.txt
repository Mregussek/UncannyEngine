
cmake_minimum_required(VERSION 3.22)
project(UncannyEngine)

set(ENV{VULKAN_SDK} "D:/SDKs/Vulkan/Vulkan_1.3.126.0")

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Binary Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project Dir: ${PROJECT_SOURCE_DIR}")

find_package( Vulkan REQUIRED )
find_package( OpenGL REQUIRED )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/glfw)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/volk)

add_subdirectory(Engine)
add_subdirectory(Sandbox)
add_subdirectory(tests)
