#include <iostream>
#include "Flock.h"
#include "Boid.h"
#include "Pvector.h"
#include "Game.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include <random>

const int slideroffset = 1000;
// Map HSV color to RGB for smooth speed-based coloring
static sf::Color hsvToRgb(float h, float s, float v)
{
    while (h < 0) h += 360.f;
    while (h >= 360.f) h -= 360.f;
    s = std::max(0.f, std::min(1.f, s));
    v = std::max(0.f, std::min(1.f, v));

    float c = v * s;
    float x = c * (1.f - std::fabs(fmod(h / 60.f, 2.f) - 1.f));
    float m = v - c;

    float r1, g1, b1;
    if (h < 60)      { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120){ r1 = x; g1 = c; b1 = 0; }
    else if (h < 180){ r1 = 0; g1 = c; b1 = x; }
    else if (h < 240){ r1 = 0; g1 = x; b1 = c; }
    else if (h < 300){ r1 = x; g1 = 0; b1 = c; }
    else             { r1 = c; g1 = 0; b1 = x; }

    sf::Uint8 R = static_cast<sf::Uint8>((r1 + m) * 255.f);
    sf::Uint8 G = static_cast<sf::Uint8>((g1 + m) * 255.f);
    sf::Uint8 B = static_cast<sf::Uint8>((b1 + m) * 255.f);
    return sf::Color(R, G, B);
}

static sf::Color colorFromVelocity(const Boid& b)
{
    // Normalize speed to [0,1] using boid's maxSpeed
    float speed = std::sqrt(b.velocity.x * b.velocity.x + b.velocity.y * b.velocity.y);
    float t = 0.f;
    if (b.maxSpeed > 0) t = std::min(1.f, std::max(0.f, speed / b.maxSpeed));
    // Hue from 220 (blue) at slow to 0 (red) at fast
    float hue = (1.f - t) * 220.f;
    return hsvToRgb(hue, 1.f, 1.f);
}

// Construct window using SFML
Game::Game(int boidCount, float flockRadius, bool debug, bool useTree) : flock(flockRadius)
{   
    this->boidCount = boidCount;
    this->flockRadius = flockRadius;
    this->debugMode = debug;
    this->useTreeEngine = useTree;
    this->boidsSize = 3.0;
    this->frameCount = 0;
    this->fps = 0.0;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    this->window_height = desktop.height;
    this->window_width  = desktop.width;
    this->window.create(sf::VideoMode(window_width, window_height, desktop.bitsPerPixel), "Boids", sf::Style::None);
    
    // Try to achieve 60 FPS.
    window.setFramerateLimit(60);

    // Load font for text rendering
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Warning: Could not load font. Text will not be displayed." << std::endl;
    }

    // Configure flock engine and bounds
    flock.setBounds(window_width, window_height);
    flock.setUseTree(useTreeEngine);
    flock.setWeights(sepWeight, aliWeight, cohWeight);
}

// Run the simulation. Run creates the boids that we'll display, checks for user
// input, and updates the view
void Game::Run()
{
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // Spawn boids uniformly inside a small circle around screen center
    const float centerX = window_width * 0.5f;
    const float centerY = window_height * 0.5f;
    const float spawnRadius = 250; // 200 spwan radius for reducing the jitter at start
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.14159265f);
    std::uniform_real_distribution<float> unitDist(0.f, 1.f);

    for (int i = 0; i < boidCount; i++) {
        // Generate random position uniformly within circle
        float angle = angleDist(gen);
        float r = spawnRadius * std::sqrt(unitDist(gen));
        float randomX = centerX + r * std::cos(angle);
        float randomY = centerY + r * std::sin(angle);
        Boid b(randomX, randomY); // Starts boids at random positions on screen
        sf::CircleShape shape(8, 3);

        // Changing the Visual Properties of the shape
        shape.setPosition(b.location.x, b.location.y); // Match shape to initial boid location
        shape.setOutlineColor(sf::Color(0,255,0));
        shape.setFillColor(sf::Color::Green);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(1);
        shape.setRadius(boidsSize);

        // Adding the boid to the flock and adding the shapes to the vector<sf::CircleShape>
        flock.addBoid(b);
        shapes.push_back(shape);
    }
    while (window.isOpen()) {
        HandleInput();
        Render();
    }
}

