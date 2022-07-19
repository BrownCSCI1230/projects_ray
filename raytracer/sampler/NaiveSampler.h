#ifndef NAIVESAMPLER_H
#define NAIVESAMPLER_H

#include "PixelSampler.h"

class NaiveSampler : public PixelSampler
{
public:
    NaiveSampler();

    // Sample and compute the color density of pixel[i, j]
    // @param i The row index of the pixel
    // @param j The column index of the pixel
    // @param raytracer The ray tracer instance
    // @param scene The scene to render
    virtual glm::vec4 sample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene) const override;
};

#endif // NAIVESAMPLER_H
