#include "BaseShape.h"
#include <QString>
#include <iostream>

using namespace std;
using namespace glm;

static vec4 toVec4Color(QColor &color) {
    return vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

BaseRTShape::BaseRTShape(SceneMaterial material) :
    m_CTM(glm::mat4(1.0f)),
    m_ICTM(glm::mat4(1.0f)),
    m_material(material)
{

}

BaseRTShape::~BaseRTShape() {

}

void BaseRTShape::setCTM(const glm::mat4 &trans) {
    m_CTM = trans;
    m_ICTM = glm::inverse(trans);
    m_ICTM_T = glm::transpose(m_ICTM);
    return;
}

SceneMaterial BaseRTShape::getMaterial() const {
    return m_material;
}