void Game::HandleInput()
{
    sf::Event event;
    while (window.pollEvent(event)) {
        // "close requested" event: we close the window
        // Implemented alternate ways to close the window. (Pressing the escape, X, and BackSpace key also close the program.)
        if ((event.type == sf::Event::Closed) ||
            (event.type == sf::Event::KeyPressed &&
             event.key.code == sf::Keyboard::Escape) ||
            (event.type == sf::Event::KeyPressed &&
             event.key.code == sf::Keyboard::BackSpace) ||
            (event.type == sf::Event::KeyPressed &&
             event.key.code == sf::Keyboard::X))
             {
            window.close();
        }
    }

    // Handle slider interaction
    sf::Vector2i mouseCoords = sf::Mouse::getPosition(window);
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    handleSliderInteraction(mouseCoords, mousePressed);

    // Check for mouse click, draws and adds boid to flock if so.
    if (mousePressed) {
        // Only add boid if not interacting with sliders
        if (!draggingSepSlider && !draggingAliSlider && !draggingCohSlider) {
            Boid b(mouseCoords.x, mouseCoords.y, false);
            sf::CircleShape shape(10,3);

            // Changing visual properties of newly created boid
            shape.setPosition(mouseCoords.x, mouseCoords.y);
            shape.setOutlineColor(sf::Color(255, 0, 0));
            shape.setFillColor(sf::Color(255, 0, 0));
            shape.setOutlineColor(sf::Color::White);
            shape.setOutlineThickness(1);
            shape.setRadius(boidsSize);

            // Adds newly created boid and shape to their respective data structure
            flock.addBoid(b);
            shapes.push_back(shape);

            // New Shape is drawn
            window.draw(shapes[shapes.size()-1]);
        }
    } else {
        // Reset dragging flags when mouse is released
        draggingSepSlider = false;
        draggingAliSlider = false;
        draggingCohSlider = false;
    }
}

void Game::Render()
{
    window.clear();

    // Calculate FPS
    frameCount++;
    if (clock.getElapsedTime().asSeconds() >= 1.0f) {
        fps = frameCount;
        frameCount = 0;
        clock.restart();
    }

    // Draws all of the Boids out, and applies functions that are needed to update.
    for (int i = 0; i < shapes.size(); i++) {
        window.draw(shapes[i]);

        // Draw flock radius circle for debugging
        if(debugMode){
            sf::CircleShape radiusCircle(flockRadius);
            radiusCircle.setPosition(flock.getBoid(i).location.x - flockRadius, flock.getBoid(i).location.y - flockRadius);
            radiusCircle.setFillColor(sf::Color::Transparent);
            radiusCircle.setOutlineThickness(1);
            radiusCircle.setOutlineColor(sf::Color(100, 100, 100, 100));
            window.draw(radiusCircle);
        }

        //cout << "Boid "<< i <<" Coordinates: (" << shapes[i].getPosition().x << ", " << shapes[i].getPosition().y << ")" << endl;
        //cout << "Boid Code " << i << " Location: (" << flock.getBoid(i).location.x << ", " << flock.getBoid(i).location.y << ")" << endl;

        // Matches up the location of the shape to the boid
        shapes[i].setPosition(flock.getBoid(i).location.x, flock.getBoid(i).location.y);

        // Calculates the angle where the velocity is pointing so that the triangle turns towards it.
        float theta = flock.getBoid(i).angle(flock.getBoid(i).velocity);
        shapes[i].setRotation(theta);

        // Color based on velocity magnitude for visual appeal
        shapes[i].setFillColor(colorFromVelocity(flock.getBoid(i)));

        // Prevent boids from moving off the screen through wrapping
        // If boid exits right boundary
        if (shapes[i].getPosition().x > window_width)
            shapes[i].setPosition(shapes[i].getPosition().x - window_width, shapes[i].getPosition().y);
        // If boid exits bottom boundary
        if (shapes[i].getPosition().y > window_height)
            shapes[i].setPosition(shapes[i].getPosition().x, shapes[i].getPosition().y - window_height);
        // If boid exits left boundary
        if (shapes[i].getPosition().x < 0)
            shapes[i].setPosition(shapes[i].getPosition().x + window_width, shapes[i].getPosition().y);
        // If boid exits top boundary
        if (shapes[i].getPosition().y < 0)
            shapes[i].setPosition(shapes[i].getPosition().x, shapes[i].getPosition().y + window_height);
    }

    // Applies the three rules to each boid in the flock and changes them accordingly.
    flock.flocking();

    // Display FPS and Flock Size on screen
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setString("FPS: " + std::to_string((int)fps));
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);
    window.draw(fpsText);

    sf::Text flockSizeText;
    flockSizeText.setFont(font);
    flockSizeText.setString("Boids: " + std::to_string(flock.getSize()));
    flockSizeText.setCharacterSize(20);
    flockSizeText.setFillColor(sf::Color::White);
    flockSizeText.setPosition(10, 40);
    window.draw(flockSizeText);

    // Render sliders
    renderSliders();
    flock.setWeights(sepWeight, aliWeight, cohWeight);
    window.display();
}

