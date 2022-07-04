#ifndef BASERAYTRACER_H
#define BASERAYTRACER_H

#include <QImage>
#include <glm/glm.hpp>
#include "utils/scenedefinition.h"
#include "raytracer/interface/intersect.h"
#include "raytracer/utils/boundutils.h"

class TextureManager;

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
    BaseRTShape(SceneMaterial material, std::shared_ptr<TextureManager> textureManager);
    virtual ~BaseRTShape();

    // Set the cumulated transformation matrix of the object.
    // FROM OBJECT SPACE TO WORLD SPACE.
    void setCTM(const glm::mat4 &trans);

    SceneMaterial getMaterial() const;

    // Obtain the texture pixel color at the intersection point.
    // @param uv The uv coordinates.
    // @param oColor On return, contains the pixel color if no error happens.
    // @return A boolean value indicating if the pixel color of the texture is successfully retrieved.
    bool getTexture(glm::vec2 uv, glm::vec4 &oColor) const;

    // Get the boundary of the primitive in OBJECT SPACE.
    virtual Bound getObjectBound() const = 0;

    // Get the boundary of the primitive in WORLD SPACE.
    virtual Bound getWorldBound() const;

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

    // texture loader
    bool loadTexture(std::shared_ptr<TextureManager> textureManager);
};

#endif // BASERAYTRACER_H
