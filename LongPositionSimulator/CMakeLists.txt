cmake_minimum_required(VERSION 3.28)
project(LongPositionSimulator)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FetchContent
include(FetchContent)

# Build SFML from source
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

# Find the Eigen3 package
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)

# Include directories
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(include)

# List all source files except test files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add the main executable
add_executable(LongPositionSimulator LongPositionSimulator.cpp ${SOURCES})

target_compile_features(LongPositionSimulator PRIVATE cxx_std_17)
target_link_libraries(LongPositionSimulator SFML::Graphics Eigen3::Eigen)

# Define the resource files to copy
set(RESOURCE_FILES
        ${CMAKE_SOURCE_DIR}/resources/4-Vesta.png
        ${CMAKE_SOURCE_DIR}/resources/SpaceShip.png
        ${CMAKE_SOURCE_DIR}/resources/OSIRIS_Mars_true_color.png
        ${CMAKE_SOURCE_DIR}/resources/sternen_himmel.PNG
        ${CMAKE_SOURCE_DIR}/resources/MartianMono.ttf
)

# Define the destination directory
set(DESTINATION_DIR ${CMAKE_BINARY_DIR}/bin)

# Create the destination directory if it doesn't exist
file(MAKE_DIRECTORY ${DESTINATION_DIR})

# Copy the resource files to the destination directory
foreach(RESOURCE_FILE ${RESOURCE_FILES})
    add_custom_command(TARGET LongPositionSimulator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RESOURCE_FILE}
            ${DESTINATION_DIR}
    )
endforeach()


########################### UNIT TESTS ####################################


# Include GoogleTest
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add the test executable
add_executable(runTests ${SOURCES}
        test_main.cpp
        src/positionable.test.cpp
        src/polynomPower4.test.cpp
        src/Collission.test.cpp)

# Link GoogleTest and other necessary libraries to the test executable
target_link_libraries(runTests GTest::gtest GTest::gtest_main SFML::Graphics Eigen3::Eigen)

# Include directories for your main project's headers
target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Discover and register tests
include(GoogleTest)
gtest_discover_tests(runTests)
