#include "CylinderShape.h"
#include <queue>

#define CYLD_TPDISK_IDX 0
#define CYLD_BTDISK_IDX 1
#define CYLD_BARREL_IDX 2

using namespace std;
using namespace glm;

CylinderRTShape::CylinderRTShape(SceneMaterial material) :
    BaseRTShape(material),
    m_maxY(0.5f),
    m_minY(-0.5f),
    m_R(0.5f)
{

}

glm::vec4 CylinderRTShape::getNormalTop() const {
    vec4 normal = vec4(0.f, 1.f, 0.f, 0.f);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

glm::vec4 CylinderRTShape::getNormalBottom() const {
    vec4 normal = vec4(0.f, -1.f, 0.f, 0.f);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

glm::vec4 CylinderRTShape::getNormalBarrel(const glm::vec4 &intersection) const {
    vec4 normal = vec4(intersection.x, 0.f, intersection.z, 0.f);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

bool CylinderRTShape::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    vec4 p = m_ICTM * ray.origin, d = m_ICTM * ray.direction;

    priority_queue<SurfaceIntersection, vector<SurfaceIntersection>, ShapePQCompare> pq;

    auto checkCapIntersection = [&p, &d, &pq, this](bool top) {
        if (d.y == 0) {
            return;
        }

        float y = this->m_minY;
        if (top) { y = this->m_maxY; }

        float t = (y - p.y) / d.y;

        vec4 potentialPoint = p + t * d;
        if (t > 0.f && potentialPoint.x * potentialPoint.x + potentialPoint.z * potentialPoint.z <= m_R * m_R) {
            pq.push(SurfaceIntersection(t, top ? CYLD_TPDISK_IDX : CYLD_BTDISK_IDX));
        }
    };

    // check cap intersection
    checkCapIntersection(true);
    checkCapIntersection(false);

    // check barrel intersection
    float a = d.x * d.x + d.z * d.z;
    float b = 2 * (p.x * d.x + p.z * d.z);
    float c = p.x * p.x + p.z * p.z - m_R * m_R;

    float square = b * b - 4 * a * c;
    if (a != 0 && square > 0) {
        float root = glm::sqrt(square);
        float t1 = (-b + root) / (2 * a);
        float t2 = (-b - root) / (2 * a);
        vec4 p1 = p + t1 * d;
        vec4 p2 = p + t2 * d;


        if (t1 > 0 && m_minY < p1.y && p1.y < m_maxY) {
            pq.push(SurfaceIntersection(t1, CYLD_BARREL_IDX));
        }
        if (t2 > 0 && m_minY < p2.y && p2.y < m_maxY) {
            pq.push(SurfaceIntersection(t2, CYLD_BARREL_IDX));
        }
    }

    while (!pq.empty() && pq.top().t < 0.f) { pq.pop(); }

    if (pq.empty() || pq.top().t > ray.tMax) {
        return false;
    }

    ray.tMax = pq.top().t;
    vec4 isectP = p + pq.top().t * d;
    vec4 normal;
    vec2 uv;

    switch(pq.top().idx) {
    case CYLD_TPDISK_IDX:
    {
        normal = getNormalTop();
        break;
    }
    case CYLD_BTDISK_IDX:
    {
        normal = getNormalBottom();
        break;
    }
    case CYLD_BARREL_IDX:
    {
        normal = getNormalBarrel(isectP);
        break;
    }
    default:
        return false;
    }

    oSurInteraction = SurfaceInteraction(pq.top().t, normal, uv);
    oSurInteraction.primitive = (BaseRTShape *)this;

    return true;
}
