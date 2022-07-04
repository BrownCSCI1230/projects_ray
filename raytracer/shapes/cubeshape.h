#ifndef CUBERAYTRACER_H
#define CUBERAYTRACER_H

#include "baseshape.h"
#include <vector>

class CubeRTShape : public BaseRTShape
{
public:
    CubeRTShape(SceneMaterial material, std::shared_ptr<TextureManager> textureManager);

    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;

    virtual Bound getObjectBound() const override;

private:
    // surface index mapping
    // 0: right
    // 1: left
    // 2: top
    // 3: bottom
    // 4: front
    // 5: back
    glm::vec4 getNormal(int idx) const;
    glm::vec2 getUV(const glm::vec4 &intersection, int idx) const;

private:
    float m_edge;
    std::vector<glm::vec3> m_normals;
};

#endif // CUBERAYTRACER_H
