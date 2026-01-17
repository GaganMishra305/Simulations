#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "SFML/Graphics.hpp"
#include "Boid.h"

// Global Variables for borders()
// desktopTemp gets screen resolution of PC running the program
sf::VideoMode desktopTemp = sf::VideoMode::getDesktopMode();
const int window_height = desktopTemp.height;
const int window_width = desktopTemp.width;

#define w_height window_height
#define w_width window_width
#define PI 3.141592635


// =============================================== //
// ======== Boid Functions from Boid.h =========== //
// =============================================== //

Boid::Boid(float x, float y)
{
    acceleration = Pvector(0, 0);
    velocity = Pvector(rand()%3 - 2, rand()%3 - 2);
    location = Pvector(x, y);
    maxSpeed = 7.5;
    maxForce = 0.75;
}

Boid::Boid(float x, float y, bool predCheck)
{
    predator = predCheck;
    if (predCheck == true) {
        maxSpeed = 7.5;
        maxForce = 0.75;
        velocity = Pvector(rand()%3 - 1, rand()%3 - 1);
    } else {
        maxSpeed = 7.5;
        maxForce = 0.75;
        velocity = Pvector(rand()%3 - 2, rand()%3 - 2);
    }
    acceleration = Pvector(0, 0);
    location = Pvector(x, y);
}

// Adds force Pvector to current force Pvector
void Boid::applyForce(const Pvector& force)
{
    acceleration.addVector(force);
}

// Separation
// Keeps boids from getting too close to one another
Pvector Boid::Separation(const vector<Boid>& boids, float radius)
{
    // Distance of field of vision for separation between boids
    float desiredseparation = radius;
    Pvector steer(0, 0);
    int count = 0;
    // For every boid in the system, check if it's too close
    for (int i = 0; i < boids.size(); i++) {
        // Calculate distance from current boid to boid we're looking at
        float d = location.distance(boids[i].location);
        // If this is a fellow boid and it's too close, move away from it
        if ((d > 0) && (d < desiredseparation)) {
            Pvector diff(0,0);
            diff = diff.subTwoVector(location, boids[i].location);
            diff.normalize();
            diff.divScalar(d);      // Weight by distance
            steer.addVector(diff);
            count++;
        }
        // If current boid is a predator and the boid we're looking at is also
        // a predator, then separate only slightly
        if ((d > 0) && (d < desiredseparation) && predator == true
            && boids[i].predator == true) {
            Pvector pred2pred(0, 0);
            pred2pred = pred2pred.subTwoVector(location, boids[i].location);
            pred2pred.normalize();
            pred2pred.divScalar(d);
            steer.addVector(pred2pred);
            count++;
        }
        // If current boid is not a predator, but the boid we're looking at is
        // a predator, then create a large separation Pvector
        else if ((d > 0) && (d < desiredseparation+70) && boids[i].predator == true) {
            Pvector pred(0, 0);
            pred = pred.subTwoVector(location, boids[i].location);
            pred.mulScalar(900);
            steer.addVector(pred);
            count++;
        }
    }
    // Adds average difference of location to acceleration
    if (count > 0)
        steer.divScalar((float)count);
    if (steer.magnitude() > 0) {
        // Steering = Desired - Velocity
        steer.normalize();
        steer.mulScalar(maxSpeed);
        steer.subVector(velocity);
        steer.limit(maxForce);
    }
    return steer;
}

// Alignment
// Calculates the average velocity of boids in the field of vision and
// manipulates the velocity of the current boid in order to match it
Pvector Boid::Alignment(const vector<Boid>& Boids, float radius)
{
    float neighbordist = radius; // Field of vision

    Pvector sum(0, 0);
    int count = 0;
    for (int i = 0; i < Boids.size(); i++) {
        float d = location.distance(Boids[i].location);
        if ((d > 0) && (d < neighbordist)) { // 0 < d < 50
            sum.addVector(Boids[i].velocity);
            count++;
        }
    }
    // If there are boids close enough for alignment...
    if (count > 0) {
        sum.divScalar((float)count);// Divide sum by the number of close boids (average of velocity)
        sum.normalize();            // Turn sum into a unit vector, and
        sum.mulScalar(maxSpeed);    // Multiply by maxSpeed
        // Steer = Desired - Velocity
        Pvector steer;
        steer = steer.subTwoVector(sum, velocity); //sum = desired(average)
        steer.limit(maxForce);
        return steer;
    } else {
        Pvector temp(0, 0);
        return temp;
    }
}

// Cohesion
// Finds the average location of nearby boids and manipulates the
// steering force to move in that direction.
Pvector Boid::Cohesion(const vector<Boid>& Boids, float radius)
{
    float neighbordist = radius;
    Pvector sum(0, 0);
    int count = 0;
    for (int i = 0; i < Boids.size(); i++) {
        float d = location.distance(Boids[i].location);
        if ((d > 0) && (d < neighbordist)) {
            sum.addVector(Boids[i].location);
            count++;
        }
    }
    if (count > 0) {
        sum.divScalar(count);
        return seek(sum);
    } else {
        Pvector temp(0,0);
        return temp;
    }
}

