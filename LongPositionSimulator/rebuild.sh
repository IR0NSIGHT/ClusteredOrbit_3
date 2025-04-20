#!/bin/bash

# Define the build directory
BUILD_DIR="build"

# Remove the existing build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Create a new build directory
echo "Creating new build directory: $BUILD_DIR"
mkdir "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR"

# Configure the project with CMake
echo "Configuring the project with CMake..."
cmake ..

# Build the project
echo "Building the project..."
make

# Return to the original directory
cd ..

echo "Rebuild complete."
