//
// Created by Morten Nobel-Jørgensen on 6/5/18.
//

#include "GLSLEditor.hpp"
#include <vector>
#include <fstream>
#include <sre/Color.hpp>
#include <sre/Resource.hpp>
#include <sre/imgui_sre.hpp>
#include <sre/ModelImporter.hpp>
#include "glm/gtc/random.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "imgui_dock.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#ifndef EMSCRIPTEN
#include "tinyfiledialogs.h"
#endif

using namespace sre;

namespace {

    void copyFile(const char * source, const char * dest)
    {
        std::ifstream  src(source, std::ios::binary);
        std::ofstream  dst(dest,   std::ios::binary);

        dst << src.rdbuf();
    }
    bool isFileExist(const char *fileName)
    {
        std::ifstream infile(fileName);
        return infile.good();
    }

    bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    const char* to_string(ShaderType shaderType){
        switch (shaderType){
            case ShaderType::Geometry:
                return "Geometry Shader";
            case ShaderType::Fragment:
                return "Fragment Shader";
            case ShaderType::Vertex:
                return "Vertex Shader";
            case ShaderType::TessellationControl:
                return "Tess Ctrl Shader";
            case ShaderType::TessellationEvaluation:
                return "Tess Eval Shader";
            default:
                return "Unknown";
        }
    }
}

GLSLEditor::GLSLEditor()
        :settingsComponent(this),
         uniformComponent(this)
{
#ifndef EMSCRIPTEN
    prefPath = SDL_GetPrefPath("SimpleRenderEngine","GLSLEditor");
    static auto iniFilename = prefPath+"imgui.ini";
    if (!isFileExist(iniFilename.c_str()) ){
        copyFile("resources/imgui.ini", iniFilename.c_str());
    }
#endif
    versionString = std::to_string(versionMajor)+"."+std::to_string(versionMinor);
    SDLRenderer r;
    r.setWindowTitle(std::string("GLSL Editor ")+versionString);
    r.init();
#ifndef EMSCRIPTEN
    ImGui::GetIO().IniFilename = iniFilename.c_str();
#endif
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
    r.mouseEvent = [&](SDL_Event& mouse){
        if (mouse.type == SDL_MOUSEMOTION){
            mousePos = {mouse.motion.x,mouse.motion.y};
            mouseDelta = {mouse.motion.xrel,mouse.motion.yrel};
        }
        if (mouse.type == SDL_MOUSEBUTTONDOWN || mouse.type == SDL_MOUSEBUTTONUP){
            if (mouse.button.button == SDL_BUTTON_LEFT){
                mousePressed = mouse.button.state == SDL_PRESSED;
                mousePos = {mouse.button.x,mouse.button.y};
                mouseDelta = {0,0};
            }
        }
    };
    init();
    r.startEventLoop();
}

void GLSLEditor::newProject(){
    Settings settings;
    settings.filenames[ShaderType::Vertex] = "standard_blinn_phong_vert.glsl";
    settings.filenames[ShaderType::Fragment] = "standard_blinn_phong_frag.glsl";
    settings.shaderSource[ShaderType::Geometry] = "";
    settings.editor = this;
    setProject(settings);
}

void GLSLEditor::loadProject(){
#ifndef EMSCRIPTEN
    const char* filePattern = "*.shader";
    auto filePath = tinyfd_openFileDialog(
            "Open glsl project", /* NULL or "" */
            "", /* NULL or "" */
            1 , /* 0 */
            &filePattern, /* NULL | {"*.jpg","*.png"} */
            "Shader project",
            false); /* NULL | "text files" */
    Settings::load(filePath, this);
#endif
}

void GLSLEditor::saveProject(){
#ifndef EMSCRIPTEN
    if (settings.filepath.length() == 0){
        saveAsProject();
        return;
    }
    this->settings.save(settings.filepath);
#endif
}

void GLSLEditor::setShader(Settings& settings){
    editorComponents.clear();
    auto shaderBuilder = Shader::create();
    for (auto typeName : settings.filenames){
        auto shaderType = typeName.first;
        auto shaderSource = Resource::loadText(typeName.second);
        settings.shaderSource[shaderType] = shaderSource;

        editorComponents.emplace(std::pair<sre::ShaderType, std::shared_ptr<EditorComponent>>{shaderType, std::make_shared<EditorComponent>(this, shaderType, shaderSource)});
        shaderBuilder.withSourceResource(typeName.second, shaderType);
    }

    shader = shaderBuilder.build();
    settings.material = shader->createMaterial();
}

void GLSLEditor::setProject(Settings& settings){
    this->settings = settings;

    setShader(this->settings);

}

