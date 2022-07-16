#include "AdaptiveSuperSampler.h"
#include "camera/Camera.h"
#include "raytracer/RayTracer.h"
#include "raytracer/RayTraceScene.h"

#include <memory>

using namespace glm;

static float superSampleThreshold = 50.f / 255.f;

static bool exceedThreshold(const vec4 &colorA, const vec4 &colorB) {
    return fabs(colorA.r - colorB.r) > superSampleThreshold ||
           fabs(colorA.g - colorB.g) > superSampleThreshold ||
           fabs(colorA.b - colorB.b) > superSampleThreshold;
}

AdaptiveSuperSampler::AdaptiveSuperSampler()
{

}

glm::vec4 AdaptiveSuperSampler::sample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene) const {
    vec2 center(0.5, 0.5);
    float offset = 0.25;
    return superSample(i, j, raytracer, scene, center, offset, 2);
}

glm::vec4 AdaptiveSuperSampler::superSample(int i, int j, RayTracer *raytracer, const RayTraceScene &scene, const glm::vec2 &center, const float offset, const int depth) const {
    std::vector<vec4> sampleColors(4);
    std::vector<vec2> samplePixels = generateSamples(center, offset);

    std::shared_ptr<Camera> camera = scene.getCamera();
    auto transformMat = camera->getTransformationMatrix();

    // evaluate the color for each sample
    for (int s = 0; s < 4; s++) {
        float x = (j + samplePixels[s].x) / scene.width() - 0.5;
        float y = (scene.height() - 1 - i + samplePixels[s].y) / scene.height() - 0.5;

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

        sampleColors[s] = raytracer->traceRay(r, scene, RAY_TRACE_MAX_DEPTH);
    }

    // the final color
    vec4 ret(0.f, 0.f, 0.f, 0.f);

    // compute the offset for next depth
    float newOffset = offset / 2;
    std::vector<bool> needSuperSample(4, false);

    checkSuperSampleIsNeeded(sampleColors, needSuperSample);

    for (int s = 0; s < 4; s++) {
        if (depth && needSuperSample[s]) {
            // if the difference exceeds the threshold, perform super-sampling around current sample
            sampleColors[s] = superSample(i, j, raytracer, scene, samplePixels[s], newOffset, depth - 1);
        }
        ret += sampleColors[s];
    }
    ret = ret / 4.f;
    return ret;
}

std::vector<glm::vec2> AdaptiveSuperSampler::generateSamples(const glm::vec2 &center, float offset) const {
    std::vector<vec2> ret(4);
    vec2 xyMin = vec2(0.f, 0.f);
    vec2 xyMax = vec2(1.f, 1.f);

    ret[0] = vec2(center.x - offset, center.y - offset);
    ret[1] = vec2(center.x + offset, center.y - offset);
    ret[2] = vec2(center.x - offset, center.y + offset);
    ret[3] = vec2(center.x + offset, center.y + offset);

    ret[0] = glm::clamp(ret[0], xyMin, xyMax);
    ret[1] = glm::clamp(ret[1], xyMin, xyMax);
    ret[2] = glm::clamp(ret[2], xyMin, xyMax);
    ret[3] = glm::clamp(ret[3], xyMin, xyMax);

    return ret;
}

void AdaptiveSuperSampler::checkSuperSampleIsNeeded(std::vector<glm::vec4> &sampleColors, std::vector<bool> &needSuperSample) const {
    assert(needSuperSample.size() == 4);

    // The comparison is hardcoded for each sample to only compare with the adjacent ones
    bool exceed01 = exceedThreshold(sampleColors[0], sampleColors[1]);
    bool exceed02 = exceedThreshold(sampleColors[0], sampleColors[2]);
    bool exceed13 = exceedThreshold(sampleColors[1], sampleColors[3]);
    bool exceed23 = exceedThreshold(sampleColors[2], sampleColors[3]);

    needSuperSample[0] = exceed01 || exceed02;
    needSuperSample[1] = exceed01 || exceed13;
    needSuperSample[2] = exceed02 || exceed23;
    needSuperSample[3] = exceed13 || exceed23;
    return;
}
