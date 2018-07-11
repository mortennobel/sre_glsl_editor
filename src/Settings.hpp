#pragma once

#include <map>
#include <string>
#include "sre/Shader.hpp"

struct UniformTypeValue {
    sre::UniformType uniformType;
    union {
        glm::vec3 vec3Value;
        glm::vec4 vec4Value;
        glm::mat4 mat3Value;
        float floatValue;
        int textureId;
    } value;
};

struct Settings {
    int selectedMesh = 0;
    bool perspectiveCamera = true;
    glm::vec2 rotateCamera = glm::vec2{0.0};
    glm::vec4 clearColor = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    std::map<std::string, UniformTypeValue> uniforms;
    std::map<sre::ShaderType, std::string> shaderSource;
    std::shared_ptr<sre::Material> material;
};
