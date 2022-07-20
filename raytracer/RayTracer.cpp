#include "RayTracer.h"
#include "RayTraceScene.h"
#include "camera/Camera.h"
#include "raytracer/shapes/BaseShape.h"

#include <vector>
#include <iostream>

using namespace glm;
using namespace std;

RayTracer::RayTracer(RayTracerConfig config) :
    m_running(false),
    m_config(config)
{

}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    if (m_running) {
        return;
    }

    auto startTS = std::chrono::system_clock::now();

    std::cout << std::endl << "Begin rendering with size (" << scene.width() << ", " << scene.height() << ")" << std::endl;

    m_running = true;

    // obtain the camera transformation matrix
    std::shared_ptr<Camera> camera = scene.getCamera();
    auto transformMat = camera->getTransformationMatrix();

    for (int i = 0; i < scene.height(); i++) {
        for (int j = 0; j < scene.width(); j++) {
            float y = (scene.height() - 1 - i + 0.5) / scene.height() - 0.5;
            float x = (j + 0.5) / scene.width() - 0.5;

            // we need to calculate the actual size of the view plane based on camera parameters
            float k = 1;
            float v = 2 * k * glm::tan(0.5 * camera->getHeightAngle() * M_PI / 180.f);
            float u = v * camera->getAspectRatio();

            glm::vec4 dir(u * x, v * y, -k, 0.f);
            glm::vec4 pos(0, 0, 0, 1);

            // transform from camera space to world space
            pos = transformMat * pos;
            dir = transformMat * dir;

            Ray r(pos, dir, FLT_MAX);

            glm::vec4 color = traceRay(r, scene, RAY_TRACE_MAX_DEPTH);
            color = color * 255.f;
            imageData[i * scene.width() + j] = RGBA(color.r, color.g, color.b);
        }
    }

    m_running = false;

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS-startTS;

    std::cout << "Render complete. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;
}

void RayTracer::renderAsync(RGBA *imageData, const RayTraceScene &scene) {
    return;
}

void RayTracer::cancel() {
    return;
}

glm::vec4 RayTracer::traceRay(const Ray& r, const RayTraceScene &scene, const int depth) {
    SurfaceInteraction isect(-1);
    bool foundIntersection = scene.intersect(r, isect);

    if (!foundIntersection) {
        // the ray escape the scene or the depth is greater than MAX_DEPTH, terminate
        return vec4(0.);
    }

    // get the normal at the intersection point
    vec4 normal = isect.normal;

    vec4 color = (normal + 1.f) / 2.f;
    color = glm::clamp(color, vec4(0.f), vec4(1.f));
    return color;
}
