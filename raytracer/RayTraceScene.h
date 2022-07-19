#ifndef RAYTRACESCENE_H
#define RAYTRACESCENE_H

#include <memory>
#include "SceneParser.h"
#include "utils/SceneData.h"
#include "raytracer/interface/Intersect.h"

class BaseRTShape;
class Camera;
class RayCamera;
class Light;
class TextureManager;

struct RenderData;

class RayTraceScene
{
public:
    RayTraceScene(int width, int height, const struct RenderData &metaData);

    void initialize(bool useTexture);

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of the light data of the scene
    const std::vector<std::shared_ptr<Light> >& getLights() const;

    // The getter of the camera instance of the scene
    const std::shared_ptr<Camera> getCamera() const;

    // Check if the ray intersects with any primitive in the scene.
    // @param ray The Ray object describing the ray in the WORLD SPACE.
    // @param oSurInteraction Upon return, contains the intersection information if there is one.
    // @return A boolean value indicating if there is an intersection with a primitive.
    bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction, bool useKdTree) const;

private:
    void setupCamera(const SceneCameraData &camera);
    void setupLights(const std::vector<SceneLightData> &lights);

    void loadPrimitives(const struct RenderData &metaData, int start, int end, bool useTexture);

    // Raw data
    int m_width, m_height;
    struct RenderData m_metaData;

    // scene metadata
    SceneGlobalData m_globalData;

    std::vector<std::shared_ptr<Light> > m_lights;
    std::vector<std::shared_ptr<BaseRTShape> > m_shapes;

    // scene camera instance
    std::shared_ptr<RayCamera> m_camera;

    // texture manager that accelerates texture loading
    std::shared_ptr<TextureManager> m_textureManager;

    // the intersection checker
    std::shared_ptr<Intersect> m_naiveIntersect;
    std::shared_ptr<Intersect> m_kdTree;
};

#endif // RAYTRACESCENE_H
