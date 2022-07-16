#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <memory>
#include <unordered_map>
#include <QImage>

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    std::shared_ptr<QImage> getTextureImage(std::string fileName);

private:
    std::unordered_map<std::string, std::weak_ptr<QImage>> m_textures;
};

#endif // TEXTUREMANAGER_H
