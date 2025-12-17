#include <iostream>
#include "Flock.h"
#include "Boid.h"
#include "Pvector.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#ifndef GAME_H
#define GAME_H

// Game handles the instantiation of a flock of boids, game input, asks the
// model to compute the next step in the stimulation, and handles all of the
// program's interaction with SFML. 

class Game {
private:
    sf::RenderWindow window;
    int window_width;
    int window_height;

    Flock flock;
    float boidsSize;
    vector<sf::CircleShape> shapes;

    sf::Clock clock;
    sf::Font font;
    int frameCount;
    float fps;
    int boidCount;
    float flockRadius;
    bool debugMode;

    void Render();
    void HandleInput();

public:
    Game(int boidCount = 500, float flockRadius = 40.0, bool debug = false);
    void Run();
};

#endif
