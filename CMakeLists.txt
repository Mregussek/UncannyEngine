
cmake_minimum_required(VERSION 3.22)
project(UncannyEngine)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Binary Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project Dir: ${PROJECT_SOURCE_DIR}")

add_subdirectory(Engine)
add_subdirectory(Sandbox)
