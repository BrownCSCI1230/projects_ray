#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

using namespace std;

bool SceneParser::parse(std::string filepath, SceneMetaData &oMetaData) {
    shared_ptr<ScenefileReader> fileReader = make_shared<ScenefileReader>(filepath);
    bool success = fileReader->parseXML();
    if (!success) {
        return false;
    }

    /* TA SOLUTION BEGIN */

    // load the scene with the data in the parser
    oMetaData.lights.clear();
    oMetaData.shapes.clear();

    fileReader->getCameraData(oMetaData.cameraData);
    fileReader->getGlobalData(oMetaData.globalData);

    int numLights = fileReader->getNumLights();
    oMetaData.lights.reserve(numLights);

    for (int i = 0; i < numLights; i++) {
        SceneLightData lightData;
        fileReader->getLightData(i, lightData);
        oMetaData.lights.emplace_back(lightData);
    }

    SceneNode *root = fileReader->getRootNode();
    glm::mat4 matrix(1.0f);

    auto startTS = std::chrono::system_clock::now();
    std::cout << std::endl << "Begin loading scene " << filepath << std::endl;

    dfsParseSceneNode(oMetaData, root, matrix);

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS - startTS;
    std::cout << "Complete loading scene. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;

    /* TA SOLUTION END */

    return true;
}

/* TA SOLUTION BEGIN */

void SceneParser::dfsParseSceneNode(SceneMetaData &oMetaData, SceneNode *node, glm::mat4 matrix) {
    if (node == nullptr) {
        return;
    }

    for (auto transform : node->transformations) {
        glm::mat4 transMatrix;
        switch (transform->type) {
        case TRANSFORMATION_TRANSLATE:
        {
            transMatrix = glm::translate(transform->translate);
            break;
        }
        case TRANSFORMATION_SCALE:
        {
            transMatrix = glm::scale(transform->scale);
            break;
        }
        case TRANSFORMATION_ROTATE:
        {
            transMatrix = glm::rotate(transform->angle, transform->rotate);
            break;
        }
        case TRANSFORMATION_MATRIX:
        {
            transMatrix = transform->matrix;
            break;
        }
        default:
            ;
        }
        matrix = matrix * transMatrix;
    }

    for (auto primitive : node->primitives) {
        SceneShapeData shape = {.primitive = *primitive, .ctm = matrix};
        oMetaData.shapes.emplace_back(shape);
    }

    for (SceneNode * child : node->children) {
        dfsParseSceneNode(oMetaData, child, matrix);
    }
    return;
}

/* TA SOLUTION BEGIN */
