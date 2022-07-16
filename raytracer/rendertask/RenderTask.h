#ifndef RENDERTASK_H
#define RENDERTASK_H

#include <QRunnable>
#include "utils/RGBA.h"

class RayTracer;
class RayTraceScene;

struct RenderTaskConfig {
    int rowStart;
    int rowEnd;
    int colStart;
    int colEnd;

    RGBA *dstData;
    RayTracer *raytracer;
    const RayTraceScene &scene;


    RenderTaskConfig(RGBA *dstData_, RayTracer *raytracer_, const RayTraceScene &scene_)
        : dstData(dstData_), raytracer(raytracer_), scene(scene_) {}
};

class RenderTask : public QRunnable
{
public:
    RenderTask(RenderTaskConfig config, volatile bool *stopped);

    void run() override;

private:
    RenderTaskConfig m_config;
    volatile bool *m_stopped;
};

#endif // RENDERTASK_H
