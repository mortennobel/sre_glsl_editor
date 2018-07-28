#!/bin/bash

if [ -z "$EMSDK" ]
then
      echo "\$EMSDK is not defined. Using ~/programming/cpp/emsdk_portable"
      EMSDK=~/programming/cpp/emsdk_portable
fi

source ${EMSDK}/emsdk_env.sh

emcc -Isrc -ISimpleRenderEngine/include -ISimpleRenderEngine/submodules/imgui -ISimpleRenderEngine/submodules/glm -ISimpleRenderEngine/submodules/picojson -ISimpleRenderEngine/submodules/ImGuiColorTextEdit -Isubmodules/imguiDock \
               SimpleRenderEngine/submodules/imgui/imgui.cpp \
               SimpleRenderEngine/submodules/imgui/imgui_draw.cpp \
               SimpleRenderEngine/submodules/ImGuiColorTextEdit/TextEditor.cpp \
               SimpleRenderEngine/src/sre/Camera.cpp \
               SimpleRenderEngine/src/sre/Color.cpp \
               SimpleRenderEngine/src/sre/Framebuffer.cpp \
               SimpleRenderEngine/src/sre/imgui_sre.cpp \
               SimpleRenderEngine/src/sre/Inspector.cpp \
               SimpleRenderEngine/src/sre/Light.cpp \
               SimpleRenderEngine/src/sre/Log.cpp \
               SimpleRenderEngine/src/sre/Material.cpp \
               SimpleRenderEngine/src/sre/Mesh.cpp \
               SimpleRenderEngine/src/sre/ModelImporter.cpp \
               SimpleRenderEngine/src/sre/Renderer.cpp \
               SimpleRenderEngine/src/sre/RenderPass.cpp \
               SimpleRenderEngine/src/sre/Resource.cpp \
               SimpleRenderEngine/src/sre/SDLRenderer.cpp \
               SimpleRenderEngine/src/sre/Shader.cpp \
               SimpleRenderEngine/src/sre/Skybox.cpp \
               SimpleRenderEngine/src/sre/Sprite.cpp \
               SimpleRenderEngine/src/sre/SpriteAtlas.cpp \
               SimpleRenderEngine/src/sre/SpriteBatch.cpp \
               SimpleRenderEngine/src/sre/Texture.cpp \
               SimpleRenderEngine/src/sre/VR.cpp \
               SimpleRenderEngine/src/sre/WorldLights.cpp \
               SimpleRenderEngine/src/sre/impl/GL.cpp \
               SimpleRenderEngine/src/sre/impl/UniformSet.cpp \
               submodules/imguiDock/imgui_dock.cpp \
               src/EditorComponent.cpp \
               src/GLSLEditor.cpp \
               src/main.cpp \
               src/Settings.cpp \
               src/SettingsComponent.cpp \
               src/UniformComponent.cpp \
               -O2 -s ASSERTIONS=1 -std=c++14 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s TOTAL_MEMORY=67108864 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file resources/imgui.ini@imgui.ini --preload-file resources -s USE_SDL=2 -o docs/GLSL_Editor.html
               #-O3 -g4 -s ASSERTIONS=1 -std=c++14 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s TOTAL_MEMORY=67108864 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file test_data -s USE_SDL=2 -o html/$FILENAME.html
