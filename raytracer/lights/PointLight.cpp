#include "pointlight.h"

PointLight::PointLight(glm::vec4 color, glm::vec4 pos, glm::vec3 attenuation)
    : m_color(color),
      m_pos(pos),
      m_attenuation(attenuation)
{

}

glm::vec4 PointLight::getIntensity(glm::vec4 pos) const {
    float dist = getDistance(pos);
    float atten = 1.f / (m_attenuation.x + dist * m_attenuation.y + dist * dist * m_attenuation.z);
    atten = glm::min(1.f, atten);
    return m_color * atten;
}

glm::vec4 PointLight::getDirection(glm::vec4 pos) const {
    return glm::normalize(m_pos - pos);
}

float PointLight::getDistance(glm::vec4 pos) const {
    return glm::length(m_pos - pos);
}
