//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#pragma once
#include "sre/SDLRenderer.hpp"
#include "sre/Framebuffer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextEditor.h"

class GLSLEditor {
public:
    GLSLEditor();
private:
    void init();
    void render();
    void gui();
    void update(float deltaTime);
    void compile();
    float timeSinceStartup = 0;
    float lastKeypress = 0;
    void onKey(SDL_Event& key);
    void rebuildFBO(int width, int height);
    void showErrors();
    void editShader(sre::Shader* shader);
    sre::Camera camera;
    sre::WorldLights worldLights;
    std::shared_ptr<sre::Mesh> mesh;
    std::shared_ptr<sre::Shader> shader;
    std::shared_ptr<sre::Material> material;
    std::shared_ptr<sre::Texture> sceneTexture;
    std::shared_ptr<sre::Framebuffer> framebufferObject;

    std::map<sre::ShaderType, std::string> shaderSources;

    glm::mat4 pos1 = glm::translate(glm::mat4(1), {0,0,0});

    std::vector<std::string> errors;

    sre::Shader* shaderRef = nullptr;
    std::vector<std::string> shaderCode;
    std::string errorsStr;
    TextEditor textEditor;
    int selectedShader = 0;
    std::vector<const char*> activeShaders;
    std::vector<sre::ShaderType> shaderTypes;
};


