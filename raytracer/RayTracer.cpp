#include "RayTracer.h"
#include "RayTraceScene.h"
#include "camera/Camera.h"
#include "raytracer/interface/Light.h"
#include "raytracer/sampler/NaiveSampler.h"
#include "raytracer/sampler/AdaptiveSuperSampler.h"
#include "raytracer/rendertask/RenderTask.h"
#include "raytracer/shapes/BaseShape.h"

#include <vector>
#include <iostream>
#include <QtConcurrent>

using namespace glm;
using namespace std;

RayTracer::RayTracer(RayTracerConfig config) :
    m_running(false),
    m_cancel(false),
    m_threadPool(QThreadPool()),
    m_config(config)
{
    m_threadPool.setMaxThreadCount(QThread::idealThreadCount());

    if (config.enableSuperSample) {
        m_pixelSampler = std::make_shared<AdaptiveSuperSampler>();
    } else {
        m_pixelSampler = std::make_shared<NaiveSampler>();
    }
}

void RayTracer::cancel() {
    if (!m_running) {
        return;
    }

    m_cancel = true;
    m_threadPool.waitForDone();
}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    if (m_running) {
        return;
    }

    auto startTS = std::chrono::system_clock::now();

    std::cout << std::endl << "Begin rendering with size (" << scene.width() << ", " << scene.height() << ")" << std::endl;

    m_running = true;
    m_cancel = false;

    // prepare configuration for all render tasks
    vector<RenderTaskConfig> allConfigs;
    prepareRenderTasks(imageData, scene, allConfigs);

    for (auto &config : allConfigs) {
        RenderTask *task = new RenderTask(config, &m_cancel);
        m_threadPool.start(task);
    }

    m_threadPool.waitForDone();

    m_running = false;

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS-startTS;

    std::cout << "Render complete. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;
}

void RayTracer::renderAsync(RGBA *imageData, const RayTraceScene &scene) {
    QFuture<void> future = QtConcurrent::run(&RayTracer::render, this, imageData, scene);
    return;
}

glm::vec4 RayTracer::traceRay(const Ray& r, const RayTraceScene &scene, const int depth) {
    SurfaceInteraction isect(-1);
    bool foundIntersection = scene.intersect(r, isect);

    if (!foundIntersection) {
        // the ray escape the scene or the depth is greater than MAX_DEPTH, terminate
        return vec4(0.);
    }


    // compute the intersection point
    vec4 isectPoint = r.origin + r.tMax * r.direction;
    // get the normal at the intersection point
    vec4 normal = isect.normal;
    // get the material at the intersection point
    SceneMaterial material = isect.primitive->getMaterial();

    const auto globalData = scene.getGlobalData();
    const auto lights = scene.getLights();

    vec4 color(0.);

    // accumulate ambient & emissive contribution
    color += globalData.ka * material.cAmbient + material.cEmissive;
    // initialize diffuse color and specular color
    vec4 diffuseColor = vec4(0.f);
    vec4 specularColor = vec4(0.f);
    for (const auto& light : lights) {
        vec4 isectToLight = light->getDirection(isectPoint);
        vec4 lightIntensity = light->getIntensity(isectPoint);

        if (m_config.enableShadow) {
            // perform shadow detection
            vec4 origin = isectPoint + isectToLight * 0.001f;
            float tMax = light->getDistance(origin);

            Ray shadowRay(origin, isectToLight, tMax);
            SurfaceInteraction shadowIsect(-1);
            if (scene.intersect(shadowRay, shadowIsect)) {
                // the shadow ray intersects with another primitive, this light should be ignored.
                continue;
            }
        }

        vec4 isectToLightMirror = glm::reflect(-isectToLight, normal);
        vec4 wo = -r.direction;

        // The cosine value should be in range [0, 1].
        // If the value is negative, then the light is never going to reach this intersection
        float cosineLN = std::max(0.f, glm::dot(normal, isectToLight));
        float cosineRE = glm::max(0.f, glm::dot(isectToLightMirror, wo));
        float N = material.shininess;

        vec4 diffuse = globalData.kd * material.cDiffuse;
        vec4 specular = N == 0 ? vec4(0.f) : globalData.ks * material.cSpecular;

        if (m_config.enableTextureMap) {
            vec4 texture = vec4(0.f);
            if (isect.primitive->getTexture(isect.uv, texture)) {
                // blend the material color with the texture color if needed.
                // Because there is no meaning of 'specular texture' and no ambient light,
                // We only blend with the diffuse light
                diffuse = (1.f - material.blend) * diffuse + texture * material.blend;
            }
        }

        diffuseColor += lightIntensity * diffuse * cosineLN;
        specularColor += lightIntensity * specular * powf(cosineRE, N);
    }

    color += diffuseColor + specularColor;

    // calculate refraction
    if (depth > 0) {
        color += specularRefraction(r, isect, scene, depth);
        color += specularReflection(r, isect, scene, depth);
    }

    color = glm::clamp(color, vec4(0.f), vec4(1.f));
    return color;
}

