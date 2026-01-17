#include "Boid.h"
#include "Flock.h"
#include "QuadTree.h"

// =============================================== //
// ======== Flock Functions from Flock.h ========= //
// =============================================== //

int Flock::getSize()
{
    return flock.size();
}

//Read/write method that returns a reference of the Boid.
Boid &Flock::getBoid(int i)
{
    return flock[i];
}

void Flock::addBoid(const Boid& b)
{
    flock.push_back(std::move(b));
}

// Runs the run function for every boid in the flock checking against the flock
// itself. Which in turn applies all the rules to the flock.
void Flock::flocking()
{
    if (!useTree) {
        for (int i = 0; i < (int)flock.size(); i++) {
            flock[i].run(flock, radius, sepWeight, aliWeight, cohWeight);
        }
        return;
    }

    // Build quadtree for current positions
    QTRect bounds{0.f, 0.f, (float)width, (float)height};
    QuadTree qt(bounds, 8);
    for (int i = 0; i < (int)flock.size(); ++i) {
        qt.insert(flock[i].location.x, flock[i].location.y, i);
    }

    // For each boid, query neighbors within radius and run using neighbors
    std::vector<int> neighborIdx;
    neighborIdx.reserve(32);
    std::vector<const Boid*> neighbors;
    neighbors.reserve(32);
    for (int i = 0; i < (int)flock.size(); ++i) {
        neighborIdx.clear();
        neighbors.clear();
        qt.queryCircle(flock[i].location.x, flock[i].location.y, radius, neighborIdx);
        for (int idx : neighborIdx) {
            if (idx == i) continue; // skip self
            neighbors.push_back(&flock[idx]);
        }
        flock[i].runNeighbors(neighbors, radius, sepWeight, aliWeight, cohWeight);
    }
}
