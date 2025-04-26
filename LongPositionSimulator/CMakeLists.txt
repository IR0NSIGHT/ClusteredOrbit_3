cmake_minimum_required(VERSION 3.28)
project(LongPositionSimulator)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# build SFML from source
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)


# Find the Eigen3 package
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)

# Include directories
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(include)

# List all source files
set(SOURCES
        src/BasicPuppetMaster.cpp
        src/Canvas.cpp
        src/linearInterceptEquation.cpp
        src/LongPositionSimulator.cpp
        src/MagicMissile.cpp
        src/MissileGuidance.cpp
        src/ObjectMetaInfo.cpp
        src/PlayerState.cpp
        src/PointDefenseAI.cpp
        src/polynomPower4.cpp
        src/positionable.cpp
        src/SpaceObject.cpp
        src/Updateable.cpp
        src/vel3d.cpp
        src/WorldState.cpp
        src/WorldStateEventHandler.cpp
        include/RouterPlanner.h
)

# Add the main executable
add_executable(LongPositionSimulator ${SOURCES})

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


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        hello_test.cc
)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)