glm::vec4 RayTracer::specularReflection(const Ray& r, const SurfaceInteraction& isect, const RayTraceScene& scene, const int depth) {
    SceneMaterial material = isect.primitive->getMaterial();
    if (!m_config.enableReflection || material.cReflective.rgb() == vec3(0.f)) {
        return vec4(0);
    }

    glm::vec4 wo = -r.direction;
    glm::vec4 normal = isect.normal;

    if (glm::dot(wo, normal) < 0) {
        return vec4(0);
    }

    vec4 isectPoint = r.origin + isect.t * r.direction;

    vec4 reflectDir = glm::reflect(r.direction, normal);
    vec4 reflectPos = isectPoint + reflectDir * 0.001f;
    Ray reflectRay = Ray(reflectPos, reflectDir, FLT_MAX);
    vec4 color = traceRay(reflectRay, scene, depth - 1);
    color = scene.getGlobalData().ks * material.cReflective * color;
    return color;
}

glm::vec4 RayTracer::specularRefraction(const Ray& r, const SurfaceInteraction& isect, const RayTraceScene& scene, const int depth) {
    SceneMaterial material = isect.primitive->getMaterial();
    if (!m_config.enableRefraction || material.cTransparent.rgb() == vec3(0.f)) {
        return glm::vec4(0.f);
    }

    // The ratio of indices of refraction.
    float eta = 1.f / material.ior;
    glm::vec4 normal = isect.normal;
    glm::vec4 isectPoint = r.origin + isect.t * r.direction;
    glm::vec4 color(0);

    // The refracted ray from the incident ray and the normal
    vec4 innerRefractDir = glm::refract(r.direction, normal, eta);

    if (innerRefractDir != vec4(0.f)) {
        // add tiny offset similar in shadow detection to avoid self-intersection
        vec4 innerRefractPos = isectPoint + innerRefractDir * 0.001f;
        Ray innerRefractRay(innerRefractPos, innerRefractDir, FLT_MAX);

        // check the intersection point where the ray leaves the shape primitive
        SurfaceInteraction exitIsect(-1);
        bool hasExitPoint = isect.primitive->intersect(innerRefractRay, exitIsect);
        if (hasExitPoint) {
            // cast a ray with a similar offset from the original exit point
            vec4 exitPoint = innerRefractPos + innerRefractDir * exitIsect.t;
            // the surface normal at the exit point
            vec4 exitNormal = exitIsect.normal;

            // refract the ray back into empty space
            vec4 exitRayDir = glm::refract(innerRefractDir, -exitNormal, material.ior);
            vec4 exitRayPos = exitPoint + exitRayDir * 0.001f;

            // check for further intersection
            Ray exitRay(exitRayPos, exitRayDir, FLT_MAX);
            vec4 refractionColor = traceRay(exitRay, scene, depth - 1);
            color += scene.getGlobalData().kt * material.cTransparent * refractionColor;
        }
    }

    return color;
}

void RayTracer::prepareRenderTasks(RGBA *dstData, const RayTraceScene &scene, std::vector<RenderTaskConfig> &oTasks) {
    if (!m_config.enableParallelism) {
        // if parallel rendering is disabled, create only one task
        RenderTaskConfig config(dstData, this, scene);
        config.rowStart = 0;
        config.colStart = 0;
        config.rowEnd = scene.height();
        config.colEnd = scene.width();
        oTasks.emplace_back(config);
        return;
    }

    static int minBlockW = 4;
    static int minBlockH = 4;

    int blockWCnt = scene.width() / 100;
    int blockHCnt = scene.height() / 100;

    blockWCnt = std::max(blockWCnt, minBlockW);
    blockHCnt = std::max(blockHCnt, minBlockH);

    int blockW = scene.width() / blockWCnt;
    int blockH = scene.height() / blockHCnt;

    for (int i = 0; i < blockHCnt; i++) {
        for (int j = 0; j < blockWCnt; j++) {
            RenderTaskConfig taskConfig(dstData, this, scene);
            taskConfig.rowStart = i * blockH;
            taskConfig.colStart = j * blockW;

            if (i == blockHCnt - 1) {
                taskConfig.rowEnd = scene.height();
            } else {
                taskConfig.rowEnd = (i + 1) * blockH;
            }

            if (j == blockWCnt - 1) {
                taskConfig.colEnd = scene.width();
            } else {
                taskConfig.colEnd = (j + 1) * blockW;
            }

            oTasks.emplace_back(taskConfig);
        }
    }
    return;
}
