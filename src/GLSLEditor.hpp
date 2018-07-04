//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#pragma once
#include "sre/SDLRenderer.hpp"
#include "sre/Framebuffer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "EditorComponent.hpp"
#include "SettingsComponent.hpp"
#include "Settings.hpp"


class GLSLEditor {
public:
    GLSLEditor();
private:
    void init();
    void render();
    void gui();
    void update(float deltaTime);

    float timeSinceStartup = 0;
    float lastKeypress = 0;
    void onKey(SDL_Event& key);
    void rebuildFBO(int width, int height);
    void showErrors();

    void updateErrorMarkers(std::vector<std::string>& errors, TextEditor& textEditor, sre::ShaderType type);
    sre::Camera camera;
    sre::WorldLights worldLights;
    std::vector<std::shared_ptr<sre::Mesh>> meshes;
    std::shared_ptr<sre::Shader> shader;
    std::shared_ptr<sre::Material> material;
    std::shared_ptr<sre::Texture> sceneTexture;
    std::shared_ptr<sre::Framebuffer> framebufferObject;
    EditorComponent editorComponent;
    SettingsComponent settingsComponent;
    Settings settings;

    glm::mat4 pos1 = glm::translate(glm::mat4(1), {0,0,0});

    std::vector<std::string> errors;

    std::string errorsStr;

    friend class EditorComponent;
    friend class SettingsComponent;
};


