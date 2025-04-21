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
)

# Add the main executable
add_executable(LongPositionSimulator ${SOURCES})

target_compile_features(LongPositionSimulator PRIVATE cxx_std_17)
target_link_libraries(LongPositionSimulator SFML::Graphics Eigen3::Eigen)

# Optionally, disable building tests if not needed
set(BUILD_TESTING OFF)
