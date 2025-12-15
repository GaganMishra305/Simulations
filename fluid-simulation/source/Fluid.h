#pragma once

#include <vector>
#include "Renderer.h"
#include "Mouse.h"

class Fluid {
private:
    int Nw; // grid width (cells)
    int Nh; // grid height (cells)
    int cellSize; // pixels per cell

    std::vector<float> density;
    std::vector<float> densityPrev;

    std::vector<float> u, v; // velocity components
    std::vector<float> uPrev, vPrev;

    float diffusion = 0.0001f;
    float viscosity = 0.0001f;
    int iterations = 10;

    int IX(int i, int j) const { return i + j * Nw; }

    void addSource(std::vector<float>& x, const std::vector<float>& s, float dt);
    void diffuse(int b, std::vector<float>& x, const std::vector<float>& x0, float diff, float dt);
    void advect(int b, std::vector<float>& d, const std::vector<float>& d0, const std::vector<float>& uu, const std::vector<float>& vv, float dt);
    void project(std::vector<float>& uu, std::vector<float>& vv, std::vector<float>& p, std::vector<float>& div);
    void setBnd(int b, std::vector<float>& x);

public:
    Fluid(int gridW, int gridH, int cellSize);
    ~Fluid() = default;

    void Update(Renderer* renderer, Mouse* mouse, float dt);
    void Draw(Renderer* renderer) const;
};
