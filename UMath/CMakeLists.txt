
set(UMATH_PATH ${PROJECT_SOURCE_DIR}/UMath)

file(GLOB_RECURSE MATH_LIBRARY_SOURCES
        ${UMATH_PATH}/*.cpp
        ${UMATH_PATH}/*.inl
        ${UMATH_PATH}/*.h
        )
add_library(UncannyMath ${MATH_LIBRARY_SOURCES})
set_target_properties(UncannyMath PROPERTIES LINKER_LANGUAGE C++)
target_include_directories(UncannyMath PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_features(UncannyMath PUBLIC cxx_std_20)
