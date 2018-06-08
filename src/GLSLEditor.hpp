//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#pragma once
#include "sre/SDLRenderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GLSLEditor {
public:
    GLSLEditor();
    void init();
    void render();
    void gui();
private:
    void editShader(sre::Shader* shader);
    sre::Camera camera;
    sre::WorldLights worldLights;
    std::shared_ptr<sre::Mesh> mesh;
    std::shared_ptr<sre::Shader> shader;
    std::shared_ptr<sre::Material> material;

    std::map<sre::ShaderType, std::string> shaderSources;

    glm::mat4 pos1 = glm::translate(glm::mat4(1), {-1,0,0});
    glm::mat4 pos2 = glm::translate(glm::mat4(1), {1,0,0});
};


