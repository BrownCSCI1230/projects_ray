#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "raytracer/interface/Light.h"

class PointLight : public Light
{
public:
    PointLight(glm::vec4 color, glm::vec4 pos, glm::vec3 attenuation);

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
    glm::vec4 m_pos;
    glm::vec3 m_attenuation;
};

#endif // POINTLIGHT_H
