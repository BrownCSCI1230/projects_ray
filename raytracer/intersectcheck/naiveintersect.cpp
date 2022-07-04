#include "naiveintersect.h"
#include "raytracer/shapes/baseshape.h"

using namespace std;

NaiveIntersectCheck::NaiveIntersectCheck() :
    m_primitives(vector<shared_ptr<BaseRTShape>>())
{

}

NaiveIntersectCheck::NaiveIntersectCheck(std::vector<std::shared_ptr<BaseRTShape>> primitives) :
    m_primitives(move(primitives))
{

}

NaiveIntersectCheck::~NaiveIntersectCheck() {

}

bool NaiveIntersectCheck::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    bool ret = false;
    for (shared_ptr<BaseRTShape> const &shape : m_primitives) {
        if (shape->intersect(ray, oSurInteraction)) {
            ret = true;
        }
    }
    return ret;
}
