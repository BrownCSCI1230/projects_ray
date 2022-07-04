/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "camtranscamera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

CamtransCamera::CamtransCamera()
{
    m_near = 1.f;
    m_far = 30.f;

    m_thetaH = glm::radians(60.f);
    m_aspectRatio = 1.f;

    // configure uvw with eye, look, up
    orientLook(vec4(2.f, 2.f, 2.f, 1.f), vec4(-1.f, -1.f, -1.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f));

    updateProjectionMatrix();
    updateViewMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    return m_projectionMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    return m_viewMatrix;
}

glm::mat4x4 CamtransCamera::getTransformationMatrix() const {
    return m_transformMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

float CamtransCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    return glm::degrees(m_thetaH);
}

float CamtransCamera::getNear() const {
    return m_near;
}

float CamtransCamera::getFar() const {
    return m_far;
}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    updateProjectionMatrix();
    return;
}

void CamtransCamera::setHeightAngle(float h) {
    m_thetaH = glm::radians(h);
    updateProjectionMatrix();
    return;
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    return m_look;
}

glm::vec4 CamtransCamera::getUp() const {
    return m_up;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_look = glm::normalize(look);
    m_up = glm::normalize(up);

    m_w = -m_look;
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w) * m_w);
    m_u = vec4(glm::cross(vec3(m_v), vec3(m_w)), 0.f);

    m_rotationMatrix = glm::transpose(mat4(m_u, m_v, m_w, vec4(0.f, 0.f, 0.f, 1.f)));

    m_translationMatrix = mat4(1.f);
    m_translationMatrix[3] = vec4(-eye.x, -eye.y, -eye.z, 1.f);

    updateViewMatrix();
    return;
}

void CamtransCamera::translate(const glm::vec4 &v) {
    m_eye += v;
    m_translationMatrix[3] -= v;
    updateViewMatrix();
    return;
}

void CamtransCamera::rotateU(float degrees) {
    float rad = -glm::radians(degrees);
    float cosine = glm::cos(rad);
    float sine = glm::sin(rad);

    mat4 rotateMatrix = mat4(1.f);
    rotateMatrix[1][1] = cosine;
    rotateMatrix[1][2] = -sine;

    rotateMatrix[2][1] = sine;
    rotateMatrix[2][2] = cosine;

    rotate(rotateMatrix);
}

void CamtransCamera::rotateV(float degrees) {
    float rad = -glm::radians(degrees);
    float cosine = glm::cos(rad);
    float sine = glm::sin(rad);

    mat4 rotateMatrix = mat4(1.f);
    rotateMatrix[0][0] = cosine;
    rotateMatrix[0][2] = sine;

    rotateMatrix[2][0] = -sine;
    rotateMatrix[2][2] = cosine;

    rotate(rotateMatrix);
}

void CamtransCamera::rotateW(float degrees) {
    float rad = -glm::radians(degrees);
    float cosine = glm::cos(rad);
    float sine = glm::sin(rad);

    mat4 rotateMatrix = mat4(1.f);
    rotateMatrix[0][0] = cosine;
    rotateMatrix[0][1] = -sine;

    rotateMatrix[1][0] = sine;
    rotateMatrix[1][1] = cosine;

    rotate(rotateMatrix);
}

void CamtransCamera::rotate(mat4 &rotationMatrix) {
    mat4 inverseView = glm::inverse(m_viewMatrix);

    vec4 look = inverseView * rotationMatrix * m_viewMatrix * m_look;
    vec4 up = inverseView * rotationMatrix * m_viewMatrix * m_up;

    orientLook(m_eye, look, up);
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    // Make sure glm gets a far value that is greater than the near value.
    m_near = nearPlane;
    m_far = glm::max(farPlane, nearPlane + 100.f * FLT_EPSILON);
    updateProjectionMatrix();
    return;
}

void CamtransCamera::updateProjectionMatrix() {
//    float h = m_far * glm::tan(m_thetaH * 0.5f);
//    float w = m_aspectRatio * h;

//    float c = -m_near / m_far;

//    mat4 perspectiveMatrix = mat4(1.f);
//    perspectiveMatrix[2][2] = 1.f / (1 + c);
//    perspectiveMatrix[2][3] = -1;

//    perspectiveMatrix[3][2] = -c / (1 + c);
//    perspectiveMatrix[3][3] = 0.f;

//    m_scaleMatrix = mat4(
//                vec4(1.f / w, 0.f, 0.f, 0.f),
//                vec4(0.f, 1.f / h, 0.f, 0.f),
//                vec4(0.f, 0.f, 1.f / m_far, 0.f),
//                vec4(0.f, 0.f, 0.f, 1.f));
//    m_projectionMatrix = perspectiveMatrix * m_scaleMatrix;

    m_projectionMatrix = glm::perspective(m_thetaH, m_aspectRatio, m_near, m_far);

    return;
}

void CamtransCamera::updateViewMatrix() {
    m_viewMatrix = m_rotationMatrix * m_translationMatrix;
    m_transformMatrix = glm::inverse(m_viewMatrix);
    return;
}
