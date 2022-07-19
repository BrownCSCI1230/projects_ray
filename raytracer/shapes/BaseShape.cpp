#include "BaseShape.h"
#include <QString>
#include <iostream>
#include "raytracer/utils/TextureManager.h"

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

bool BaseRTShape::getTexture(glm::vec2 uv, glm::vec4 &oColor) const {
    if (m_textureImage == nullptr || m_textureImage->width() == 0 || m_textureImage->height() == 0) {
        return false;
    }

    // clamp the value between [0, 1]
    float u = glm::clamp(uv[0], 0.f, 1.f);
    float v = glm::clamp(uv[1], 0.f, 1.f);

    float x = u * m_material.textureMap.repeatU * m_textureImage->width();
    float y = v * m_material.textureMap.repeatV * m_textureImage->height();

    while (x > m_textureImage->width()) { x -= m_textureImage->width(); }
    while (y > m_textureImage->height()) { y -= m_textureImage->height(); }

    int xLow = floor(x), xHigh = xLow + 1;
    int yLow = floor(y), yHigh = yLow + 1;

    int xLowIdx = xLow % m_textureImage->width();
    int xHighIdx = xHigh % m_textureImage->width();
    int yLowIdx = yLow % m_textureImage->height();
    int yHighIdx = yHigh % m_textureImage->height();

    QColor color11 = m_textureImage->pixelColor(xLowIdx, yLowIdx);
    QColor color12 = m_textureImage->pixelColor(xLowIdx, yHighIdx);
    QColor color21 = m_textureImage->pixelColor(xHighIdx, yLowIdx);
    QColor color22 = m_textureImage->pixelColor(xHighIdx, yHighIdx);

    vec4 c11 = toVec4Color(color11);
    vec4 c12 = toVec4Color(color12);
    vec4 c21 = toVec4Color(color21);
    vec4 c22 = toVec4Color(color22);

    vec4 f1 = (xHigh - x) * c11 + (x - xLow) * c21;
    vec4 f2 = (xHigh - x) * c12 + (x - xLow) * c22;

    vec4 fColor = (yHigh - y) * f1 + (y - yLow) * f2;

    oColor = fColor;
    return true;
}

Bound BaseRTShape::getWorldBound() const {
    Bound b = getObjectBound();
    Bound ret;
    ret.pMin = m_CTM * b.pMin;
    ret.pMax = ret.pMin;

    ret.unite(m_CTM * vec4(b.pMax.x, b.pMin.y, b.pMin.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMin.x, b.pMax.y, b.pMin.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMin.x, b.pMin.y, b.pMax.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMin.x, b.pMax.y, b.pMax.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMax.x, b.pMax.y, b.pMin.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMax.x, b.pMin.y, b.pMax.z, 1.f));
    ret.unite(m_CTM * vec4(b.pMax.x, b.pMax.y, b.pMax.z, 1.f));
    return ret;
}

bool BaseRTShape::loadTexture(shared_ptr<TextureManager> textureManager) {
    if (!m_material.textureMap.isUsed) {
        return true;
    }

    if (textureManager == nullptr) {
        return false;
    }

    m_textureImage = textureManager->getTextureImage(m_material.textureMap.filename);
    if (!m_textureImage || m_textureImage->isNull()) {
        cout << "load texture image fail:" << m_material.textureMap.filename << endl;
        return false;
    }
    return true;
}
