#ifndef PIXELSAMPLER_H
#define PIXELSAMPLER_H

#include <glm/glm.hpp>

class RayTracer;
class RayTraceScene;

class PixelSampler
{
public:
    PixelSampler() {};
    virtual ~PixelSampler() {};

    // Sample and compute the color intensity of pixel[i, j]
    // @param i The row index of the pixel
    // @param j The column index of the pixel
    // @param raytracer The ray tracer instance
    // @param scene The scene to render
    virtual glm::vec4 sample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene) const = 0;
};

#endif // PIXELSAMPLER_H
