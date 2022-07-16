/**
 * @file   FixedCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "camera/RayCamera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

RayCamera::RayCamera()
{
    m_thetaH = glm::radians(60.f);
    m_aspectRatio = 1.f;
    m_viewMatrix = glm::mat4(1);
    m_transformMatrix = glm::mat4(1);
}

glm::mat4x4 RayCamera::getViewMatrix() const {
    return m_viewMatrix;
}

glm::mat4x4 RayCamera::getTransformationMatrix() const {
    return m_transformMatrix;
}

float RayCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float RayCamera::getHeightAngle() const {
    return glm::degrees(m_thetaH);
}

void RayCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    return;
}

void RayCamera::setHeightAngle(float h) {
    m_thetaH = glm::radians(h);
    return;
}

void RayCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_look = glm::normalize(look);
    m_up = glm::normalize(up);

    m_w = -m_look;
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w) * m_w);
    m_u = vec4(glm::cross(vec3(m_v), vec3(m_w)), 0.f);

    glm::mat4 rotationMatrix = glm::transpose(mat4(m_u, m_v, m_w, vec4(0.f, 0.f, 0.f, 1.f)));

    glm::mat4 translationMatrix = mat4(1.f);
    translationMatrix[3] = vec4(-eye.x, -eye.y, -eye.z, 1.f);

    m_viewMatrix = rotationMatrix * translationMatrix;
    m_transformMatrix = glm::inverse(m_viewMatrix);

    return;
}
