#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "Building Fluid Simulation..."

# Compile the application
g++ -o fluid_simulation \
    source/Main.cpp \
    source/Application.cpp \
    source/Fluid.cpp \
    source/Renderer.cpp \
    source/Mouse.cpp \
    -I lib/SDL2-2.0.16/include \
    -L lib/SDL2-2.0.16/lib \
    -lSDL2 \
    -Wl,-rpath,lib/SDL2-2.0.16/lib

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build successful!${NC}"
    echo "Binary created: ./fluid_simulation"
    echo "Run with: ./fluid_simulation"
else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi