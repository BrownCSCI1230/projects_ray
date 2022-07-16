#include "TextureManager.h"
#include <iostream>

using namespace std;

TextureManager::TextureManager() :
    m_textures(unordered_map<string, weak_ptr<QImage>>())
{

}

TextureManager::~TextureManager() {
    cout << "TEXTUREMANAGER DESTRUCTOR" << endl;
}

std::shared_ptr<QImage> TextureManager::getTextureImage(std::string fileName) {
    if (m_textures.count(fileName) && !m_textures[fileName].expired()) {
        return m_textures[fileName].lock();
    }


    QString qFileName = QString(fileName.c_str());
    shared_ptr<QImage> ret = make_shared<QImage>(qFileName);
    if (ret == nullptr || ret->isNull()) {
        return nullptr;
    }

    weak_ptr<QImage> weakRef(ret);
    m_textures[fileName] = weakRef;
    return ret;
}
