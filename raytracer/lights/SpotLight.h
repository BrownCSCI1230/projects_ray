#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "raytracer/interface/Light.h"

class SpotLight : public Light
{
public:
    SpotLight(glm::vec4 color, glm::vec4 pos, glm::vec4 dir, glm::vec3 attenuation, float angle, float penumbra);

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
    glm::vec4 m_direction;
    glm::vec3 m_attenuation;
    float m_innerAngle;
    float m_outerAngle;
    float m_penumbra;
};

#endif // SPOTLIGHT_H
