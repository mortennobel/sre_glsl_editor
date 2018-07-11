//
// Created by Morten Nobel-Jørgensen on 7/4/18.
//

#pragma once

#include <memory>
#include <vector>
#include <sre/Texture.hpp>

class GLSLEditor;

class SettingsComponent {
public:
    explicit SettingsComponent(GLSLEditor* editor);
    void init();
    void gui();
private:
    GLSLEditor* editor;
    std::vector<std::shared_ptr<sre::Texture>> meshIcons;
    std::vector<std::shared_ptr<sre::Texture>> cameraIcons;
};


