enable_testing()

# Add the test executable
add_executable(runTests test_main.cpp )
#foreach(src ${SOURCES})
#    target_sources(runTests PRIVATE ${CMAKE_SOURCE_DIR}/${src})
#endforeach()

# Link against GoogleTest
target_link_libraries(runTests GTest::gtest GTest::gtest_main)

# Include directories for your main project's headers
target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(runTests)