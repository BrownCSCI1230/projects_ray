#include "SphereShape.h"
#include <queue>

using namespace std;
using namespace glm;

SphereRTShape::SphereRTShape(SceneMaterial material) :
    BaseRTShape(material),
    m_R(0.5f)
{

}

glm::vec4 SphereRTShape::getNormal(const glm::vec4 &intersection) const {
    vec4 normal = vec4(intersection.x, intersection.y, intersection.z, 0.f);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

glm::vec2 SphereRTShape::getUV(const glm::vec4 &intersection) const {
    float u, v;

    float Py = glm::clamp(intersection.y, -0.5f, 0.5f);
    v = m_R - asin(Py / m_R) / M_PI;

    if (v <= 0.f || v >= 1.f) {
        u = 0.5f;
    } else {
        float theta = std::atan2(intersection.z, intersection.x);
        if (theta < 0) {
            u = -theta / (2 * M_PI);
        } else {
            u = 1.f - theta / (2 * M_PI);
        }
    }
    return vec2(u, v);
}

bool SphereRTShape::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    vec4 p = m_ICTM * ray.origin, d = m_ICTM * ray.direction;

    priority_queue<SurfaceIntersection, vector<SurfaceIntersection>, ShapePQCompare> pq;

    float a = d.x * d.x + d.y * d.y + d.z * d.z;
    float b = 2 * (p.x * d.x + p.y * d.y + p.z * d.z);
    float c = p.x * p.x + p.y * p.y + p.z * p.z - m_R * m_R;

    float square = b * b - 4 * a * c;
    if (a == 0 || square < 0) {
        return false;
    }

    if (square == 0) {
        float t = -b / (2 * a);
        if (t > 0 && t < ray.tMax) {
            pq.push(SurfaceIntersection(t, 0));
        }
    } else {
        float root = glm::sqrt(square);
        float t1 = (-b + root) / (2 * a);
        float t2 = (-b - root) / (2 * a);
        if (t1 > 0 && t1 < ray.tMax) {
            pq.push(SurfaceIntersection(t1, 0));
        }
        if (t2 > 0 && t2 < ray.tMax) {
            pq.push(SurfaceIntersection(t2, 0));
        }
    }

    while (!pq.empty() && pq.top().t < 0.f) { pq.pop(); }

    if (pq.empty() || pq.top().t > ray.tMax) {
        return false;
    }

    ray.tMax = pq.top().t;

    glm::vec4 intersection = p + pq.top().t * d;
    glm::vec4 normal = getNormal(intersection);
    glm::vec2 uv = getUV(intersection);
    oSurInteraction = SurfaceInteraction(pq.top().t, normal, uv);
    oSurInteraction.primitive = (BaseRTShape *)this;

    return true;
}
