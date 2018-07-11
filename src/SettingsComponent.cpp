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
    meshIcons = {
             Texture::create().withFile("resources/sphere.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/cube.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/plane.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/teapot.png").withGenerateMipmaps(true).build(),
             Texture::create().withFile("resources/points.png").withGenerateMipmaps(true).build(),
    };
    cameraIcons = {
            Texture::create().withFile("resources/orthogonal.png").withGenerateMipmaps(true).build(),
            Texture::create().withFile("resources/perspective.png").withGenerateMipmaps(true).build()
    };
}

void SettingsComponent::gui() {
    if (ImGui::CollapsingHeader("Mesh")){
        int frame_padding = 2;
        for (int i=0;i<meshIcons.size();i++) {
            auto icon = meshIcons[i];
            ImGui::PushID(i);
            bool selected = i == editor->settings.selectedMesh;
            if (!selected){
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(200,200,200,255));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(100,100,255,255));
            }
            if (ImGui::ImageButton(icon->getNativeTexturePtr(), ImVec2(16,16), ImVec2(0,1), ImVec2(1.0f,0),
                    frame_padding, ImColor(0,0,0,0))){
                editor->settings.selectedMesh = i;
            }

            ImGui::PopStyleColor();

            if (i != meshIcons.size() - 1){
                ImGui::SameLine();
            }
            ImGui::PopID();
        }

    }
    if (ImGui::CollapsingHeader("Camera")){
        ImGui::ColorEdit4("Clear color", &(editor->settings.clearColor.x));
        char* cameraToolTip[2] = {"Orthographic", "Perspective"};
        int frame_padding = 2;
        for (int i=0;i<cameraIcons.size();i++) {
            auto icon = cameraIcons[i];
            ImGui::PushID(i);
            bool selected = i == editor->settings.perspectiveCamera;
            if (!selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor(200, 200, 200, 255));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor(100, 100, 255, 255));
            }
            if (ImGui::ImageButton(icon->getNativeTexturePtr(), ImVec2(16, 16), ImVec2(0, 1), ImVec2(1.0f, 0),
                                   frame_padding, ImColor(0, 0, 0, 0))) {
                editor->settings.perspectiveCamera = i;
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(cameraToolTip[i]);
            }

            ImGui::PopStyleColor();

            if (i != cameraIcons.size() - 1) {
                ImGui::SameLine();
            }
            ImGui::PopID();
        }
    }
}
