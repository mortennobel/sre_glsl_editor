//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#include "GLSLEditor.hpp"
#include <sre/Resource.hpp>
#include <sre/imgui_sre.hpp>
#include "imgui_dock.h"
#include "../submodules/imguiDock/imgui_dock.h"

using namespace sre;

GLSLEditor::GLSLEditor() {
    SDLRenderer r;
    r.setWindowTitle("GLSL Editor");
    r.init();
    r.frameUpdate = [&](float deltaTime){
        update(deltaTime);
    };
    r.frameRender = [&](){
        render();
    };
    r.keyEvent = [&](SDL_Event& key){
        onKey(key);
    };
    init();
    r.startEventLoop();
}

void GLSLEditor::render() {
    auto rp =  RenderPass::create()
            .withCamera(camera)
            .withFramebuffer(framebufferObject)
            .withWorldLights(&worldLights)
            .withClearColor(true,{0,0,0,1})
            .withGUI(false)
            .build();

    rp.draw(mesh, pos1, material);

    rp.finish();

    auto rp2 = RenderPass::create()
            .withCamera(camera)
            .withWorldLights(&worldLights)
            .withClearColor(true,{.98f,.98f,.98f,1})
            .build();

    gui();
}

void GLSLEditor::gui(){
    auto size = Renderer::instance->getWindowSize();
    ImGui::SetNextWindowPos(ImVec2(.0f, .0f), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(size.x+2, size.y+2), ImGuiSetCond_Always);
    bool open = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
    if(ImGui::Begin("Dock Demo", &open, ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove |
                                          ImGuiWindowFlags_NoCollapse|
                                          ImGuiWindowFlags_NoTitleBar
                                          ))
    {


        // dock layout by hard-coded or .ini file
        ImGui::BeginDockspace();

        if(ImGui::BeginDock("GLSL Editor")){
            editShader(shader.get());
        }
        ImGui::EndDock();
        ImGui::SetNextDock( "GLSL Editor", ImGuiDockSlot::ImGuiDockSlot_None);


        if(ImGui::BeginDock("Errors / Warnings")){
            showErrors();
        }
        ImGui::EndDock();
        ImGui::SetNextDock( "Errors / Warnings", ImGuiDockSlot::ImGuiDockSlot_Bottom );

        if(ImGui::BeginDock("Scene")){
            ImVec2 size = ImGui::GetContentRegionAvail();
            if (size.x != sceneTexture->getWidth() || size.y != sceneTexture->getHeight()){
                rebuildFBO(size.x, size.y);
            }
            ImGui_RenderTexture(sceneTexture.get(), {size.x,size.y});
        }
        ImGui::EndDock();
        ImGui::EndDockspace();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void GLSLEditor::rebuildFBO(int width, int height){

    sceneTexture = Texture::create()
            .withRGBAData(nullptr, width, height)
            .withFilterSampling(false)
            .withGenerateMipmaps(false).build();

    framebufferObject = sre::Framebuffer::create().withColorTexture(sceneTexture).build();
}

void GLSLEditor::init() {
    camera.lookAt({0,0,3},{0,0,0},{0,1,0});
    camera.setPerspectiveProjection(60,0.1f,100);

    mesh = Mesh::create()
            .withSphere()
            .build();

    worldLights.addLight(Light::create()
                                 .withDirectionalLight(glm::normalize(glm::vec3(1,1,1)))
                                 .build());


    shaderSources[ShaderType::Vertex] ="standard_blinn_phong_vert.glsl";
    shaderSources[ShaderType::Fragment] ="standard_blinn_phong_frag.glsl";

    shader = Shader::getStandardBlinnPhong();
    material = shader->createMaterial();
    material->setColor({1,1,1,1});
    material->setSpecularity(Color(1,1,1,50));

    rebuildFBO(200, 100);

    ImGui::StyleColorsLight();
    ImGui::InitDock();
}

void updateErrorMarkers(std::vector<std::string>& errors, TextEditor& textEditor, ShaderType type){
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
        if (filter == to_id(type)){
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


void GLSLEditor::showErrors(){
    // Show error messages
    for (int i=0;i<errors.size();i++){
        std::string id = std::string("##_errors_")+std::to_string(i);
        ImGui::LabelText(id.c_str(), errors[i].c_str());
    }

}

void GLSLEditor::editShader(Shader* shader){


    if (shaderRef != shader){
        shaderRef = shader;
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
        errors.clear();
        errorsStr = "";
        textEditor.SetErrorMarkers(TextEditor::ErrorMarkers());

    }

    ImGui::PushItemWidth(-1); // align to right
    int lastSelectedShader = selectedShader;
    bool updatedShader = ImGui::Combo("####ShaderType", &selectedShader, activeShaders.data(), static_cast<int>(activeShaders.size()));
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("CTRL+1, CTRL+2, ...");
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl){
        for (int i=SDLK_1;i<SDLK_9;i++){
            if (ImGui::IsKeyPressed(i)){
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

    if (io.KeyCtrl && ImGui::IsKeyPressed(SDLK_s)){
        compile = true;
    }

    if (compile || updatedShader){
        this->compile();
    }

    if (updatedShader){
        textEditor.SetText(shaderCode[selectedShader]);
        textEditor.SetReadOnly(false);
        updateErrorMarkers(errors,textEditor,shaderTypes[selectedShader]);
    }
    textEditor.Render("##editor");
}

void GLSLEditor::onKey(SDL_Event& key){
    lastKeypress = timeSinceStartup;
}

void GLSLEditor::update(float deltaTime){
    timeSinceStartup += deltaTime;
    if (lastKeypress && timeSinceStartup - lastKeypress > 0.2f){
        compile();
        lastKeypress = 0;
    }
}

void GLSLEditor::compile(){
    auto updatedText = textEditor.GetText(); // get text before updating the editor
    shaderCode[selectedShader] = textEditor.GetText(); // get text before updating the editor

    auto builder = shader->update();
    for (int i=0;i<shaderTypes.size();i++){
        auto filename = shaderSources[shaderTypes[i]];
        Resource::set(filename, shaderCode[i]);
        builder.withSourceResource(filename, shaderTypes[i]);
    }
    errors.clear();
    builder.build(errors);
    errorsStr = "";

    for (auto& err:errors) {
        errorsStr+=err+"\n";
    }
    updateErrorMarkers(errors,textEditor,shaderTypes[selectedShader]);
}
