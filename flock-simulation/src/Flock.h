#include <iostream>
#include <vector>
#include "Boid.h"

#ifndef FLOCK_H_
#define FLOCK_H_


// Brief description of Flock Class:
// This file contains the class needed to create a flock of boids. It utilizes
// the boids class and initializes boid flocks with parameters that can be
// specified. This class will be utilized in main.

class Flock {
public:
    //Constructors
    float radius;
    Flock(float r) : radius(r) {}
    // Accessor functions
    int getSize();
    // Read/write methods.
    Boid &getBoid(int i);
    void addBoid(const Boid& b);
    void flocking();
    void setBounds(int w, int h) { width = w; height = h; }
    void setUseTree(bool v) { useTree = v; }
    void setWeights(float sep, float ali, float coh) { 
        sepWeight = sep; aliWeight = ali; cohWeight = coh; 
    }
    float getSepWeight() const { return sepWeight; }
    float getAliWeight() const { return aliWeight; }
    float getCohWeight() const { return cohWeight; }
private:
    vector<Boid> flock;
    bool useTree = false;
    int width = 1920;
    int height = 1080;
    float sepWeight = 1.0f;
    float aliWeight = 1.0f;
    float cohWeight = 1.0f;
};

#endif
