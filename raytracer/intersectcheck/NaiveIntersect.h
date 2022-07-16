#ifndef NAIVEINTERSECT_H
#define NAIVEINTERSECT_H

#include <vector>
#include <memory>
#include "raytracer/interface/Intersect.h"

class BaseRTShape;

class NaiveIntersectCheck : public Intersect
{
public:
    NaiveIntersectCheck();
    NaiveIntersectCheck(std::vector<std::shared_ptr<BaseRTShape>> primitives);

    ~NaiveIntersectCheck();

    // Check if a ray intersects with any primitives.
    // @param ray The Ray object describing the ray in the WORLD SPACE.
    // @param oSurInteraction Upon return, contains the intersection information if there is one.
    // @return A boolean value indicating if there is an intersection with the primitives in the tree.
    virtual bool intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const override;

private:
    std::vector<std::shared_ptr<BaseRTShape>> m_primitives;
};

#endif // NAIVEINTERSECT_H
