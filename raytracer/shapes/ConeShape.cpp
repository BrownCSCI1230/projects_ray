#include "ConeShape.h"
#include <queue>

using namespace std;
using namespace glm;

static const float SIN_26 = 1.f / sqrt(5);
static const float COS_26 = 2.f / sqrt(5);

#define CONE_DISK_IDX 0
#define CONE_HAT_IDX 1

ConeRTShape::ConeRTShape(SceneMaterial material) :
    BaseRTShape(material),
    m_R(0.5f)
{

}

glm::vec4 ConeRTShape::getNormalDisk() const {
    vec4 normal = vec4(0.f, -1.f, 0.f, 0.f);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

glm::vec4 ConeRTShape::getNormalHat(const glm::vec4 &intersection) const {
    float r = 2 * SIN_26;
    float temp = glm::sqrt(intersection.x * intersection.x + intersection.z * intersection.z);
    float cosTheta = intersection.x / temp;
    float sinTheta = intersection.z / temp;

    vec4 normal = vec4(r * cosTheta, SIN_26, r * sinTheta, 0);
    normal = m_ICTM_T * normal;
    normal[3] = 0.f;
    return glm::normalize(normal);
}

glm::vec2 ConeRTShape::getUVDisk(const glm::vec4 &intersection) const {
    float u, v;
    u = intersection.x + m_R;
    v = m_R - intersection.z;
    return vec2(u, v);
}

glm::vec2 ConeRTShape::getUVHat(const glm::vec4 &intersection) const {
    float u, v;
    v = m_R - intersection.y;
    if (v <= 0.f) {
        // if oV is less than or equal to 0, than it's sigularity for u,
        // so we choose a fixed u value: 0.5
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

bool ConeRTShape::intersect(const Ray &ray, SurfaceInteraction &oSurInteraction) const {
    vec4 p = m_ICTM * ray.origin, d = m_ICTM * ray.direction;

    priority_queue<SurfaceIntersection, vector<SurfaceIntersection>, ShapePQCompare> pq;

    // check bottom cap intersection
    if (d.y != 0) {
        float t = (-m_R - p.y) / d.y;

        vec4 potentialPoint = p + t * d;
        if (t > 0.f && potentialPoint.x * potentialPoint.x + potentialPoint.z * potentialPoint.z <= m_R * m_R) {
            pq.push(SurfaceIntersection(t, CONE_DISK_IDX));
        }
    }

    // check hat intersection
    float a = d.x * d.x + d.z * d.z - 0.25 * d.y * d.y;
    float b = 2 * d.x * p.x + 2 * d.z * p.z - 0.5 * d.y * p.y + 0.5 * m_R * d.y;
    float c = p.x * p.x + p.z * p.z - 0.25 * p.y * p.y + 0.5 * m_R * p.y - 0.25 * m_R * m_R;

    float square = b * b - 4 * a * c;
    if (a != 0 && square > 0) {
        float root = glm::sqrt(square);
        float t1 = (-b + root) / (2 * a);
        float t2 = (-b - root) / (2 * a);

        vec4 p1 = p + t1 * d;
        vec4 p2 = p + t2 * d;

        if (t1 > 0 && -m_R < p1.y && p1.y < m_R) {
            pq.push(SurfaceIntersection(t1, CONE_HAT_IDX));
        }

        if (t2 > 0 && -m_R < p2.y && p2.y < m_R) {
            pq.push(SurfaceIntersection(t2, CONE_HAT_IDX));
        }
    }

    if (pq.empty() || pq.top().t > ray.tMax) {
        return false;
    }

    ray.tMax = pq.top().t;
    vec4 isectP = p + pq.top().t * d;
    vec4 normal;
    vec2 uv;

    switch (pq.top().idx) {
    case CONE_DISK_IDX:
    {
        normal = getNormalDisk();
        uv = getUVDisk(isectP);
        break;
    }
    case CONE_HAT_IDX:
    {
        normal = getNormalHat(isectP);
        uv = getUVHat(isectP);
        break;
    }
    default:
        return false;
    }

    oSurInteraction = SurfaceInteraction(pq.top().t, normal, uv);
    oSurInteraction.primitive = (BaseRTShape *)this;

    return true;
}
