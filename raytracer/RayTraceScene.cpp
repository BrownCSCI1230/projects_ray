#include "RayTraceScene.h"

#include "raytracer/shapes/CubeShape.h"
#include "raytracer/shapes/ConeShape.h"
#include "raytracer/shapes/CylinderShape.h"
#include "raytracer/shapes/SphereShape.h"

#include "utils/SceneParser.h"
#include "camera/RayCamera.h"

#include <iostream>

#include <QList>
#include <QtConcurrent>

using namespace std;

RayTraceScene::RayTraceScene(int width, int height, const struct RenderData &metaData) :
    m_width(width),
    m_height(height),
    m_metaData(metaData),
    m_globalData(metaData.globalData),
//    m_lights(vector<shared_ptr<Light> >()),
    m_shapes(vector<shared_ptr<BaseRTShape> >(metaData.shapes.size())),
    m_camera(make_shared<RayCamera>())
{

}

void RayTraceScene::initialize(bool useTexture) {
    auto startTS = std::chrono::system_clock::now();
    std::cout << std::endl << "Begin loading scene..." << std::endl;

    // setup the camera
    setupCamera(m_metaData.cameraData);

    // setup the lights
//    setupLights(m_metaData.lights);

    // Since the workload is pretty light,
    // there is no need to spread it onto too many threads
    int threadCnt = 4;
    int primitiveCnt = (int)m_metaData.shapes.size();
    int chunkSize = std::max(primitiveCnt / threadCnt, 10000);

    QThreadPool threadPool;
    threadPool.setMaxThreadCount(threadCnt);

    QList<QFuture<void> > futures;
    futures.reserve(threadCnt);

    for (int taskId = 0; taskId < threadCnt; taskId++) {
        int start = taskId * chunkSize;
        int end = std::min(primitiveCnt, (taskId + 1) * chunkSize);
        if (taskId == threadCnt - 1) {
            end = primitiveCnt;
        }
        QFuture<void> future = QtConcurrent::run(&threadPool, &RayTraceScene::loadPrimitives, this, m_metaData, start, end, useTexture);
        futures.append(future);

        if (end == primitiveCnt) break;
    }

    for (auto& future : futures) {
        future.waitForFinished();
    }

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS-startTS;
    std::cout << "Complete loading scene. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;
}

void RayTraceScene::setupCamera(const SceneCameraData &camera) {
    m_camera->setHeightAngle(camera.heightAngle);
    m_camera->setAspectRatio((float)m_width / m_height);
    m_camera->orientLook(camera.pos, camera.look, camera.up);
}

//void RayTraceScene::setupLights(const std::vector<SceneLightData> &lights) {
//    m_lights.reserve(lights.size());
//    return;
//}

void RayTraceScene::loadPrimitives(const struct RenderData &metaData, int start, int end, bool useTexture) {
    for (int i = start; i < end; i++) {
        shared_ptr<BaseRTShape> rtShape = nullptr;
        const auto &shape = metaData.shapes[i];
        switch(shape.primitive.type) {
            case PrimitiveType::PRIMITIVE_CUBE:
            {
                rtShape = make_shared<CubeRTShape>(shape.primitive.material);
                break;
            }
            case PrimitiveType::PRIMITIVE_CONE:
            {
                rtShape = make_shared<ConeRTShape>(shape.primitive.material);
                break;
            }
            case PrimitiveType::PRIMITIVE_CYLINDER:
            {
                rtShape = make_shared<CylinderRTShape>(shape.primitive.material);
                break;
            }
            case PrimitiveType::PRIMITIVE_SPHERE:
            {
                rtShape = make_shared<SphereRTShape>(shape.primitive.material);
                break;
            }
            case PrimitiveType::PRIMITIVE_TORUS:
            case PrimitiveType::PRIMITIVE_MESH:
            default:
            {
                break;
            }
        }

        if (rtShape) {
            rtShape->setCTM(shape.ctm);
            m_shapes[i] = rtShape;
        }
    }
    return;
}

const int& RayTraceScene::width() const {
    return m_width;
}

const int& RayTraceScene::height() const {
    return m_height;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return m_globalData;
}

const std::shared_ptr<Camera> RayTraceScene::getCamera() const {
    return m_camera;
}

bool RayTraceScene::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    bool ret = false;
    for (shared_ptr<BaseRTShape> const &shape : m_shapes) {
        if (shape->intersect(ray, oSurInteraction)) {
            ret = true;
        }
    }
    return ret;
}
