#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "raytracer/interface/light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(glm::vec4 color, glm::vec4 direction);

    // Returns the light intensity at the given point.
    // @param pos The position in WORLD SPACE.
    virtual glm::vec4 getIntensity(glm::vec4 pos) const override;

    // Returns the direction from the given point to the light source.
    // @param pos The position in WORLD SPACE.
    virtual glm::vec4 getDirection(glm::vec4 pos) const override;

    // Returns the distance from the given point to the light source.
    // @param pos The position in WORLD SPACE.
    virtual float getDistance(glm::vec4 pos) const override;

private:
    glm::vec4 m_color;
    glm::vec4 m_direction;
};

#endif // DIRECTIONALLIGHT_H
