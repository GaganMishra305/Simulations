#!/bin/bash

# Clean script for flock-simulation project
# Removes all compiled object files and executables

cd "$(dirname "$0")/src"

echo "Cleaning flock-simulation..."
rm -f *.o boids-sim
echo "Clean complete!"
