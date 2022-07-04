#ifndef CAMTRANSCAMERA_H
#define CAMTRANSCAMERA_H

#include "camera.h"

/**
 * @class CamtransCamera
 *.2
 * The perspective camera to be implemented in the Camtrans lab.
 */

class CamtransCamera : public Camera {
public:
    // Initialize your camera.
    CamtransCamera();

    // Returns the projection matrix given the current camera settings.
    virtual glm::mat4x4 getProjectionMatrix() const override;

    // Returns the view matrix given the current camera settings.
    virtual glm::mat4x4 getViewMatrix() const override;

    // Returns the transformation matrix for the current camera settings.
    virtual glm::mat4x4 getTransformationMatrix() const override;

    // Returns the matrix that scales down the perspective view volume into the canonical
    // perspective view volume, given the current camera settings.
    virtual glm::mat4x4 getScaleMatrix() const override;

    // Returns the aspect ratio of the camera.
    virtual float getAspectRatio() const override;

    // Returns the height angle in degree of the camera.
    virtual float getHeightAngle() const override;

    // Returns the near plane of the camera.
    virtual float getNear() const override;

    // Returns the far plane of the camera.
    virtual float getFar() const override;

    // Sets the aspect ratio of this camera. Automatically called by the GUI when the window is
    // resized.
    virtual void setAspectRatio(float aspectRatio) override;

    // Sets the height angle of this camera.
    virtual void setHeightAngle(float h) override;

    // Returns the current position of the camera.
    glm::vec4 getPosition() const;

    // Returns the current 'look' vector for this camera.
    glm::vec4 getLook() const;

    // Returns the current 'up' vector for this camera (the 'V' vector).
    glm::vec4 getUp() const;

    // Move this camera to a new eye position, and orient the camera's axes given look and up
    // vectors.
    void orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up);

    // Translates the camera along a given vector.
    void translate(const glm::vec4 &v);

    // Rotates the camera about the U axis by a specified number of degrees.
    void rotateU(float degrees);

    // Rotates the camera about the V axis by a specified number of degrees.
    void rotateV(float degrees);

    // Rotates the camera about the W axis by a specified number of degrees.
    void rotateW(float degrees);

    // Sets the near and far clip planes for this camera.
    void setClip(float nearPlane, float farPlane);

    void updateProjectionMatrix();

    void updateViewMatrix();

    void rotate(glm::mat4 &rotationMatrix);

private:
    float m_aspectRatio;
    float m_near;
    float m_far;

    float m_thetaH;
    float m_thetaW;

    glm::mat4 m_scaleMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_transformMatrix;

    glm:: mat4 m_rotationMatrix, m_translationMatrix;

    glm::vec4 m_eye, m_look, m_up;
    glm::vec4 m_u, m_v, m_w;
};

#endif // CAMTRANSCAMERA_H
