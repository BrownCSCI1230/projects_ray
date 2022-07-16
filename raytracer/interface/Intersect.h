#ifndef INTERSECT_H
#define INTERSECT_H

#include <glm/glm.hpp>

class BaseRTShape;

struct Ray {
    glm::vec4 origin;
    glm::vec4 direction;
    mutable float tMax;

    Ray(glm::vec4 origin_, glm::vec4 direction_, float tMax_) :
        origin(origin_), direction(glm::normalize(direction_)), tMax(tMax_) {}
};

struct SurfaceInteraction {
    float t;
    glm::vec4 normal;
    glm::vec2 uv;
    BaseRTShape *primitive;

    SurfaceInteraction(float t_) : t(t_), primitive(nullptr) {}

    SurfaceInteraction(float t_, glm::vec4 normal_, glm::vec2 uv_) : t(t_), normal(normal_), uv(uv_), primitive(nullptr) {}
};

// An abstract class that performs the intersection check with all the primitives it has.
class Intersect {

public:
    Intersect() {};

    // Check if the ray intersects with any primitive.
    // @param ray The Ray object describing the ray in the WORLD SPACE.
    // @param oSurInteraction Upon return, contains the intersection information if there is one.
    // @return A boolean value indicating if there is an intersection with a primitive.
    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const = 0;
};

#endif // INTERSECT_H
