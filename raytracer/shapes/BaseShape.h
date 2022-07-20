#ifndef BASERAYTRACER_H
#define BASERAYTRACER_H

#include <QImage>
#include <glm/glm.hpp>
#include "utils/SceneData.h"
#include "raytracer/interface/Intersect.h"

struct SurfaceIntersection {
    float t;
    int idx;
    SurfaceIntersection(float t_, int idx_) : t(t_), idx(idx_) {}
};

struct ShapePQCompare {
    bool operator() (const SurfaceIntersection &a, const SurfaceIntersection &b) {
        return a.t > b.t;
    }
};

class BaseRTShape : public Intersect
{
public:
    BaseRTShape(SceneMaterial material);
    virtual ~BaseRTShape();

    // Set the cumulated transformation matrix of the object.
    // FROM OBJECT SPACE TO WORLD SPACE.
    void setCTM(const glm::mat4 &trans);

    SceneMaterial getMaterial() const;

    // Check if the ray intersects with the shape.
    // @param ray The Ray object that describe a ray in the WORLD SPACE.
    // The function should properly update the tMax of the ray based on the intersection.
    // @param oSurInteraction Upon return, it contains the information of the intersection if there indeed is an intersection.
    // @return A boolean value indicating if the ray intersects with current object.
    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const { return false; };

protected:
    glm::mat4 m_CTM;
    glm::mat4 m_ICTM;
    glm::mat4 m_ICTM_T;

private:
    // material of the shape
    SceneMaterial m_material;

    // texture
    std::shared_ptr<QImage> m_textureImage;
};

#endif // BASERAYTRACER_H
