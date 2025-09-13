#!/bin/bash
rm -r build
mkdir -p build
cd build

# Configure project (re-run CMake each time to catch changes)
cmake ../

# Build everything
cmake --build .
