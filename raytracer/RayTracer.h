#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <QThreadPool>
#include <glm/glm.hpp>
#include "utils/RGBA.h"
#include "raytracer/interface/Intersect.h"

#define RAY_TRACE_MAX_DEPTH 4

class RayTraceScene;
class PixelSampler;

struct RenderTaskConfig;

struct RayTracerConfig {
    RayTracerConfig()
        : enableShadow(false),
          enableReflection(false),
          enableRefraction(false),
          enableTextureMap(false),
          enableParallelism(false),
          enableSuperSample(false),
          enableAcceleration(false) {}

    bool enableShadow;
    bool enableReflection;
    bool enableRefraction;
    bool enableTextureMap;
    bool enableParallelism;
    bool enableSuperSample;
    bool enableAcceleration;
};

class RayTracer
{
public:
    RayTracer(RayTracerConfig config);

    // Render the scene synchronously.
    // The ray tracer is only capable of running one render session at a time.
    // When the ray tracer is running, call on RayTracer::render will return immediately.
    // @param imageData The pointer to the imageData.
    // @param scene The scene containing necessary info for rendering.
    void render(RGBA *imageData, const RayTraceScene &scene);

    // Render the scene asynchronously.
    // @param imageData The pointer to the imageData.
    // @param scene The scene containing necessary info for rendering.
    void renderAsync(RGBA *imageData, const RayTraceScene &scene);

    // Cancel the current render session and wait for all background tasks to stop.
    // If the ray tracer is idle, call on this function will return immediately.
    void cancel();

    // Perform ray tracing in the given scene with a given ray direction
    // @param r The structure that describe the ray to trace.
    // @param scene The scene to render.
    // @param depth The maximum depth for recursive ray tracing.
    glm::vec4 traceRay(const Ray& r, const RayTraceScene &scene, const int depth);

private:
    void prepareRenderTasks(RGBA *dstData, const RayTraceScene &scene, std::vector<RenderTaskConfig> &oTasks);

    glm::vec4 specularReflection(const Ray& r, const SurfaceInteraction& isect, const RayTraceScene& scene, const int depth);
    glm::vec4 specularRefraction(const Ray& r, const SurfaceInteraction& isect, const RayTraceScene& scene, const int depth);

private:
    bool m_running;

    volatile bool m_cancel;

    QThreadPool m_threadPool;

    RayTracerConfig m_config;

    std::shared_ptr<PixelSampler> m_pixelSampler;

    friend class RenderTask;
};

#endif // RAYTRACER_H
