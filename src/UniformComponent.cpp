//
// Created by Morten Nobel-Jørgensen on 7/6/18.
//

#include "UniformComponent.hpp"
#include <vector>
#include "imgui.h"
#include "GLSLEditor.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
using namespace sre;

namespace {
    bool showMatrix(const char *label, glm::mat4& matrix) {
        bool changed = false;
        matrix = glm::transpose(matrix);
        changed |= ImGui::InputFloat4(label, glm::value_ptr(matrix[0]));
        changed |= ImGui::InputFloat4("", glm::value_ptr(matrix[1]));
        changed |= ImGui::InputFloat4("", glm::value_ptr(matrix[2]));
        changed |= ImGui::InputFloat4("", glm::value_ptr(matrix[3]));
        matrix = glm::transpose(matrix);
        ImGui::Spacing();
        return changed;
    }

    bool showMatrix(const char *label, glm::mat3& matrix) {
        matrix = glm::transpose(matrix);
        bool changed = false;
        changed |= ImGui::InputFloat3(label, glm::value_ptr(matrix[0]));
        changed |= ImGui::InputFloat3("", glm::value_ptr(matrix[1]));
        changed |= ImGui::InputFloat3("", glm::value_ptr(matrix[2]));
        matrix = glm::transpose(matrix);
        ImGui::Spacing();
        return changed;
    }
}

UniformComponent::UniformComponent(GLSLEditor *editor)
:editor(editor) {
}

std::string glUniformToString(UniformType type) {
    switch (type){
        case UniformType::Float:
            return "float";
        case UniformType::Int:
            return "int";
        case UniformType::Mat3Array:
            return "mat3";
        case UniformType::Mat4:
        case UniformType::Mat4Array:
            return "mat4";
        case UniformType::Texture:
            return "texture";
        case UniformType::TextureCube:
            return "texture cube";
        case UniformType::Vec3:
            return "vec3";
        case UniformType::Vec4:
            return "vec4";
        case UniformType::IVec4:
            return "ivec4";
        case UniformType::Invalid:
            return "Unsupported";
    }
    return "Unknown";
}

void UniformComponent::gui() {
    for (auto name : editor->shader->getUniformNames()){
        auto uniform = editor->shader->getUniform(name);
        auto uniformType = glUniformToString(uniform.type);
        auto uniformHeader = name + " (" + uniformType + ")";
        ImGui::PushID(uniform.id);
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);

        auto uniformIter = editor->settings.uniforms.find(name);
        UniformTypeValue* value;
        if (uniformIter == editor->settings.uniforms.end()){
            editor->settings.uniforms[name] = {};
            value = &(editor->settings.uniforms[name]);
            value->uniformType = uniform.type;
        } else {
            value = &(uniformIter->second);

        }
        auto material = editor->settings.material;
        switch (uniform.type){
            case UniformType::Float: {
                float value = material->get<float>(name);
                bool changed = ImGui::InputFloat(name.c_str(),&value);
                if (changed){
                    material->set(name, value);
                }
            }
                break;
            case UniformType::Vec4: {
                glm::vec4 value4 = material->get<glm::vec4>(name);
                bool changed = ImGui::InputFloat4(name.c_str(),&value4.x);
                if (changed){
                    material->set(name, value4);
                }
            }
                break;
            case UniformType::Texture:{
                std::shared_ptr<Texture> valueTex = material->get<std::shared_ptr<Texture>>(name);
                int selected = 0;
                std::vector<const char*> elements;
                for (int i=0;i<editor->textures.size();i++){
                    elements.push_back(editor->textures[i]->getName().c_str());
                    if (editor->textures[i] == valueTex)
                        selected = i;
                }
                bool changed = ImGui::Combo(name.c_str(), &selected, elements.data(), elements.size());
                if (changed){
                    material->set(name, editor->textures[selected]);
                }
            }
                break;
            case UniformType::TextureCube:{
                std::shared_ptr<Texture> valueTex = material->get<std::shared_ptr<Texture>>(name);
                int selected = 0;
                std::vector<const char*> elements;
                for (int i=0;i<editor->cubeTextures.size();i++){
                    elements.push_back(editor->textures[i]->getName().c_str());
                    if (editor->textures[i] == valueTex)
                        selected = i;
                }
                bool changed = ImGui::Combo(name.c_str(), &selected, elements.data(), elements.size());
                if (changed){
                    material->set(name, editor->cubeTextures[selected]);
                }
            }
                break;
                /*
                case UniformType::Mat3Array:

                    if (ImGui::TreeNode(name.c_str(), "Mat3Array")){
                        auto values = editor->material->get<std::shared_ptr<std::vector<glm::mat3>>>(name);
                        for (int i=0;i<values->size();i++){
                            sprintf(res,"%i",i);
                            showMatrix(res,(*values)[i]);
                        }
                        ImGui::TreePop();
                    }
                break;
            case UniformType::Mat4Array:
                if (ImGui::TreeNode(name.c_str(), "Mat4Array")){
                    auto values = material->get<std::shared_ptr<std::vector<glm::mat4>>>(name);

                    for (int i=0;i<values->size();i++){
                        sprintf(res,"%i",i);
                        showMatrix(res,(*values)[i]);
                    }
                    ImGui::TreePop();
                }
                break;*/
            case UniformType::Mat4:{
                if (ImGui::TreeNode(name.c_str(), "Mat4")){
                    auto value = material->get<glm::mat4>(name);
                    bool changed = showMatrix("",value);
                    if (changed) {
                        material->set(name, value);
                    }
                    ImGui::TreePop();
                }
            }
                break;
            default:
                LOG_ERROR("Unexpected error type %i", (int)uniform.type);
        }

        ImGui::PopID();
    }

}
