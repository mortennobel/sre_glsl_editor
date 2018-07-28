#include "EditorComponent.hpp"
#include "GLSLEditor.hpp"
#include "sre/Resource.hpp"

using namespace sre;

EditorComponent::EditorComponent(GLSLEditor *glslEditor, sre::ShaderType shaderType, std::string& code)
:glslEditor(glslEditor), shaderType(shaderType)
{
    titleInternal = std::string("##editor")+std::to_string((int)shaderType);
    setText(code);
}

void EditorComponent::setText(std::string& code){
    textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
    textEditor.SetText(code);
    static TextEditor::Palette p = { {
                                 0xffffffff,	// None
                                 0xffd69c56,	// Keyword
                                 0xff00ff00,	// Number
                                 0xff7070e0,	// String
                                 0xff70a0e0, // Char literal
                                 0xffffffff, // Punctuation
                                 0xff409090,	// Preprocessor
                                 0xffaaaaaa, // Identifier
                                 0xff9bc64d, // Known identifier
                                 0xffc040a0, // Preproc identifier
                                 0xff206020, // Comment (single line)
                                 0xff406020, // Comment (multi line)
                                 0xff101010, // Background
                                 0xffe0e0e0, // Cursor
                                 0x80a06020, // Selection
                                 0x800020ff, // ErrorMarker
                                 0x40f08000, // Breakpoint
                                 0xff707000, // Line number
                                 0x40000000, // Current line fill
                                 0x40808080, // Current line fill (inactive)
                                 0x40a0a0a0, // Current line edge
                                 0xff0000ff, // ErrorMarkerTitleColor
                                 0xff000000, // ErrorMarkerBodyColor
                         } };
    textEditor.SetPalette(p);
}

void EditorComponent::gui() {
    ImGui::PushItemWidth(-1); // align to right
    textEditor.Render(titleInternal.c_str());
}

void EditorComponent::update(float deltaTime) {

}

void EditorComponent::updateErrorMarkers(std::vector<std::string>& errors){
    TextEditor::ErrorMarkers errorMarkers;
    std::regex e ( "\\d+:(\\d+)", std::regex::ECMAScript);

    std::smatch m;

    for (auto & err : errors){
        auto trimmedStr = err;
        auto idx = err.find("##");
        int filter = -1;
        if (idx > 0){
            trimmedStr = err.substr(0,idx);
            auto filterStr = err.substr(idx+2);
            try{
                filter = std::stoi(filterStr);
            } catch (...){
                filter = -1;
            }
        }
        if (filter == to_id(shaderType)) {
            int line = 0;
            if (std::regex_search (trimmedStr,m,e)) {
                std::string match = m[1];
                line = std::stoi(match);
            }
            errorMarkers.insert(std::pair<int, std::string>(line, trimmedStr));
        }
    }
    textEditor.SetErrorMarkers(errorMarkers);
}
