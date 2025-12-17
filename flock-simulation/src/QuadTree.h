#pragma once
#include <vector>
#include <memory>
#include <algorithm>

struct QTPoint {
    float x;
    float y;
    int index; // index into boids array
};

struct QTRect {
    float x; // left
    float y; // top
    float w; // width
    float h; // height

    bool contains(float px, float py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }

    bool intersectsCircle(float cx, float cy, float r) const {
        float closestX = std::clamp(cx, x, x + w);
        float closestY = std::clamp(cy, y, y + h);
        float dx = cx - closestX;
        float dy = cy - closestY;
        return (dx * dx + dy * dy) <= r * r;
    }
};

class QuadTree {
public:
    QuadTree(const QTRect& boundary, int capacity = 8)
        : boundary(boundary), capacity(capacity), divided(false) {}

    void clear() {
        points.clear();
        if (divided) {
            for (auto& c : children) {
                if (c) c->clear();
            }
            children[0].reset();
            children[1].reset();
            children[2].reset();
            children[3].reset();
            divided = false;
        }
    }

    bool insert(float x, float y, int index) {
        if (!boundary.contains(x, y)) return false;
        if ((int)points.size() < capacity) {
            points.push_back({x, y, index});
            return true;
        }
        if (!divided) subdivide();
        for (auto& c : children) {
            if (c->insert(x, y, index)) return true;
        }
        return false; // Shouldn't happen
    }

    void queryCircle(float cx, float cy, float r, std::vector<int>& out) const {
        if (!boundary.intersectsCircle(cx, cy, r)) return;
        for (const auto& p : points) {
            float dx = p.x - cx;
            float dy = p.y - cy;
            if (dx * dx + dy * dy <= r * r) out.push_back(p.index);
        }
        if (divided) {
            for (const auto& c : children) c->queryCircle(cx, cy, r, out);
        }
    }

private:
    void subdivide() {
        float hw = boundary.w * 0.5f;
        float hh = boundary.h * 0.5f;
        QTRect nw{boundary.x, boundary.y, hw, hh};
        QTRect ne{boundary.x + hw, boundary.y, hw, hh};
        QTRect sw{boundary.x, boundary.y + hh, hw, hh};
        QTRect se{boundary.x + hw, boundary.y + hh, hw, hh};
        children[0] = std::make_unique<QuadTree>(nw, capacity);
        children[1] = std::make_unique<QuadTree>(ne, capacity);
        children[2] = std::make_unique<QuadTree>(sw, capacity);
        children[3] = std::make_unique<QuadTree>(se, capacity);
        divided = true;
    }

    QTRect boundary;
    int capacity;
    bool divided;
    std::vector<QTPoint> points;
    std::unique_ptr<QuadTree> children[4];
};
