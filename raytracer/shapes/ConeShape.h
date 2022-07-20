#ifndef CONERAYTRACER_H
#define CONERAYTRACER_H

#include "BaseShape.h"

class ConeRTShape : public BaseRTShape
{
public:
    ConeRTShape(SceneMaterial material);

    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;

private:
    glm::vec4 getNormalDisk() const;
    glm::vec4 getNormalHat(const glm::vec4 &intersection) const;

    glm::vec2 getUVDisk(const glm::vec4 &intersection) const;
    glm::vec2 getUVHat(const glm::vec4 &intersection) const;

private:
    float m_R;
};

#endif // CONERAYTRACER_H
