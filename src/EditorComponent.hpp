#pragma once
#include "TextEditor.h"
#include "sre/Shader.hpp"

class GLSLEditor;

class EditorComponent {
public:
    EditorComponent(GLSLEditor* glslEditor, sre::ShaderType shaderType, std::string& code);
    void gui();
    void setText(std::string& code);
    void update(float deltaTime);
    void updateErrorMarkers(std::vector<std::string>& errors);
private:
    std::string titleInternal;
    GLSLEditor* glslEditor;
    TextEditor textEditor;
    int selectedShader = 0;
    sre::ShaderType shaderType;
    sre::Shader* shaderRef = nullptr;
    std::vector<const char*> activeShaders;
    std::vector<sre::ShaderType> shaderTypes;
    std::map<sre::ShaderType, std::string> shaderSources;

    friend class GLSLEditor;
};


