#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>

/**
 * @class Camera
 *
 * An abstract class representing a virtual camera with an optional mouse interaction.
 */

class Camera {
public:
    // C++ Note: The following are pure virtual functions - they must be
    // overridden in subclasses.

    // Returns the projection matrix for the current camera settings.
    virtual glm::mat4x4 getProjectionMatrix() const = 0;

    // Returns the view matrix for the current camera settings.
    virtual glm::mat4x4 getViewMatrix() const = 0;

    // Returns the transformation matrix for the current camera settings.
    virtual glm::mat4x4 getTransformationMatrix() const = 0;

    // Returns the matrix that scales down the perspective view volume into the canonical
    // perspective view volume, given the current camera settings.
    virtual glm::mat4x4 getScaleMatrix() const = 0;

    // Returns the aspect ratio of the camera.
    virtual float getAspectRatio() const = 0;

    // Returns the height angle in degree of the camera.
    virtual float getHeightAngle() const = 0;

    // Returns the near plane of the camera.
    virtual float getNear() const = 0;

    // Returns the far plane of the camera.
    virtual float getFar() const = 0;

    // Called when the window size changes
    virtual void setAspectRatio(float aspectRatio) = 0;

    // Sets the height angle of this camera.
    virtual void setHeightAngle(float h) = 0;
};

#endif // CAMERA_H
