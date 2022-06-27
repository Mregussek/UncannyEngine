

file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        ${PROJECT_SOURCE_DIR}/Engine/*.cpp
        ${PROJECT_SOURCE_DIR}/Engine/*.inl
        ${PROJECT_SOURCE_DIR}/Engine/*.h
        )
add_library(UncannyLibrary ${ENGINE_LIBRARY_SOURCES})
target_include_directories(UncannyLibrary PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
