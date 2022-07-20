#ifndef SPHERERAYTRACER_H
#define SPHERERAYTRACER_H

#include "BaseShape.h"

class SphereRTShape : public BaseRTShape
{
public:
    SphereRTShape(SceneMaterial material);

    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;

private:
    glm::vec4 getNormal(const glm::vec4 &intersection) const;
    glm::vec2 getUV(const glm::vec4 &intersection) const;

private:
    float m_R;
};

#endif // SPHERERAYTRACER_H
