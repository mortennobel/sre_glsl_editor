#include "EditorComponent.hpp"
#include "GLSLEditor.hpp"
#include "sre/Resource.hpp"

using namespace sre;

EditorComponent::EditorComponent(GLSLEditor *glslEditor)
:glslEditor(glslEditor)
{
}

void EditorComponent::gui() {
    if (shaderRef != glslEditor->shader.get()){
        shaderRef = glslEditor->shader.get();
        shaderCode.clear();
        activeShaders.clear();
        shaderTypes.clear();
        for (auto source : shaderSources){
            auto source_ = Resource::loadText(source.second);
            shaderCode.emplace_back(source_);
            shaderTypes.push_back(source.first);
            switch (source.first){
                case ShaderType::Vertex:
                    activeShaders.push_back("Vertex");
                    break;
                case ShaderType::Fragment:
                    activeShaders.push_back("Fragment");
                    break;
                case ShaderType::Geometry:
                    activeShaders.push_back("Geometry");
                    break;
                case ShaderType::TessellationControl:
                    activeShaders.push_back("TessellationControl");
                    break;
                case ShaderType::TessellationEvaluation:
                    activeShaders.push_back("TessellationEvaluation");
                    break;
                case ShaderType::NumberOfShaderTypes:
                    LOG_ERROR("ShaderType::NumberOfShaderTypes should never be used");
                    break;
                default:
                    LOG_ERROR("Unhandled shader");
                    break;
            }
        }
        selectedShader = 0;
        textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        textEditor.SetText(shaderCode[selectedShader]);
        textEditor.SetPalette(TextEditor::GetDarkPalette());
        glslEditor->errors.clear();
        glslEditor->errorsStr = "";
        textEditor.SetErrorMarkers(TextEditor::ErrorMarkers());

    }

    ImGui::PushItemWidth(-1); // align to right
    int lastSelectedShader = selectedShader;
    bool updatedShader = ImGui::Combo("####ShaderType", &selectedShader, activeShaders.data(),
            static_cast<int>(activeShaders.size()));
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("CTRL+1, CTRL+2, ...");
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl) {
        for (int i=SDLK_1;i<SDLK_9;i++) {
            if (ImGui::IsKeyPressed(i)) {
                selectedShader = i-SDLK_1;
                updatedShader = true;
            }
        }
    }
    selectedShader = std::min(selectedShader, (int)activeShaders.size());

    if (updatedShader) {
        auto updatedText = textEditor.GetText(); // get text before updating the editor
        shaderCode[lastSelectedShader] = textEditor.GetText(); // get text before updating the editor
    }

    //bool updatedPrecompile = ImGui::Checkbox("Show precompiled", &showPrecompiled); ImGui::SameLine();
    //if (updatedPrecompile){
    //    textEditor.SetPalette(showPrecompiled? TextEditor::GetLightPalette():TextEditor::GetDarkPalette());
    //}
    bool compile = ImGui::Button("Compile");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("CTRL+S");

    if (io.KeyCtrl && ImGui::IsKeyPressed(SDLK_s)) {
        compile = true;
    }

    if (compile || updatedShader) {
        this->compile();
    }

    if (updatedShader) {
        textEditor.SetText(shaderCode[selectedShader]);
        textEditor.SetReadOnly(false);
        glslEditor->updateErrorMarkers(glslEditor->errors,textEditor, shaderTypes[selectedShader]);
    }
    textEditor.Render("##editor");
}

void EditorComponent::update(float deltaTime) {

}

void EditorComponent::compile() {
    auto updatedText = textEditor.GetText(); // get text before updating the editor
    shaderCode[selectedShader] = textEditor.GetText(); // get text before updating the editor

    auto builder = shaderRef->update();
    for (int i=0;i<shaderTypes.size();i++){
        auto filename = shaderSources[shaderTypes[i]];
        Resource::set(filename, shaderCode[i]);
        builder.withSourceResource(filename, shaderTypes[i]);
    }
    glslEditor->errors.clear();
    builder.build(glslEditor->errors);
    glslEditor->errorsStr = "";

    for (auto& err:glslEditor->errors) {
        glslEditor->errorsStr+=err+"\n";
    }
    glslEditor->updateErrorMarkers(glslEditor->errors,textEditor,shaderTypes[selectedShader]);
}
