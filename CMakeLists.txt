cmake_minimum_required(VERSION 3.22)
project(UncannyEngine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        Engine/*.cpp
        Engine/*.inl
        Engine/*.h
)
add_library(UncannyLibrary ${ENGINE_LIBRARY_SOURCES})

add_executable(UncannySandbox Sandbox/main.cpp)
target_include_directories(UncannySandbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
target_link_libraries(UncannySandbox PUBLIC UncannyLibrary)
