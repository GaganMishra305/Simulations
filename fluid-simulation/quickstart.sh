#!/bin/bash
# Quick Start - Fluid Simulation

echo "╔════════════════════════════════════════════════════════════╗"
echo "║          Fluid Simulation - Quick Start Guide            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "❌ Error: make not found. Please install build-essential."
    exit 1
fi

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "❌ Error: g++ not found. Please install build-essential."
    exit 1
fi

# Check Eigen3
if [ ! -d "/usr/include/eigen3" ]; then
    echo "⚠️  Warning: Eigen3 not found at /usr/include/eigen3"
    echo "   Install with: sudo apt-get install libeigen3-dev"
    echo ""
fi

echo "📦 Building the project..."
echo ""

# Clean and build
make clean > /dev/null 2>&1
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Build failed. Check the error messages above."
    exit 1
fi

echo ""
echo "✅ Build successful!"
echo ""
echo "📋 Available commands:"
echo ""
echo "  Run tests:       make test"
echo "  Run simulator:   make run"
echo "  Clean build:     make clean"
echo "  Debug build:     make debug"
echo ""
echo "🚀 Starting simulator..."
echo ""
make run
