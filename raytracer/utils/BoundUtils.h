#ifndef BOUND_H
#define BOUND_H

#include <glm/glm.hpp>
#include "raytracer/interface/Intersect.h"

enum class EdgeType {
    START,
    END
};

struct BoundEdge {
    float t;
    int primNum;
    EdgeType type;

    BoundEdge()
        : t(0), primNum(0), type(EdgeType::START) {}

    BoundEdge(float t, int primNum, bool isStart)
        : t(t), primNum(primNum)
    {
        type = isStart ? EdgeType::START : EdgeType::END;
    }
};

struct Bound {
    glm::vec4 pMax;
    glm::vec4 pMin;

    Bound() : pMax(glm::vec4(0.f, 0.f, 0.f, 1.f)), pMin(glm::vec4(0.f, 0.f, 0.f, 1.f)) {}

    float surfaceArea() {
        glm::vec4 diagnol = pMax - pMin;
        return 2 * (diagnol.x * diagnol.y + diagnol.x * diagnol.z + diagnol.y * diagnol.z);
    }

    // Union with another boundary box.
    void unite(Bound & other) {
        pMin = glm::min(pMin, other.pMin);
        pMax = glm::max(pMax, other.pMax);
    }

    // Union with a point.
    void unite(const glm::vec4 &p) {
        pMin = glm::min(pMin, p);
        pMax = glm::max(pMax, p);
    }

    // Returns the axis with maximum extent.
    int maxExtent() const {
        glm::vec4 d = pMax - pMin;
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    bool intersect(const Ray &ray, float &oHit0, float &oHit1) const {
        float t0 = 0, t1 = ray.tMax;
        for (int i = 0; i < 3; i++) {
            // Update interval for _i_th bounding box slab
//            if (ray.direction[i] == 0) continue;
            float invRayDir = 1 / ray.direction[i];
            float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
            float tFar = (pMax[i] - ray.origin[i]) * invRayDir;

            // If the ray enters the ray box from axisMax rather than axisMin. Make a swap.
            if (tNear > tFar) std::swap(tNear, tFar);

            // Update _tFar_ to ensure robust ray--bounds intersection
//            tFar *= 1 + 2 * gamma(3);
            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar < t1 ? tFar : t1;
            if (t0 > t1) return false;
        }
        oHit0 = t0;
        oHit1 = t1;
        return true;
    }
};

#endif // BOUND_H
