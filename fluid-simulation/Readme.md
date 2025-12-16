# Fluid Simulation

A professional-grade fluid dynamics simulator using FLIP/PIC (Fluid Implicit Particle) method.

## Overview

This project implements a 3D incompressible fluid simulation using the FLIP/PIC approach with staggered grids and pressure solving.

**Original Tutorial:** https://unusualinsights.github.io/fluid_tutorial/

## Project Structure

```
├── src/                    # Source code (.cpp files)
├── include/                # Header files (.h files)
│   ├── json/              # JSON library headers
│   ├── Array3D.h          # 3D array data structure
│   ├── Particle.h         # Particle class
│   ├── StaggeredGrid.h    # Grid structure
│   ├── PressureSolver.h   # Pressure solving
│   └── ...
├── inputs/                 # Configuration and data files
│   ├── fluid.json         # Main simulation parameters
│   └── particles.in       # Initial particle data
├── build/                  # Build artifacts (generated)
├── bin/                    # Compiled executables (generated)
├── Makefile               # Build configuration
└── Readme.md              # This file
```

## Requirements

- **Compiler:** GCC 5.0+ or Clang 3.8+ (C++11 required)
- **Libraries:**
  - Eigen3 (for linear algebra)
  - GLUT and OpenGL (for ParticleViewer only)
- **Build Tool:** GNU Make

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential libeigen3-dev freeglut3-dev libgl1-mesa-dev
```

**macOS (Homebrew):**
```bash
brew install eigen glfw3 glut
```

## Building

### Basic Build
```bash
make
```

This compiles all targets:
- `FluidSimulator` - Main simulation engine
- `Array3DTest` - Unit tests for 3D array
- `StaggeredGridTest` - Unit tests for staggered grid
- `ParticleViewer` - OpenGL-based particle visualization

### Debug Build
```bash
make debug
```

Includes debug symbols and disables optimizations.

### Clean Build
```bash
make clean      # Remove build artifacts
make distclean  # Full clean
```

## Running

### Main Simulator
```bash
make run
```

Or manually:
```bash
./bin/FluidSimulator inputs/fluid.json
```

### Running Tests
```bash
make test
```

### Individual Tests
```bash
./bin/Array3DTest
./bin/StaggeredGridTest inputs/fluid.json
```

### Particle Viewer
```bash
./bin/ParticleViewer
```

## Configuration

Edit `inputs/fluid.json` to customize simulation parameters:

```json
{
  "timestep": 0.0416,
  "gravity": [0, -9.81, 0],
  "domain": {
    "x": 1.0,
    "y": 1.0,
    "z": 1.0
  },
  "grid": {
    "x": 10,
    "y": 10,
    "z": 10
  }
}
```

## Compilation Targets

| Target | Description |
|--------|-------------|
| `make` | Build all executables |
| `make debug` | Build with debug symbols |
| `make clean` | Remove build artifacts |
| `make run` | Run main simulator |
| `make test` | Run all tests |
| `make help` | Show available targets |

## Implementation Details

### Core Components

1. **Array3D** - Generic 3D array template container
2. **Particle** - Individual fluid particle representation
3. **StaggeredGrid** - Grid structure for velocity and pressure fields
4. **PressureSolver** - Incompressibility constraint solver
5. **SimulationParameters** - Configuration management

### Algorithm

The simulator uses the FLIP/PIC method:
1. Advection of particles
2. Grid-based force calculation
3. Pressure projection
4. Particle velocity update

## Output

Simulation generates:
- `.in` files - Particle positions and velocities
- Console output - Simulation progress and statistics

## Performance Notes

- Optimization flags: `-O3` for release builds
- Recommended for systems with Eigen3 and modern CPU
- GPU acceleration not currently implemented

## Development

### Adding New Features

1. Add headers to `include/`
2. Add implementation to `src/`
3. Update `Makefile` if adding new targets
4. Rebuild with `make clean && make`

### Code Style

- Use C++11 standard library
- Include guards: `FILENAME_H_`
- Consistent 2-space indentation

## References

- Original Tutorial: https://unusualinsights.github.io/fluid_tutorial/
- Eigen Library: http://eigen.tuxfamily.org/
- FLIP/PIC Method: Zhu & Bridson, "Animating sand as a fluid"