// Limits the maxSpeed, finds necessary steering force and
// normalizes vectors
Pvector Boid::seek(const Pvector& v)
{
    Pvector desired;
    desired.subVector(v);  // A vector pointing from the location to the target
    // Normalize desired and scale to maximum speed
    desired.normalize();
    desired.mulScalar(maxSpeed);
    // Steering = Desired minus Velocity
    acceleration.subTwoVector(desired, velocity);
    acceleration.limit(maxForce);  // Limit to maximum steering force
    return acceleration;
}

// Modifies velocity, location, and resets acceleration with values that
// are given by the three laws.
void Boid::update()
{
    //To make the slow down not as abrupt
    acceleration.mulScalar(.4);
    // Update velocity
    velocity.addVector(acceleration);
    // Limit speed
    velocity.limit(maxSpeed);
    location.addVector(velocity);
    // Reset accelertion to 0 each cycle
    acceleration.mulScalar(0);
}

// Run flock() on the flock of boids.
// This applies the three rules, modifies velocities accordingly, updates data,
// and corrects boids which are sitting outside of the SFML window
void Boid::run(const vector <Boid>& v, float radius, float sepW, float aliW, float cohW)
{
    flock(v, radius, sepW, aliW, cohW);
    update();
    borders();
}

void Boid::runNeighbors(const std::vector<const Boid*>& neighbors, float radius, float sepW, float aliW, float cohW)
{
    flockNeighbors(neighbors, radius, sepW, aliW, cohW);
    update();
    borders();
}

// Applies the three laws to the flock of boids
void Boid::flock(const vector<Boid>& v, float radius, float sepW, float aliW, float cohW)
{
    Pvector sep = Separation(v, radius);
    Pvector ali = Alignment(v, radius);
    Pvector coh = Cohesion(v, radius);
    // Weight these forces with dynamic values
    sep.mulScalar(sepW);
    ali.mulScalar(aliW);
    coh.mulScalar(cohW);
    // Add the force vectors to acceleration
    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
}

void Boid::flockNeighbors(const std::vector<const Boid*>& neighbors, float radius, float sepW, float aliW, float cohW)
{
    // Compute forces using only neighbors
    float desiredSep = radius;
    float neighbordist = radius;

    // Separation
    Pvector sep(0,0);
    int sepCount = 0;
    for (auto* b : neighbors) {
        float d = location.distance(b->location);
        if ((d > 0) && (d < desiredSep)) {
            Pvector diff(0,0);
            diff = diff.subTwoVector(location, b->location);
            diff.normalize();
            diff.divScalar(d);
            sep.addVector(diff);
            sepCount++;
        }
        if ((d > 0) && (d < desiredSep) && predator && b->predator) {
            Pvector pred2pred(0, 0);
            pred2pred = pred2pred.subTwoVector(location, b->location);
            pred2pred.normalize();
            pred2pred.divScalar(d);
            sep.addVector(pred2pred);
            sepCount++;
        } else if ((d > 0) && (d < desiredSep+70) && b->predator) {
            Pvector pred(0, 0);
            pred = pred.subTwoVector(location, b->location);
            pred.mulScalar(900);
            sep.addVector(pred);
            sepCount++;
        }
    }
    if (sepCount > 0) sep.divScalar((float)sepCount);
    if (sep.magnitude() > 0) {
        sep.normalize();
        sep.mulScalar(maxSpeed);
        sep.subVector(velocity);
        sep.limit(maxForce);
    }

    // Alignment
    Pvector aliSum(0,0);
    int aliCount = 0;
    for (auto* b : neighbors) {
        float d = location.distance(b->location);
        if ((d > 0) && (d < neighbordist)) {
            aliSum.addVector(b->velocity);
            aliCount++;
        }
    }
    Pvector ali(0,0);
    if (aliCount > 0) {
        aliSum.divScalar((float)aliCount);
        aliSum.normalize();
        aliSum.mulScalar(maxSpeed);
        ali = ali.subTwoVector(aliSum, velocity);
        ali.limit(maxForce);
    }

    // Cohesion
    Pvector cohSum(0,0);
    int cohCount = 0;
    for (auto* b : neighbors) {
        float d = location.distance(b->location);
        if ((d > 0) && (d < neighbordist)) {
            cohSum.addVector(b->location);
            cohCount++;
        }
    }
    Pvector coh(0,0);
    if (cohCount > 0) {
        cohSum.divScalar((float)cohCount);
        coh = seek(cohSum);
    }

    // Weights
    sep.mulScalar(sepW);
    ali.mulScalar(aliW);
    coh.mulScalar(cohW);

    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
}

// Checks if boids go out of the window and if so, wraps them around to
// the other side.
void Boid::borders()
{
    if (location.x < 0)    location.x += w_width;
    if (location.y < 0)    location.y += w_height;
    if (location.x > w_width)  location.x -= w_width;
    if (location.y > w_height) location.y -= w_height;
}

// Calculates the angle for the velocity of a boid which allows the visual
// image to rotate in the direction that it is going in.
float Boid::angle(const Pvector& v)
{
    // From the definition of the dot product
    float angle = (float)(atan2(v.x, -v.y) * 180 / PI);
    return angle;
}
