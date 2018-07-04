//
// Created by Morten Nobel-Jørgensen on 7/4/18.
//

#include <imgui.h>
#include <iostream>
#include "SettingsComponent.hpp"
#include "GLSLEditor.hpp"

using namespace sre;

SettingsComponent::SettingsComponent(GLSLEditor *editor)
:editor(editor)
{

}

void SettingsComponent::init() {
    icons = {
             Texture::create().withFile("resources/sphere.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/cube.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/plane.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/teapot.png").withGenerateMipmaps(true).build(),
    };
}

void SettingsComponent::gui() {
    if (ImGui::CollapsingHeader("Mesh")){

        int frame_padding = 2;
        for (int i=0;i<icons.size();i++) {
            auto icon = icons[i];
            ImGui::PushID(i);
            bool selected = i == editor->settings.selectedMesh;
            if (!selected){
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(200,200,200,255));
            }
            if (ImGui::ImageButton(icon->getNativeTexturePtr(), ImVec2(16,16), ImVec2(0,1), ImVec2(1.0f,0), frame_padding, ImColor(0,0,0,0))){
                editor->settings.selectedMesh = i;
            }
            if (!selected){
                ImGui::PopStyleColor();
            }
            if (i != icons.size() - 1){
                ImGui::SameLine();
            }
            ImGui::PopID();
        }

    }
    if (ImGui::CollapsingHeader("Camera")){

    }
}
