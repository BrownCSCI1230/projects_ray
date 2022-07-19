#include "RayTracer.h"
#include "RayTraceScene.h"
#include "camera/Camera.h"
#include "raytracer/interface/Light.h"
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

    if (depth > 0) {
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
