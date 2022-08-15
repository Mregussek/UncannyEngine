
add_executable(UncannySandbox main.cpp)
target_include_directories(UncannySandbox PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
target_link_libraries(UncannySandbox PUBLIC UncannyLibrary)
target_compile_features(UncannySandbox PUBLIC cxx_std_20)
