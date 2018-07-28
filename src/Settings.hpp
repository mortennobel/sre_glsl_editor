#pragma once

#include <map>
#include <string>
#include "sre/Shader.hpp"
#include "sre/Texture.hpp"
#define PICOJSON_USE_INT64 1
#include "picojson.h"

class GLSLEditor; //fwd decl

enum class Clear {
    Color,
    ProceduralSky,
    Skybox
};

struct Settings {
    // serialized
    int selectedMesh = 0;
    bool perspectiveCamera = true;
    glm::vec2 rotateCamera = glm::vec2{0.0};
    Clear clearType = Clear::Color;
    glm::vec4 clearColor = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    std::shared_ptr<sre::Texture> skybox;
    std::map<sre::ShaderType, std::string> shaderSource;
    std::shared_ptr<sre::Material> material;
    std::map<sre::ShaderType, std::string> filenames;

    // not serialized
    std::string filepath;
    GLSLEditor * editor = nullptr;

    static void load(std::string filepath, GLSLEditor * editor);
    void save(std::string filepath);
private:
    picojson::value materialUniformToJson();
    picojson::value fileNamesToJSON(std::string filepath);

    static void parseUniform(picojson::value val, sre::Material* mat, GLSLEditor * editor);

};
