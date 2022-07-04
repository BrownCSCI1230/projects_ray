#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    virtual ~Light() {}

    // Returns the light intensity at the given point.
    // @param pos The position in WORLD SPACE.
    virtual glm::vec4 getIntensity(glm::vec4 pos) const = 0;

    // Returns the direction from the given point to the light source.
    // @param pos The position in WORLD SPACE.
    virtual glm::vec4 getDirection(glm::vec4 pos) const = 0;

    // Returns the distance from the given point to the light source.
    // @param pos The position in WORLD SPACE.
    virtual float getDistance(glm::vec4 pos) const = 0;
};

#endif // LIGHT_H
