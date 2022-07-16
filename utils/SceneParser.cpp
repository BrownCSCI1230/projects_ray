#include "SceneParser.h"
#include "ScenefileReader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

using namespace std;

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    shared_ptr<ScenefileReader> fileReader = make_shared<ScenefileReader>(filepath);
    bool success = fileReader->readXML();
    if (!success) {
        return false;
    }

    /* TA SOLUTION BEGIN */

    // load the scene with the data in the parser
    renderData.lights.clear();
    renderData.shapes.clear();

    fileReader->getCameraData(renderData.cameraData);
    fileReader->getGlobalData(renderData.globalData);

    int numLights = fileReader->getNumLights();
    renderData.lights.reserve(numLights);

    for (int i = 0; i < numLights; i++) {
        SceneLightData lightData;
        fileReader->getLightData(i, lightData);
        renderData.lights.emplace_back(lightData);
    }

    SceneNode *root = fileReader->getRootNode();
    glm::mat4 matrix(1.0f);

    auto startTS = std::chrono::system_clock::now();
    std::cout << std::endl << "Begin loading scene " << filepath << std::endl;

    dfsParseSceneNode(renderData, root, matrix);

    auto endTS = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTS - startTS;
    std::cout << "Complete loading scene. Time elapse: " << elapsed_seconds.count() * 1000.f << "ms" << std::endl;

    /* TA SOLUTION END */

    return true;
}

/* TA SOLUTION BEGIN */

void SceneParser::dfsParseSceneNode(RenderData &renderData, SceneNode *node, glm::mat4 matrix) {
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
        RenderShapeData shape = {.primitive = *primitive, .ctm = matrix};
        renderData.shapes.emplace_back(shape);
    }

    for (SceneNode * child : node->children) {
        dfsParseSceneNode(renderData, child, matrix);
    }
    return;
}

/* TA SOLUTION BEGIN */
