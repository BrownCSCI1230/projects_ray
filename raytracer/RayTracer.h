#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <glm/glm.hpp>
#include "utils/RGBA.h"
#include "raytracer/interface/Intersect.h"

#define RAY_TRACE_MAX_DEPTH 4

class RayTraceScene;

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
    // The ray tracer will render the scene and fill the imageData in place.
    // The ray tracer is only capable of running one render session at a time.
    // When the ray tracer is running, call on RayTracer::render will return immediately.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene containing necessary info for rendering.
    void render(RGBA *imageData, const RayTraceScene &scene);

    // Render the scene asynchronously.
    // The ray tracer will render the scene and fill the imageData in place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene containing necessary info for rendering.
    void renderAsync(RGBA *imageData, const RayTraceScene &scene);

    // Cancel the current render session and wait for all background tasks to stop.
    // If the ray tracer is idle, call on this function will return immediately.
    void cancel();

private:
    // Perform ray tracing in the given scene with a given ray direction
    // @param r The structure that describe the ray to trace.
    // @param scene The scene to render.
    // @param depth The maximum depth for recursive ray tracing.
    glm::vec4 traceRay(const Ray& r, const RayTraceScene &scene, const int depth);

private:
    bool m_running;

    RayTracerConfig m_config;
};

#endif // RAYTRACER_H
