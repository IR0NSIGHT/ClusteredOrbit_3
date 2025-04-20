cmake_minimum_required(VERSION 3.31)
project(ClusteredOrbit_3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

# Find the Eigen3 package
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)

# Include the Eigen headers
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(dependencies/SFML-3.0.0/include)
include_directories(dependencies/SFML-3.0.0/include/SFML)
include_directories(dependencies/SFML-3.0.0/include/SFML/Audio)
include_directories(dependencies/SFML-3.0.0/include/SFML/Graphics)
include_directories(dependencies/SFML-3.0.0/include/SFML/Network)
include_directories(dependencies/SFML-3.0.0/include/SFML/System)
include_directories(dependencies/SFML-3.0.0/include/SFML/Window)

add_executable(ClusteredOrbit_3
        src/BasicPuppetMaster.cpp
        include/BasicPuppetMaster.h
        src/Canvas.cpp
        include/Canvas.h
        include/Collission.h
        src/linearInterceptEquation.cpp
        include/linearInterceptEquation.h
        src/LongPositionSimulator.cpp
        src/MagicMissile.cpp
        include/MagicMissile.h
        src/MissileGuidance.cpp
        include/MissileGuidance.h
        src/ObjectMetaInfo.cpp
        include/ObjectMetaInfo.h
        src/PlayerState.cpp
        include/PlayerState.h
        src/PointDefenseAI.cpp
        include/PointDefenseAI.h
        src/polynomPower4.cpp
        include/polynomPower4.h
        src/positionable.cpp
        include/positionable.h
        include/shipIds.h
        src/SpaceObject.cpp
        include/SpaceObject.h
        src/Updateable.cpp
        include/Updateable.h
        src/vel3d.cpp
        include/vel3d.h
        src/WorldState.cpp
        include/WorldState.h
        src/WorldStateEventHandler.cpp
        include/WorldStateEventHandler.h)