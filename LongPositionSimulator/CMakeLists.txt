cmake_minimum_required(VERSION 3.10)
project(LongPositionSimulator)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Eigen3 package
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)

# Find the SFML package
find_package(SFML 3.0 COMPONENTS system window graphics network audio REQUIRED)

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

# Link the main executable with the required libraries
target_link_libraries(LongPositionSimulator ${SFML_LIBRARIES} Eigen3::Eigen)

# Optionally, disable building tests if not needed
set(BUILD_TESTING OFF)
