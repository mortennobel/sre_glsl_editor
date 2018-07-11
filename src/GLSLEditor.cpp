//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#include "GLSLEditor.hpp"
#include <vector>
#include <sre/Color.hpp>
#include <sre/Resource.hpp>
#include <sre/imgui_sre.hpp>
#include <sre/ModelImporter.hpp>
#include "glm/gtc/random.hpp"
#include "imgui_dock.h"

using namespace sre;

GLSLEditor::GLSLEditor()
        :vertexShaderComponent(this, sre::ShaderType::Vertex),
         geometryShaderComponent(this, sre::ShaderType::Geometry),
         fragmentShaderComponent(this, sre::ShaderType::Fragment),
         settingsComponent(this),
         uniformComponent(this)
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
    renderScene();

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

        for (auto elem : filenames){
            switch (elem.first){
                case ShaderType::Vertex:
                    if(ImGui::BeginDock("Vertex Shader")){
                        vertexShaderComponent.gui();
                    }
                    ImGui::EndDock();
                    ImGui::SetNextDock( "Vertex Shader", ImGuiDockSlot::ImGuiDockSlot_None );
                    break;
                case ShaderType::Fragment:
                    if(ImGui::BeginDock("Fragment Shader")){
                        fragmentShaderComponent.gui();
                    }
                    ImGui::EndDock();
                    ImGui::SetNextDock( "Fragment Shader", ImGuiDockSlot::ImGuiDockSlot_None );
                    break;
                case ShaderType::Geometry:
                    if(ImGui::BeginDock("Geometry Shader")){
                        geometryShaderComponent.gui();
                    }
                    ImGui::EndDock();
                    ImGui::SetNextDock( "Geometry Shader", ImGuiDockSlot::ImGuiDockSlot_None );
                    break;
            }
        }

        if(ImGui::BeginDock("Errors")){
            showErrors();
        }
        ImGui::EndDock();
        ImGui::SetNextDock( "Errors", ImGuiDockSlot::ImGuiDockSlot_Bottom );

        if(ImGui::BeginDock("Scene")){
            ImVec2 size = ImGui::GetContentRegionAvail();
            if (size.x != sceneTexture->getWidth() || size.y != sceneTexture->getHeight()){
                rebuildFBO(size.x, size.y);
            }
            ImGui_RenderTexture(sceneTexture.get(), {size.x,size.y}, {0,1}, {1,0});
        }
        ImGui::EndDock();

        if(ImGui::BeginDock("Settings")){
            settingsComponent.gui();
        }
        ImGui::EndDock();

        if(ImGui::BeginDock("Uniforms")){
            uniformComponent.gui();
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

    // clear fbo
    renderScene();
}

void GLSLEditor::renderScene(){
    camera.lookAt({0,0,3},{0,0,0},{0,1,0});
    if (settings.perspectiveCamera){
        camera.setPerspectiveProjection(60,0.1f,100);
    } else {
        camera.setOrthographicProjection(1,0,100);
    }


    sre::Color color(settings.clearColor);
    auto rp =  RenderPass::create()
            .withCamera(camera)
            .withFramebuffer(framebufferObject)
            .withWorldLights(&worldLights)
            .withClearColor(true, color)
            .withGUI(false)
            .build();

    rp.draw(meshes[settings.selectedMesh], pos1, settings.material);

    rp.finish();
}

void GLSLEditor::init() {
    camera.lookAt({0,0,3},{0,0,0},{0,1,0});
    camera.setPerspectiveProjection(60,0.1f,100);

    textures = {
            Texture::getWhiteTexture(),
            Texture::create().withFile("resources/checker-uv.png").withGenerateMipmaps(true).build(),
            Texture::create().withFile("resources/checker-pattern.png").withGenerateMipmaps(true).build(),
    };

    cubeTextures = {
            Texture::getDefaultCubemapTexture()
    };

    // randomize particle positions
    std::vector<glm::vec3> particles(1000);
    for (auto & p : particles){
        p = glm::linearRand(glm::vec3(-1,-1,-1), glm::vec3(1,1,1));
    }

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
            sre::ModelImporter::importObj("resources", "utah-teapot.obj"),
            Mesh::create()
                    .withPositions(particles)
                    .withMeshTopology(MeshTopology::Points)
                    .build()
    };


    worldLights.addLight(Light::create()
                                 .withDirectionalLight(glm::normalize(glm::vec3(1,1,1)))
                                 .build());

    settings.shaderSource[ShaderType::Vertex] = Resource::loadText("standard_blinn_phong_vert.glsl");
    settings.shaderSource[ShaderType::Fragment] = Resource::loadText("standard_blinn_phong_frag.glsl");
    settings.shaderSource[ShaderType::Geometry] = "";

    filenames[ShaderType::Vertex] = "standard_blinn_phong_vert.glsl";
    filenames[ShaderType::Fragment] = "standard_blinn_phong_frag.glsl";

    shader = Shader::getStandardBlinnPhong();
    settings.material = shader->createMaterial();
    settings.material->setColor({1,1,1,1});
    settings.material->setSpecularity(Color(1,1,1,50));

    rebuildFBO(200, 100);

    ImGui::StyleColorsLight();
    ImGui::InitDock();
}

void GLSLEditor::compileShader(){
    auto builder = shader->update();

    for (auto elem : filenames){
        auto filename = elem.second;
        Resource::set(filename, (&vertexShaderComponent)[(int)elem.first].textEditor.GetText());
        builder.withSourceResource(filename, elem.first);
    }
    errors.clear();
    builder.build(errors);

    vertexShaderComponent.updateErrorMarkers(errors);
    fragmentShaderComponent.updateErrorMarkers(errors);
    geometryShaderComponent.updateErrorMarkers(errors);

}

void GLSLEditor::showErrors(){
    if (errors.empty()){
        ImGui::LabelText("", "No errors");
    }
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
        compileShader();
        lastKeypress = 0;
    }
}
