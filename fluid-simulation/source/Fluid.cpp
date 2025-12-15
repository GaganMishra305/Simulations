#include <algorithm>
#include <cmath>
#include "Fluid.h"

Fluid::Fluid(int gridW, int gridH, int cellSize): Nw(gridW), Nh(gridH), cellSize(cellSize) {
    int size = Nw * Nh;
    density.assign(size, 0.0f);
    densityPrev.assign(size, 0.0f);
    u.assign(size, 0.0f);
    v.assign(size, 0.0f);
    uPrev.assign(size, 0.0f);
    vPrev.assign(size, 0.0f);
}

void Fluid::addSource(std::vector<float>& x, const std::vector<float>& s, float dt) {
    for (size_t i = 0; i < x.size(); ++i) x[i] += dt * s[i];
}

void Fluid::setBnd(int b, std::vector<float>& x) {
    // simple reflective boundaries
    for (int i = 0; i < Nw; ++i) {
        x[IX(i, 0)] = (b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)]);
        x[IX(i, Nh - 1)] = (b == 2 ? -x[IX(i, Nh - 2)] : x[IX(i, Nh - 2)]);
    }
    for (int j = 0; j < Nh; ++j) {
        x[IX(0, j)] = (b == 1 ? -x[IX(1, j)] : x[IX(1, j)]);
        x[IX(Nw - 1, j)] = (b == 1 ? -x[IX(Nw - 2, j)] : x[IX(Nw - 2, j)]);
    }
    x[IX(0,0)] = 0.5f*(x[IX(1,0)] + x[IX(0,1)]);
    x[IX(0,Nh-1)] = 0.5f*(x[IX(1,Nh-1)] + x[IX(0,Nh-2)]);
    x[IX(Nw-1,0)] = 0.5f*(x[IX(Nw-2,0)] + x[IX(Nw-1,1)]);
    x[IX(Nw-1,Nh-1)] = 0.5f*(x[IX(Nw-2,Nh-1)] + x[IX(Nw-1,Nh-2)]);
}

void Fluid::diffuse(int b, std::vector<float>& x, const std::vector<float>& x0, float diff, float dt) {
    float a = dt * diff * (Nw - 2) * (Nh - 2);
    for (int k = 0; k < iterations; ++k) {
        for (int j = 1; j < Nh - 1; ++j) {
            for (int i = 1; i < Nw - 1; ++i) {
                x[IX(i,j)] = (x0[IX(i,j)] + a * (
                    x[IX(i-1,j)] + x[IX(i+1,j)] + x[IX(i,j-1)] + x[IX(i,j+1)]
                )) / (1 + 4*a);
            }
        }
        setBnd(b, x);
    }
}

void Fluid::advect(int b, std::vector<float>& d, const std::vector<float>& d0, const std::vector<float>& uu, const std::vector<float>& vv, float dt) {
    for (int j = 1; j < Nh - 1; ++j) {
        for (int i = 1; i < Nw - 1; ++i) {
            float x = i - dt * uu[IX(i,j)];
            float y = j - dt * vv[IX(i,j)];
            x = std::clamp(x, 0.5f, (float)Nw - 1.5f);
            y = std::clamp(y, 0.5f, (float)Nh - 1.5f);
            int i0 = (int)std::floor(x);
            int j0 = (int)std::floor(y);
            int i1 = i0 + 1;
            int j1 = j0 + 1;
            float s1 = x - i0; float s0 = 1.0f - s1;
            float t1 = y - j0; float t0 = 1.0f - t1;
            d[IX(i,j)] = 
                s0*(t0*d0[IX(i0,j0)] + t1*d0[IX(i0,j1)]) +
                s1*(t0*d0[IX(i1,j0)] + t1*d0[IX(i1,j1)]);
        }
    }
    setBnd(b, d);
}

