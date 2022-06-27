
add_executable(UncannySandbox main.cpp)
target_include_directories(UncannySandbox PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
target_link_libraries(UncannySandbox PUBLIC UncannyLibrary)
