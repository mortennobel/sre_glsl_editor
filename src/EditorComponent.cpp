#include "EditorComponent.hpp"
#include "GLSLEditor.hpp"
#include "sre/Resource.hpp"

using namespace sre;

EditorComponent::EditorComponent(GLSLEditor *glslEditor, sre::ShaderType shaderType)
:glslEditor(glslEditor), shaderType(shaderType)
{
    titleInternal = std::string("##editor")+std::to_string((int)shaderType);
}

void EditorComponent::gui() {
    if (shaderRef != glslEditor->shader.get()){
        shaderRef = glslEditor->shader.get();
        textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        textEditor.SetText(glslEditor->settings.shaderSource[shaderType]);
        textEditor.SetPalette(TextEditor::GetDarkPalette());
    }

    ImGui::PushItemWidth(-1); // align to right

    /*int lastSelectedShader = selectedShader;
    bool updatedShader = ImGui::Combo("####ShaderType", &selectedShader, activeShaders.data(),
            static_cast<int>(activeShaders.size()));
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("CTRL+1, CTRL+2, ...");

    if (io.KeyCtrl) {
        std::cout << "Ctrl "<< std::endl;
        for (int i=SDLK_1;i<SDLK_9;i++) {
            if (ImGui::IsKeyPressed(i)) {
                selectedShader = i-SDLK_1;
                updatedShader = true;
                std::cout << "selectedShader "<<selectedShader<< std::endl;
            }
        }
    }*/

    //bool updatedPrecompile = ImGui::Checkbox("Show precompiled", &showPrecompiled); ImGui::SameLine();
    //if (updatedPrecompile){
    //    textEditor.SetPalette(showPrecompiled? TextEditor::GetLightPalette():TextEditor::GetDarkPalette());
    //}
    /*bool compile = ImGui::Button("Compile");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("CTRL+S");
*/
    /*if (io.KeyCtrl && ImGui::IsKeyPressed(SDLK_s)) {
        compile = true;
    }*/

    /*if (compile || updatedShader) {
        glslEditor->compileShader();
    }*/

    //if (updatedShader) {
    //    glslEditor->updateErrorMarkers(glslEditor->errors,textEditor, shaderTypes[selectedShader]);
    //}
    textEditor.Render(titleInternal.c_str());
}

void EditorComponent::update(float deltaTime) {

}

void EditorComponent::updateErrorMarkers(std::vector<std::string>& errors){
    TextEditor::ErrorMarkers errorMarkers;
    std::regex e ( "\\d+:(\\d+)", std::regex::ECMAScript);

    std::smatch m;

    for (auto err : errors){
        auto trimmedStr = err;
        auto idx = err.find("##");
        int filter = -1;
        if (idx > 0){
            trimmedStr = err.substr(0,idx);
            auto filterStr = err.substr(idx+2);
            filter = std::stoi(filterStr);
        }
        if (filter == to_id(shaderType)) {
            int line = 0;
            if (std::regex_search (trimmedStr,m,e)) {
                std::string match = m[1];
                line = std::stoi(match);
            }
            errorMarkers.insert(std::pair<int, std::string>(line, trimmedStr));

        }
        std::cout << filter<<" "<<to_id(shaderType)<<trimmedStr<<std::endl;
    }
    textEditor.SetErrorMarkers(errorMarkers);
}