void GLSLEditor::saveAsProject(){
#ifndef EMSCRIPTEN
    const char* filePattern = "*.shader";
    auto filepath = tinyfd_saveFileDialog(
            "Save glsl project", /* NULL or "" */
            "some.shader", /* NULL or "" */
            1 , /* 0 */
            &filePattern, /* NULL | {"*.jpg","*.png"} */
            "Shader project"); /* NULL | "text files" */
    if (filepath != nullptr && strlen(filepath) > 0){
        settings.filepath = filepath;
        if (!hasEnding(settings.filepath, ".shader")){
            settings.filepath += ".shader";
        }
        saveProject();
    }
#endif
}


void GLSLEditor::render() {
    renderScene();

    auto rp2 = RenderPass::create()
            .withCamera(camera)
            .withWorldLights(&worldLights)
            .withClearColor(true,{.98f,.98f,.98f,1})
            .build();

    gui();
    mouseDelta = {0,0};
}

void GLSLEditor::guiMenu(){
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")){
                newProject();
            }
            if (ImGui::MenuItem("Load")){
                loadProject();
            }
            if (ImGui::MenuItem("Save")){
                saveProject();
            }
            if (ImGui::MenuItem("Save as ...")){
                saveAsProject();
            }
            if (ImGui::MenuItem("Revert from disk")){
                Settings::load(settings.filepath, this);
            }
            /* ImGui::Separator();
            if (ImGui::BeginMenu("Vertex shader")){
                if (ImGui::MenuItem("New")){

                }
                if (ImGui::MenuItem("Import")){

                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Fragment shader")){
                if (ImGui::MenuItem("New")){

                }
                if (ImGui::MenuItem("Import")){

                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Geometry shader")){
                if (ImGui::MenuItem("New")){

                }
                if (ImGui::MenuItem("Import")){

                }
                if (ImGui::MenuItem("Close")){

                }
                ImGui::EndMenu();
            }
             */
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GLSLEditor::gui(){
    guiMenu();
    auto size = Renderer::instance->getWindowSize();
    ImGui::SetNextWindowPos(ImVec2(.0f, 20.0f), ImGuiSetCond_Always);
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

        for (auto elem : settings.filenames){
            auto shaderName = to_string(elem.first);
            if(ImGui::BeginDock(shaderName)){
                editorComponents[elem.first]->gui();
            }
            ImGui::EndDock();
            ImGui::SetNextDock( shaderName, ImGuiDockSlot::ImGuiDockSlot_None );
        }

        if(ImGui::BeginDock("Errors")){
            showErrors();
        }
        ImGui::EndDock();
        ImGui::SetNextDock( "Errors", ImGuiDockSlot::ImGuiDockSlot_Bottom );

        if(ImGui::BeginDock("Scene")){
            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            if (contentSize.x >0 && contentSize.y > 0){
                if (contentSize.x != sceneTexture->getWidth() || contentSize.y != sceneTexture->getHeight()){
                    rebuildFBO((int)contentSize.x, (int)contentSize.y);
                }
                ImGui_RenderTexture(sceneTexture.get(), {contentSize.x,contentSize.y}, {0,1}, {1,0});
                if (ImGui::IsItemHoveredRect() && mousePressed){
                    settings.rotateCamera -= glm::ivec2{mouseDelta.y,mouseDelta.x};
                    settings.rotateCamera.x = glm::clamp(settings.rotateCamera.x,-89.0f,89.0f);
                }
            }
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
    glm::vec3 position = {0,0,3};
    position = glm::rotateX(position, glm::radians(settings.rotateCamera.x));
    position = glm::rotateY(position, glm::radians(settings.rotateCamera.y));
    camera.lookAt(position,{0,0,0},{0,1,0});
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

    newProject();

    rebuildFBO(200, 100);

    ImGui::StyleColorsLight();
    ImGui::InitDock();
}

void GLSLEditor::compileShader(){
    auto builder = shader->update();

    for (auto elem : settings.filenames){
        auto filename = elem.second;
        if (filename.length() > 0){

            Resource::set(filename, editorComponents[elem.first]->textEditor.GetText());
            builder.withSourceResource(filename, elem.first);
        }
    }
    errors.clear();
    builder.build(errors);

    for (auto &e : editorComponents){
        e.second->updateErrorMarkers(errors);
    }
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
    switch (key.key.keysym.sym){
        case SDLK_HOME:
        case SDLK_PAGEUP:
        case SDLK_END:
        case SDLK_PAGEDOWN:
        case SDLK_RIGHT:
        case SDLK_LEFT:
        case SDLK_DOWN:
        case SDLK_UP:
            return;
    }
    lastKeypress = timeSinceStartup;
}

void GLSLEditor::update(float deltaTime){
    timeSinceStartup += deltaTime;
    if (lastKeypress > 0 && timeSinceStartup - lastKeypress > 0.2f){
        compileShader();
        lastKeypress = 0;
    }
}

#pragma clang diagnostic pop