void Fluid::project(std::vector<float>& uu, std::vector<float>& vv, std::vector<float>& p, std::vector<float>& div) {
    for (int j = 1; j < Nh - 1; ++j) {
        for (int i = 1; i < Nw - 1; ++i) {
            div[IX(i,j)] = -0.5f*(
                uu[IX(i+1,j)] - uu[IX(i-1,j)] +
                vv[IX(i,j+1)] - vv[IX(i,j-1)]
            ) / ((Nw + Nh) * 0.5f);
            p[IX(i,j)] = 0.0f;
        }
    }
    setBnd(0, div); setBnd(0, p);
    for (int k = 0; k < iterations; ++k) {
        for (int j = 1; j < Nh - 1; ++j) {
            for (int i = 1; i < Nw - 1; ++i) {
                p[IX(i,j)] = (div[IX(i,j)] + p[IX(i-1,j)] + p[IX(i+1,j)] + p[IX(i,j-1)] + p[IX(i,j+1)]) / 4.0f;
            }
        }
        setBnd(0, p);
    }
    for (int j = 1; j < Nh - 1; ++j) {
        for (int i = 1; i < Nw - 1; ++i) {
            uu[IX(i,j)] -= 0.5f*(p[IX(i+1,j)] - p[IX(i-1,j)]);
            vv[IX(i,j)] -= 0.5f*(p[IX(i,j+1)] - p[IX(i,j-1)]);
        }
    }
    setBnd(1, uu); setBnd(2, vv);
}

void Fluid::Update(Renderer* renderer, Mouse* mouse, float dt) {
    // inject forces/density from mouse
    int mx = (int)mouse->GetPosition().x / cellSize;
    int my = (int)mouse->GetPosition().y / cellSize;
    int rad = std::max(1, (int)(mouse->GetCursorSize() / cellSize));

    std::fill(uPrev.begin(), uPrev.end(), 0.0f);
    std::fill(vPrev.begin(), vPrev.end(), 0.0f);
    std::fill(densityPrev.begin(), densityPrev.end(), 0.0f);

    if (mx >= 1 && mx < Nw-1 && my >= 1 && my < Nh-1) {
        for (int j = std::max(1, my - rad); j <= std::min(Nh - 2, my + rad); ++j) {
            for (int i = std::max(1, mx - rad); i <= std::min(Nw - 2, mx + rad); ++i) {
                float falloff = 1.0f - std::hypotf((float)(i - mx), (float)(j - my)) / (float)rad;
                if (falloff < 0) falloff = 0;
                if (mouse->GetLeftButtonDown()) {
                    densityPrev[IX(i,j)] += 200.0f * falloff; // add ink
                }
                if (mouse->GetRightMouseButton()) {
                    Vec2 d = mouse->GetPosition() - mouse->GetPreviousPosition();
                    uPrev[IX(i,j)] += d.x * 5.0f * falloff;
                    vPrev[IX(i,j)] += d.y * 5.0f * falloff;
                }
            }
        }
    }

    // velocity step
    addSource(u, uPrev, dt); addSource(v, vPrev, dt);
    diffuse(1, u, u, viscosity, dt);
    diffuse(2, v, v, viscosity, dt);
    {
        std::vector<float> p(Nw*Nh, 0.0f), div(Nw*Nh, 0.0f);
        project(u, v, p, div);
    }
    {
        std::vector<float> u0 = u, v0 = v;
        advect(1, u, u0, u0, v0, dt);
        advect(2, v, v0, u0, v0, dt);
    }
    {
        std::vector<float> p(Nw*Nh, 0.0f), div(Nw*Nh, 0.0f);
        project(u, v, p, div);
    }

    // density step
    addSource(density, densityPrev, dt);
    diffuse(0, density, density, diffusion, dt);
    {
        std::vector<float> d0 = density;
        advect(0, density, d0, u, v, dt);
    }

    // damp slightly
    for (auto &d : density) d = std::max(0.0f, d * 0.995f);
}

void Fluid::Draw(Renderer* renderer) const {
    // Render density as grayscale squares
    for (int j = 1; j < Nh - 1; ++j) {
        for (int i = 1; i < Nw - 1; ++i) {
            float d = std::clamp(density[IX(i,j)], 0.0f, 255.0f);
            Uint32 col = (Uint32)d;
            Uint32 color = 0xFF000000 | (col << 16) | (col << 8) | col;
            renderer->FillRect(i * cellSize, j * cellSize, cellSize, cellSize, color);
        }
    }
}
