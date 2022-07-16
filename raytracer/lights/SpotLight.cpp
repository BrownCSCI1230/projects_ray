#include "SpotLight.h"

SpotLight::SpotLight(glm::vec4 color, glm::vec4 pos, glm::vec4 dir, glm::vec3 attenuation, float angle, float penumbra)
    : m_color(color),
      m_pos(pos),
      m_direction(glm::normalize(dir)),
      m_attenuation(attenuation)
{
    angle = glm::clamp(angle, 0.f, 90.f);
    penumbra = glm::clamp(penumbra, 0.f, angle);
    m_outerAngle = angle;
    m_innerAngle = angle - penumbra;
    m_penumbra = penumbra;
}

glm::vec4 SpotLight::getIntensity(glm::vec4 pos) const {
    glm::vec4 lightToPos = glm::normalize(pos - m_pos);
    float cosine = glm::dot(m_direction, lightToPos);
    float posAngle = glm::degrees(glm::acos(cosine));

    float dist = getDistance(pos);
    float atten = 1.f / (m_attenuation.x + dist * m_attenuation.y + dist * dist * m_attenuation.z);
    atten = glm::min(1.f, atten);

    if (posAngle > m_outerAngle) {
        return glm::vec4(0.f, 0.f, 0.f, 1.f);
    } else if (posAngle < m_innerAngle) {
        return m_color * atten;
    }

    float rel_pos = ((posAngle - m_innerAngle) / m_penumbra);
    float falloff = -2*std::pow(rel_pos, 3) + 3*std::pow(rel_pos, 2);
    float portion = 1 - falloff;
    return m_color * portion * atten;
}

glm::vec4 SpotLight::getDirection(glm::vec4 pos) const {
    return glm::normalize(m_pos - pos);
}

float SpotLight::getDistance(glm::vec4 pos) const {
    return glm::length(m_pos - pos);
}
