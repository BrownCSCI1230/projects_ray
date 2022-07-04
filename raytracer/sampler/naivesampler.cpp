#include "naivesampler.h"
#include "camera/camera.h"
#include "raytracer/raytracer.h"
#include "raytracer/raytracescene.h"

#include <memory>

NaiveSampler::NaiveSampler()
{

}

glm::vec4 NaiveSampler::sample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    auto transformMat = camera->getTransformationMatrix();

    float y = (scene.height() - 1 - i + 0.5) / scene.height() - 0.5;
    float x = (j + 0.5) / scene.width() - 0.5;

    // we need to calculate the actual size of the view plane based on camera parameters
    float k = camera->getNear();
    float v = 2 * k * glm::tan(0.5 * camera->getHeightAngle() * M_PI / 180.f);
    float u = v * camera->getAspectRatio();

    glm::vec4 dir(u * x, v * y, -camera->getNear(), 0.f);
    glm::vec4 pos(0, 0, 0, 1);

    // transform from camera space to world space
    pos = transformMat * pos;
    dir = transformMat * dir;

    Ray r(pos, dir, FLT_MAX);
    return raytracer->traceRay(r, scene, RAY_TRACE_MAX_DEPTH);
}
