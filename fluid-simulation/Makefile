# Fluid Simulation Makefile
# Professional build configuration

# Compiler and flags
CXX          := g++
CXXFLAGS     := -std=c++11 -Wall -Wextra -O3
CXXFLAGS_DEBUG := -std=c++11 -Wall -Wextra -g -O0

# Directories
SRC_DIR      := src
BUILD_DIR    := build
BIN_DIR      := bin
OUTPUT_DIR   := outputs
INCLUDE_DIR  := include

# Include paths
INCLUDES     := -I$(INCLUDE_DIR) -I/usr/include/eigen3

# Library paths and libraries
LDFLAGS      := -L/usr/lib/x86_64-linux-gnu
LDLIBS_BASE  := 
LDLIBS_GL    := -lGL -lglut -lGLU

# Source files
CORE_SOURCES := $(SRC_DIR)/jsoncpp.cpp \
                $(SRC_DIR)/Particle.cpp \
                $(SRC_DIR)/PressureSolver.cpp \
                $(SRC_DIR)/SimulationParameters.cpp \
                $(SRC_DIR)/StaggeredGrid.cpp

CORE_OBJECTS := $(BUILD_DIR)/jsoncpp.o \
                $(BUILD_DIR)/Particle.o \
                $(BUILD_DIR)/PressureSolver.o \
                $(BUILD_DIR)/SimulationParameters.o \
                $(BUILD_DIR)/StaggeredGrid.o

# Target executables
TARGETS      := $(BIN_DIR)/FluidSimulator \
                $(BIN_DIR)/Array3DTest \
                $(BIN_DIR)/StaggeredGridTest \
                $(BIN_DIR)/ParticleViewer

# Default target
.PHONY: all
all: $(TARGETS)
	@mkdir -p $(OUTPUT_DIR)

# Debug build
.PHONY: debug
debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: clean $(TARGETS)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# Core object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# FluidSimulator
$(BIN_DIR)/FluidSimulator: $(CORE_OBJECTS) $(BUILD_DIR)/FluidSimulator.o | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS_BASE) -o $@
	@echo "✓ Built: $@"

# Array3DTest
$(BIN_DIR)/Array3DTest: $(BUILD_DIR)/Array3DTest.o | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS_BASE) -o $@
	@echo "✓ Built: $@"

# StaggeredGridTest
$(BIN_DIR)/StaggeredGridTest: $(CORE_OBJECTS) $(BUILD_DIR)/StaggeredGridTest.o | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS_BASE) -o $@
	@echo "✓ Built: $@"

# ParticleViewer
$(BIN_DIR)/ParticleViewer: $(BUILD_DIR)/ParticleViewer.o | $(BIN_DIR)
	@echo "Linking $@ (with OpenGL)..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS_GL) -o $@
	@echo "✓ Built: $@"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@[ -d $(OUTPUT_DIR) ] && rm -f $(OUTPUT_DIR)/*.part $(OUTPUT_DIR)/*.png 2>/dev/null || true
	@[ -d $(OUTPUT_DIR) ] && find $(OUTPUT_DIR) -maxdepth 1 -type f -name '*.in' -delete || true
	@rmdir --ignore-fail-on-non-empty $(OUTPUT_DIR) 2>/dev/null || true

# Clean everything including bin directory
.PHONY: distclean
distclean: clean
	@echo "Dist clean complete"

# Run main simulator
.PHONY: run
run: $(BIN_DIR)/FluidSimulator
	@mkdir -p $(OUTPUT_DIR)
	@$(BIN_DIR)/FluidSimulator inputs/fluid.json

# Run tests
.PHONY: test
test: $(BIN_DIR)/Array3DTest $(BIN_DIR)/StaggeredGridTest
	@echo "\n=== Running Array3D tests ==="
	@$(BIN_DIR)/Array3DTest
	@echo "\n=== Running StaggeredGrid tests ==="
	@mkdir -p $(OUTPUT_DIR)
	@$(BIN_DIR)/StaggeredGridTest inputs/fluid.json

# Help target
.PHONY: help
help:
	@echo "Fluid Simulation - Available Targets:"
	@echo "  make              - Build all executables"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make distclean    - Full clean"
	@echo "  make run          - Run the main simulator"
	@echo "  make test         - Run tests"
	@echo "  make help         - Show this help message"

# Prevent issues with files named like targets
.PRECIOUS: $(BUILD_DIR)/%.o
