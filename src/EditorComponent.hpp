#pragma once
#include "TextEditor.h"
#include "sre/Shader.hpp"

class GLSLEditor;

class EditorComponent {
public:
    explicit EditorComponent(GLSLEditor* glslEditor);
    void gui();
    void update(float deltaTime);
    void compile();
private:
    GLSLEditor* glslEditor;
    TextEditor textEditor;
    int selectedShader = 0;
    sre::Shader* shaderRef = nullptr;
    std::vector<std::string> shaderCode;
    std::vector<const char*> activeShaders;
    std::vector<sre::ShaderType> shaderTypes;
    std::map<sre::ShaderType, std::string> shaderSources;

    friend class GLSLEditor;
};


