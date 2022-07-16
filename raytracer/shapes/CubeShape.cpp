#include "cubeshape.h"
#include <queue>

using namespace glm;
using namespace std;

CubeRTShape::CubeRTShape(SceneMaterial material, std::shared_ptr<TextureManager> textureManager) :
    BaseRTShape(material, textureManager),
    m_edge(0.5f),
    m_normals(vector<vec3>(6))
{
    // setup normal of six surfaces
    m_normals[0] = {1.f, 0.f, 0.f};
    m_normals[1] = {-1.f, 0.f, 0.f};
    m_normals[2] = {0.f, 1.f, 0.f};
    m_normals[3] = {0.f, -1.f, 0.f};
    m_normals[4] = {0.f, 0.f, 1.f};
    m_normals[5] = {0.f, 0.f, -1.f};
}

Bound CubeRTShape::getObjectBound() const {
    Bound b;
    b.pMax = vec4(m_edge, m_edge, m_edge, 1.0f);
    b.pMin = vec4(-m_edge, -m_edge, -m_edge, 1.0f);
    return b;
}


glm::vec4 CubeRTShape::getNormal(int idx) const {
    // transform the intersection point from world space to object space
    vec3 normal = mat3(transpose(m_ICTM)) * m_normals[idx];
    return vec4(normalize(normal), 0.f);
}

glm::vec2 CubeRTShape::getUV(const glm::vec4 &intersection, int idx) const {
    float u, v;
    switch (idx) {
        case 0:
        {
            u = m_edge - intersection.z;
            v = m_edge - intersection.y;
            break;
        }
        case 1:
        {
            u = intersection.z + m_edge;
            v = m_edge - intersection.y;
            break;
        }
        case 2:
        {
            u = intersection.x + m_edge;
            v = intersection.z + m_edge;
            break;
        }
        case 3:
        {
            u = intersection.x + m_edge;
            v = m_edge - intersection.z;
            break;
        }
        case 4:
        {
            u = intersection.x + m_edge;
            v = m_edge - intersection.y;
            break;
        }
        case 5:
        {
            u = m_edge - intersection.x;
            v = m_edge - intersection.y;
            break;
        }
        default:
        {
            // This should not happen, hence return false indicating an error.
        }
    }
    return vec2(u, v);
}

bool CubeRTShape::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    vec4 p = m_ICTM * ray.origin, d = m_ICTM * ray.direction;

    priority_queue<SurfaceIntersection, vector<SurfaceIntersection>, ShapePQCompare> pq;

    auto checkIntersection = [&p, &d, this](float t, bool x, bool y, bool z) {
        if (t < 0) return false;

        vec4 potentialPoint = p + d * t;
        if (x && (potentialPoint.x < -m_edge || potentialPoint.x > m_edge)) {
            return false;
        }

        if (y && (potentialPoint.y < -m_edge || potentialPoint.y > m_edge)) {
            return false;
        }

        if (z && (potentialPoint.z < -m_edge || potentialPoint.z > m_edge)) {
            return false;
        }

        return true;
    };

    float t;
    if (d.x != 0.f) {
        t = (m_edge - p.x) / d.x;
        if (checkIntersection(t, false, true, true)) {
            pq.push(SurfaceIntersection(t, 0));
        }

        t = (-m_edge - p.x) / d.x;
        if (checkIntersection(t, false, true, true)) {
            pq.push(SurfaceIntersection(t, 1));
        }
    }

    if (d.y != 0.f) {
        t = (m_edge - p.y) / d.y;
        if (checkIntersection(t, true, false, true)) {
            pq.push(SurfaceIntersection(t, 2));
        }

        t = (-m_edge - p.y) / d.y;
        if (checkIntersection(t, true, false, true)) {
            pq.push(SurfaceIntersection(t, 3));
        }
    }

    if (d.z != 0.f) {
        t = (m_edge - p.z) / d.z;
        if (checkIntersection(t, true, true, false)) {
            pq.push(SurfaceIntersection(t, 4));
        }

        t = (-m_edge - p.z) / d.z;
        if (checkIntersection(t, true, true, false)) {
            pq.push(SurfaceIntersection(t, 5));
        }
    }

    while (!pq.empty() && pq.top().t < 0.f) { pq.pop(); }

    if (pq.empty() || pq.top().t > ray.tMax) {
        return false;
    }

    ray.tMax = pq.top().t;
    vec4 isectP = p + pq.top().t * d;
    vec4 normal = getNormal(pq.top().idx);
    vec2 uv = getUV(isectP, pq.top().idx);

    oSurInteraction = SurfaceInteraction(pq.top().t, normal, uv);
    oSurInteraction.primitive = (BaseRTShape *)this;

    return true;
}
