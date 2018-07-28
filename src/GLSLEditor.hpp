//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#pragma once

#include <string>
#include "sre/SDLRenderer.hpp"
#include "sre/Framebuffer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "EditorComponent.hpp"
#include "SettingsComponent.hpp"
#include "UniformComponent.hpp"
#include "Settings.hpp"


class GLSLEditor {
public:
    GLSLEditor();
    void compileShader();
    void renderScene();
private:
    void init();
    void render();
    void gui();
    void guiMenu();
    void newProject();
    void loadProject();
    void saveProject();
    void saveAsProject();
    void setProject(Settings& settings);
    void setShader(Settings& settings);
    void update(float deltaTime);

    float timeSinceStartup = 0;
    float lastKeypress = 0;
    void onKey(SDL_Event& key);
    void rebuildFBO(int width, int height);
    void showErrors();

    bool mousePressed = false;
    glm::ivec2 mousePos = glm::ivec2(0, 0);
    glm::ivec2 mouseDelta = glm::ivec2(0, 0);

    sre::Camera camera;
    sre::WorldLights worldLights;
    std::vector<std::shared_ptr<sre::Mesh>> meshes;
    std::vector<std::shared_ptr<sre::Texture>> textures;
    std::vector<std::shared_ptr<sre::Texture>> cubeTextures;
    std::shared_ptr<sre::Shader> shader;

    std::shared_ptr<sre::Texture> sceneTexture;
    std::shared_ptr<sre::Framebuffer> framebufferObject;
    std::map<sre::ShaderType, std::shared_ptr<EditorComponent>> editorComponents;
    SettingsComponent settingsComponent;
    UniformComponent uniformComponent;
    Settings settings;

    glm::mat4 pos1 = glm::translate(glm::mat4(1), {0,0,0});

    std::vector<std::string> errors;

    friend class EditorComponent;
    friend class SettingsComponent;
    friend class Settings;
    friend class UniformComponent;
};


