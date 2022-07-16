#ifndef RAYCAMERA_H
#define RAYCAMERA_H

#include "camera.h"

/**
 * @class RayCamera
 *
 * The perspective camera to be implemented in the Camtrans lab.
 */

class RayCamera : public Camera {
public:
    // Initialize your camera.
    RayCamera();

    // Returns the view matrix given the current camera settings.
    virtual glm::mat4x4 getViewMatrix() const override;

    // Returns the transformation matrix for the current camera settings.
    virtual glm::mat4x4 getTransformationMatrix() const override;

    // Returns the aspect ratio of the camera.
    virtual float getAspectRatio() const override;

    // Returns the height angle in degree of the camera.
    virtual float getHeightAngle() const override;

    // Sets the aspect ratio of this camera.
    virtual void setAspectRatio(float aspectRatio) override;

    // Sets the height angle of this camera.
    virtual void setHeightAngle(float h) override;

    // Move this camera to a new eye position,
    // and orient the camera's axes given look and up vectors.
    void orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up);

private:
    float m_aspectRatio;
    float m_thetaH;
    float m_thetaW;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_transformMatrix;

    glm::vec4 m_eye, m_look, m_up;
    glm::vec4 m_u, m_v, m_w;
};

#endif // RAYCAMERA_H
