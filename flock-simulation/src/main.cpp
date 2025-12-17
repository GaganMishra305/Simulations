#include <iostream>
#include "Game.h"
#include <string>

int main(int argc, char* argv[])
{
    int boidCount = 500;
    float flockRadius = 40.0f;
    bool debug = false;
    bool useTree = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--boids" && i + 1 < argc) {
            boidCount = std::stoi(argv[++i]);
        } else if (arg == "--radius" && i + 1 < argc) {
            flockRadius = std::stof(argv[++i]);
        } else if (arg == "--debug" && i + 1 < argc) {
            std::string debugStr = argv[++i];
            debug = (debugStr == "true" || debugStr == "1" || debugStr == "True");
        } else if (arg == "--engine" && i + 1 < argc) {
            std::string engine = argv[++i];
            if (engine == "tree") useTree = true;
            else if (engine == "basic") useTree = false;
            else {
                std::cerr << "Unknown engine: " << engine << ". Use 'basic' or 'tree'.\n";
                return 1;
            }
        } else if (arg == "--help") {
            std::cout << "Usage: boids-sim [options]\n"
                      << "Options:\n"
                      << "  --boids <count>      Number of boids (default: 500)\n"
                      << "  --radius <float>     Flock interaction radius (default: 40.0)\n"
                      << "  --debug <true|false> Enable debug mode (default: false)\n"
                      << "  --engine <basic|tree>Choose naive or quadtree engine (default: basic)\n"
                      << "  --help               Show this help message\n";
            return 0;
        }
    }

    Game game(boidCount, flockRadius, debug, useTree);
    game.Run();
    return 0;
}
