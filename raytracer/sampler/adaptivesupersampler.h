#ifndef ADAPTIVESUPERSAMPLER_H
#define ADAPTIVESUPERSAMPLER_H

#include "pixelsampler.h"
#include <glm/glm.hpp>
#include <vector>

class AdaptiveSuperSampler : public PixelSampler
{
public:
    AdaptiveSuperSampler();

    // Sample and compute the color density of pixel[i, j]
    // @param i The row index of the pixel
    // @param j The column index of the pixel
    // @param raytracer The ray tracer instance
    // @param scene The scene to render
    virtual glm::vec4 sample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene) const override;
private:
    glm::vec4 superSample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene, const glm::vec2 &center, const float offset, const int depth) const;
    std::vector<glm::vec2> generateSamples(const glm::vec2 &center, float offset) const;

    void checkSuperSampleIsNeeded(std::vector<glm::vec4> &sampleColors, std::vector<bool> &needSuperSample) const;

};

#endif // ADAPTIVESUPERSAMPLER_H
