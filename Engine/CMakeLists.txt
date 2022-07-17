
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

file(GLOB_RECURSE SPDLOG_LIBRARY_SOURCES
        $ENV{SPDLOG_3RDPARTY_DIR}/include/spdlog/*.h
        )

file(GLOB_RECURSE ENGINE_LIBRARY_SOURCES
        ${PROJECT_SOURCE_DIR}/Engine/*.cpp
        ${PROJECT_SOURCE_DIR}/Engine/*.inl
        ${PROJECT_SOURCE_DIR}/Engine/*.h
        )
add_library(UncannyLibrary ${ENGINE_LIBRARY_SOURCES} ${SPDLOG_LIBRARY_SOURCES})
target_include_directories(UncannyLibrary
        PUBLIC
        ${PROJECT_SOURCE_DIR}/Engine
        ${OPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS}
        $ENV{SPDLOG_3RDPARTY_DIR}/include
        $ENV{VOLK_3RDPARTY_DIR}
        $ENV{VULKAN_SDK}/include
        $ENV{GLFW_3RDPARTY_DIR}/include
        )
target_link_libraries(UncannyLibrary PRIVATE ${OPENGL_LIBRARIES} glfw volk_headers)
target_compile_features(UncannyLibrary PUBLIC cxx_std_20)

if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0")
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GLFW-CMake-starter )
    endif()
endif()
