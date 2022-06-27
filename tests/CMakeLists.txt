
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(FirstTest first_test.cpp)
target_include_directories(FirstTest PUBLIC ${PROJECT_SOURCE_DIR}/Engine)
target_link_libraries(FirstTest PRIVATE GTest::GTest UncannyLibrary)

add_test(FirstTest_GTest FirstTest)
