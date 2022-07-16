#include "directionallight.h"

DirectionalLight::DirectionalLight(glm::vec4 color, glm::vec4 direction)
    : m_color(color),
      m_direction(glm::normalize(direction))

{

}

glm::vec4 DirectionalLight::getIntensity(glm::vec4 pos) const {
    return m_color;
}

glm::vec4 DirectionalLight::getDirection(glm::vec4 pos) const {
    return -m_direction;
}

float DirectionalLight::getDistance(glm::vec4 pos) const {
    return FLT_MAX;
}
