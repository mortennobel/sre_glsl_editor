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
    std::map<std::string, UniformTypeValue> uniforms;
    std::shared_ptr<sre::Material> material;
};
