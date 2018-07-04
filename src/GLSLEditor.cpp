//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#include "GLSLEditor.hpp"
#include <sre/Resource.hpp>
#include <sre/imgui_sre.hpp>
#include "imgui_dock.h"
#include "../submodules/imguiDock/imgui_dock.h"

using namespace sre;

GLSLEditor::GLSLEditor()
:editorComponent(this), settingsComponent(this)
{
    SDLRenderer r;
    r.setWindowTitle("GLSL Editor");
    r.init();
    settingsComponent.init();
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

    rp.draw(meshes[settings.selectedMesh], pos1, material);

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
            editorComponent.gui();
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
        if(ImGui::BeginDock("Settings")){
            settingsComponent.gui();
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

    meshes = {
        Mesh::create()
                .withSphere()
                .build(),
        Mesh::create()
                .withCube()
                .build(),
        Mesh::create()
                .withQuad()
                .build(),
        Mesh::create()  // teapot
                .withQuad()
                .build(),
        Mesh::create()  // head
                .withQuad()
                .build()
    };


    worldLights.addLight(Light::create()
                                 .withDirectionalLight(glm::normalize(glm::vec3(1,1,1)))
                                 .build());


    editorComponent.shaderSources[ShaderType::Vertex] ="standard_blinn_phong_vert.glsl";
    editorComponent.shaderSources[ShaderType::Fragment] ="standard_blinn_phong_frag.glsl";

    shader = Shader::getStandardBlinnPhong();
    material = shader->createMaterial();
    material->setColor({1,1,1,1});
    material->setSpecularity(Color(1,1,1,50));

    rebuildFBO(200, 100);

    ImGui::StyleColorsLight();
    ImGui::InitDock();
}

void GLSLEditor::updateErrorMarkers(std::vector<std::string>& errors, TextEditor& textEditor, ShaderType type){
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

void GLSLEditor::onKey(SDL_Event& key){
    lastKeypress = timeSinceStartup;
}

void GLSLEditor::update(float deltaTime){
    timeSinceStartup += deltaTime;
    if (lastKeypress && timeSinceStartup - lastKeypress > 0.2f){
        editorComponent.compile();
        lastKeypress = 0;
    }
}
