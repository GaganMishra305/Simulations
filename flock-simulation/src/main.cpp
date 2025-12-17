#include <iostream>
#include "Game.h"
#include <string>

int main(int argc, char* argv[])
{
    int boidCount = 500;
    float flockRadius = 40.0f;
    bool debug = false;

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
        } else if (arg == "--help") {
            std::cout << "Usage: boids-sim [options]\n"
                      << "Options:\n"
                      << "  --boids <count>    Number of boids (default: 500)\n"
                      << "  --radius <float>   Flock interaction radius (default: 40.0)\n"
                      << "  --debug <true|false> Enable debug mode (default: false)\n"
                      << "  --help             Show this help message\n";
            return 0;
        }
    }

    Game game(boidCount, flockRadius, debug);
    game.Run();
    return 0;
}
