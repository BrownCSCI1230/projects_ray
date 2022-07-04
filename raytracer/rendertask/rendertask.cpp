#include "rendertask.h"
#include "raytracer/raytracer.h"
#include "raytracer/raytracescene.h"
#include "raytracer/sampler/pixelsampler.h"

#include <iostream>
#include <glm/glm.hpp>

RenderTask::RenderTask(RenderTaskConfig config, volatile bool *stopped) :
    QRunnable(),
    m_config(config),
    m_stopped(stopped)

{

}

void RenderTask::run() {
    RayTracer *raytracer = m_config.raytracer;
    int rowStart = m_config.rowStart;
    int colStart = m_config.colStart;
    int rowEnd = m_config.rowEnd;
    int colEnd = m_config.colEnd;

    for (int i = rowStart; i < rowEnd; i++) {
        for (int j = colStart; j < colEnd; j++) {
            if (*m_stopped) {
                return;
            }

            glm::vec4 color = raytracer->m_pixelSampler->sample(i, j, raytracer, m_config.scene);
            color = color * 255.f;
            m_config.dstData[i * m_config.scene.width() + j] = RGBA(color.r, color.g, color.b);
        }
    }
}
