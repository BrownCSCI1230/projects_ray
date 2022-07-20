#ifndef CYLINDERRAYTRACER_H
#define CYLINDERRAYTRACER_H

#include "BaseShape.h"

class CylinderRTShape : public BaseRTShape
{
public:
    CylinderRTShape(SceneMaterial material);

    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;


private:
    glm::vec4 getNormalTop() const;
    glm::vec4 getNormalBottom() const;
    glm::vec4 getNormalBarrel(const glm::vec4 &intersection) const;

private:
    float m_maxY;
    float m_minY;
    float m_R;
};

#endif // CYLINDERRAYTRACER_H