void Game::renderSliders()
{
    int sliderY = window_height - slideroffset;
    int sliderX = 50;
    int sliderWidth = 200;
    int sliderHeight = 10;
    int spacing = 80;

    // Slider background boxes
    sf::RectangleShape sepSliderBg(sf::Vector2f(sliderWidth, sliderHeight));
    sf::RectangleShape aliSliderBg(sf::Vector2f(sliderWidth, sliderHeight));
    sf::RectangleShape cohSliderBg(sf::Vector2f(sliderWidth, sliderHeight));

    sepSliderBg.setPosition(sliderX, sliderY);
    aliSliderBg.setPosition(sliderX, sliderY + spacing);
    cohSliderBg.setPosition(sliderX, sliderY + spacing * 2);

    sepSliderBg.setFillColor(sf::Color(50, 50, 50));
    aliSliderBg.setFillColor(sf::Color(50, 50, 50));
    cohSliderBg.setFillColor(sf::Color(50, 50, 50));

    window.draw(sepSliderBg);
    window.draw(aliSliderBg);
    window.draw(cohSliderBg);

    // Slider handles (circles)
    float sepHandleX = sliderX + (sepWeight / 3.0f) * sliderWidth; // Assume max weight is 3.0
    float aliHandleX = sliderX + (aliWeight / 3.0f) * sliderWidth;
    float cohHandleX = sliderX + (cohWeight / 3.0f) * sliderWidth;

    sf::CircleShape sepHandle(10);
    sf::CircleShape aliHandle(10);
    sf::CircleShape cohHandle(10);

    sepHandle.setPosition(sepHandleX - 10, sliderY - 5);
    aliHandle.setPosition(aliHandleX - 10, sliderY + spacing - 5);
    cohHandle.setPosition(cohHandleX - 10, sliderY + spacing * 2 - 5);

    sepHandle.setFillColor(sf::Color::Red);
    aliHandle.setFillColor(sf::Color::Green);
    cohHandle.setFillColor(sf::Color::Blue);

    window.draw(sepHandle);
    window.draw(aliHandle);
    window.draw(cohHandle);

    // Labels
    sf::Text sepLabel;
    sepLabel.setFont(font);
    sepLabel.setString("Sep: " + std::to_string(sepWeight).substr(0, 4));
    sepLabel.setCharacterSize(16);
    sepLabel.setFillColor(sf::Color::White);
    sepLabel.setPosition(sliderX + sliderWidth + 20, sliderY);
    window.draw(sepLabel);

    sf::Text aliLabel;
    aliLabel.setFont(font);
    aliLabel.setString("Ali: " + std::to_string(aliWeight).substr(0, 4));
    aliLabel.setCharacterSize(16);
    aliLabel.setFillColor(sf::Color::White);
    aliLabel.setPosition(sliderX + sliderWidth + 20, sliderY + spacing);
    window.draw(aliLabel);

    sf::Text cohLabel;
    cohLabel.setFont(font);
    cohLabel.setString("Coh: " + std::to_string(cohWeight).substr(0, 4));
    cohLabel.setCharacterSize(16);
    cohLabel.setFillColor(sf::Color::White);
    cohLabel.setPosition(sliderX + sliderWidth + 20, sliderY + spacing * 2);
    window.draw(cohLabel);
}

void Game::handleSliderInteraction(sf::Vector2i mousePos, bool mousePressed)
{
    int sliderY = window_height - slideroffset;
    int sliderX = 50;
    int sliderWidth = 200;
    int sliderHeight = 10;
    int spacing = 80;

    // Check if mouse is near separation slider
    if (mousePos.y >= sliderY - 15 && mousePos.y <= sliderY + sliderHeight + 15) {
        if (mousePressed && mousePos.x >= sliderX && mousePos.x <= sliderX + sliderWidth) {
            draggingSepSlider = true;
            sepWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
            sepWeight = std::max(0.0f, std::min(3.0f, sepWeight));
        }
    }

    // Check if mouse is near alignment slider
    if (mousePos.y >= sliderY + spacing - 15 && mousePos.y <= sliderY + spacing + sliderHeight + 15) {
        if (mousePressed && mousePos.x >= sliderX && mousePos.x <= sliderX + sliderWidth) {
            draggingAliSlider = true;
            aliWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
            aliWeight = std::max(0.0f, std::min(3.0f, aliWeight));
        }
    }

    // Check if mouse is near cohesion slider
    if (mousePos.y >= sliderY + spacing * 2 - 15 && mousePos.y <= sliderY + spacing * 2 + sliderHeight + 15) {
        if (mousePressed && mousePos.x >= sliderX && mousePos.x <= sliderX + sliderWidth) {
            draggingCohSlider = true;
            cohWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
            cohWeight = std::max(0.0f, std::min(3.0f, cohWeight));
        }
    }

    // Continue dragging if already dragging
    if (draggingSepSlider && mousePressed) {
        sepWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
        sepWeight = std::max(0.0f, std::min(3.0f, sepWeight));
    }
    if (draggingAliSlider && mousePressed) {
        aliWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
        aliWeight = std::max(0.0f, std::min(3.0f, aliWeight));
    }
    if (draggingCohSlider && mousePressed) {
        cohWeight = ((float)(mousePos.x - sliderX) / sliderWidth) * 3.0f;
        cohWeight = std::max(0.0f, std::min(3.0f, cohWeight));
    }
}
