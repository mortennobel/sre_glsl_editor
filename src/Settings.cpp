#include "Settings.hpp"
#include <fstream>
#include <iostream>
#include <sre/Log.hpp>
#include <sre/Resource.hpp>
#include "sre/Material.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLSLEditor.hpp"

using namespace picojson;

namespace {
    std::array<std::string,2> splitPathAndFilename(std::string filepath){
        int unixSep = (int)filepath.rfind('/');
        int winSep = (int)filepath.rfind('\\');
        size_t lastPathSeperator = std::max(unixSep, winSep);
        auto fileName = filepath.substr(lastPathSeperator + 1);
        auto path = filepath.substr(0,lastPathSeperator+1);
        return {path, fileName};
    }


    glm::vec2 parseVec2(value val){
        glm::vec2 res;
        auto array = val.get<picojson::array>();
        res.x = (float)array[0].get<double>();
        res.y = (float)array[1].get<double>();
        return res;
    }
    glm::vec3 parseVec3(value val){
        glm::vec3 res;
        auto array = val.get<picojson::array>();
        res.x = (float)array[0].get<double>();
        res.y = (float)array[1].get<double>();
        res.z = (float)array[2].get<double>();
        return res;
    }
    glm::vec4 parseVec4(value val){
        glm::vec4 res;
        auto array = val.get<picojson::array>();
        res.x = (float)array[0].get<double>();
        res.y = (float)array[1].get<double>();
        res.z = (float)array[2].get<double>();
        res.w = (float)array[3].get<double>();
        return res;
    }
    glm::mat4 parseMat4(value val){
        glm::mat4 res;
        auto array = val.get<picojson::array>();
        for (int i=0;i<16;i++){
            glm::value_ptr(res)[i] = (float)array[i].get<double>();
        }
        return res;
    }
    glm::mat3 parseMat3(value val){
        glm::mat4 res;
        auto array = val.get<picojson::array>();
        for (int i=0;i<16;i++){
            glm::value_ptr(res)[i] = (float)array[i].get<double>();
        }
        return res;
    }

    value vec2JSON(glm::vec2 v){
        value res = value(picojson::array_type, true);
        for (int i=0;i<2;i++){
            res.get<array>().push_back(value((double)v[i]));
        }
        return res;
    }
    value vec3JSON(glm::vec3 v){
        value res = value(picojson::array_type, true);
        for (int i=0;i<3;i++){
            res.get<array>().push_back(value((double)v[i]));
        }
        return res;
    }
    value vec4JSON(glm::vec4 v){
        value res = value(picojson::array_type, true);
        for (int i=0;i<4;i++){
            res.get<array>().push_back(value((double)v[i]));
        }
        return res;
    }
    value mat4JSON(glm::mat4 v){
        value res = value(picojson::array_type, true);
        for (int i=0;i<16;i++){
            res.get<array>().push_back(value((double)glm::value_ptr(v)[i]));
        }
        return res;
    }
    value mat3JSON(glm::mat4 v){
        value res = value(picojson::array_type, true);
        for (int i=0;i<9;i++){
            res.get<array>().push_back(value((double)glm::value_ptr(v)[i]));
        }
        return res;
    }
}

picojson::value Settings::materialUniformToJson(){
    value uniformsJSON = value(object_type,true);
    object& uniformsJSONObj = uniformsJSON.get<picojson::object>();

    for (auto uniformName : material->getShader()->getUniformNames()) {
        auto uniform = material->getShader()->getUniform(uniformName);

        value uniformJSON = value(object_type,true);
        object& uniformJSONObj = uniformJSON.get<picojson::object>();
        if (uniform.arraySize != 1) {
            LOG_ERROR("Array as Uniforms is currently not supported");
            continue;
        }
        uniformJSONObj["type"] = value((int64_t)uniform.type);
        switch (uniform.type){
            case sre::UniformType::Float: {
                float val = material->get<float>(uniformName);
                uniformJSONObj["value"] = value((double) val);
            }
                break;
            case sre::UniformType::Texture: {
                int64_t selected = 0;
                auto val = material->get<std::shared_ptr<sre::Texture>>(uniformName);
                for (int i=0;i<editor->textures.size();i++){
                    if (editor->textures[i] == val){
                        selected = i;
                        break;
                    }
                }
                uniformJSONObj["value"] = value((int64_t) selected);
            }
                break;
            case sre::UniformType::TextureCube: {
                int64_t selected = 0;
                auto val = material->get<std::shared_ptr<sre::Texture>>(uniformName);
                for (int i=0;i<editor->cubeTextures.size();i++){
                    if (editor->cubeTextures[i] == val){
                        selected = i;
                        break;
                    }
                }
                uniformJSONObj["value"] = value((int64_t) selected);
            }
                break;
            case sre::UniformType::Vec4: {
                auto val = material->get<glm::vec4>(uniformName);
                uniformJSONObj["value"] = vec4JSON(val);
            }
                break;
            case sre::UniformType::Mat4: {
                auto val = material->get<glm::mat4>(uniformName);
                uniformJSONObj["value"] = mat4JSON(val);
            }
                break;
            default:
                LOG_ERROR("%i not currently supported as uniform", uniform.type);
        }
        uniformsJSONObj[uniformName] = uniformJSON;
    }
    return uniformsJSON;
}

void Settings::parseUniform(picojson::value val, sre::Material* mat, GLSLEditor * editor) {
    object& valObject = val.get<object>();
    for (auto valAttribute : valObject) {
        sre::UniformType uniformType = (sre::UniformType)valAttribute.second.get("type").get<int64_t>();
        switch (uniformType){
            case sre::UniformType::Float:{
                auto floatValue = (float)valAttribute.second.get("value").get<double>();
                mat->set(valAttribute.first, floatValue);
            }
                break;
            case sre::UniformType::Texture:{
                auto intValue = (float)valAttribute.second.get("value").get<int64_t>();
                mat->set(valAttribute.first, editor->textures[intValue]);
            }
                break;
            case sre::UniformType::TextureCube:{
                auto intValue = (float)valAttribute.second.get("value").get<int64_t>();
                mat->set(valAttribute.first, editor->cubeTextures[intValue]);
            }
                break;
            case sre::UniformType::Vec4:
            {
                auto value = parseVec4(valAttribute.second.get("value"));
                mat->set(valAttribute.first, value);
            }
                break;
            case sre::UniformType::Mat4:
            {
                auto value = parseMat4(valAttribute.second.get("value"));
                mat->set(valAttribute.first, value);
            }
                break;
            default:
                LOG_ERROR("%i not currently supported as uniform", uniformType);
        }
    }
}

std::map<sre::ShaderType, std::string> parseFileNames(value v, std::string path){
    std::map<sre::ShaderType, std::string> res;
    for (auto attribute : v.get<array>()){
        sre::ShaderType type = (sre::ShaderType)attribute.get("type").get<int64_t>();
        std::string value = attribute.get("value").get<std::string>();
        res[type] = path + value;
    }
    return res;
}

void Settings::load(std::string filepath, GLSLEditor * editor){
    picojson::value v;
    std::fstream fis(filepath, std::ios_base::in);
    fis >> v;
    if (std::cin.fail()) {
        std::cerr << picojson::get_last_error() << std::endl;
        return;
    }
    auto & root = v.get<object>();
    editor->settings = {};
    Settings& settings = editor->settings;
    settings.selectedMesh = (int)root["selectedMesh"].get<int64_t>();
    settings.perspectiveCamera = root["perspectiveCamera"].get<bool>();
    settings.rotateCamera = parseVec2(root["rotateCamera"]);
    settings.clearType = (Clear)root["clearType"].get<int64_t>();
    settings.clearColor = parseVec4(root["clearColor"]);

    auto res = splitPathAndFilename(filepath);

    auto path = res[0];
    settings.filenames = parseFileNames(root["filenames"], path);

    settings.filepath = filepath;
    settings.editor = editor;

    editor->setShader(settings);

    parseUniform(root["uniforms"], settings.material.get(), editor);
}

void Settings::save(std::string filepath){
    using namespace picojson;
    value v(object_type, true);
    auto & root = v.get<object>();
    // serialize data
    root["selectedMesh"] = value((int64_t)selectedMesh);
    root["perspectiveCamera"] = value(perspectiveCamera);
    root["rotateCamera"] = vec2JSON(rotateCamera);
    root["clearType"] = value((int64_t)clearType);
    root["clearColor"] = vec4JSON(clearColor);
    root["uniforms"] = materialUniformToJson();

    // filepath not serialized
    root["filenames"] = fileNamesToJSON(filepath);



    std::fstream fos(filepath, std::ios_base::out);
    fos << v;

}


picojson::value Settings::fileNamesToJSON(std::string filepath) {
    auto res = splitPathAndFilename(filepath);
    auto fileName = res[1];
    auto path = res[0];
    value v(array_type, true);
    array& a = v.get<array>();

    for (auto fn : filenames){
        value filenameObject(object_type, true);
        object& filenameObjectO = filenameObject.get<object>();
        filenameObjectO["type"] = value((int64_t)fn.first);
        switch (fn.first){
            case sre::ShaderType::TessellationEvaluation:
                filenameObjectO["value"] = value(fileName+".tess_e.glsl");
                break;
            case sre::ShaderType::TessellationControl:
                filenameObjectO["value"] = value(fileName+".tess_c.glsl");
                break;
            case sre::ShaderType::Vertex:
                filenameObjectO["value"] = value(fileName+".vert.glsl");
                break;
            case sre::ShaderType::Fragment:
                filenameObjectO["value"] = value(fileName+".frag.glsl");
                break;
            case sre::ShaderType::Geometry:
                filenameObjectO["value"] = value(fileName+".geom.glsl");
                break;
            default:
                LOG_ERROR("Type %i not supported", fn.first);
        }

        auto glslCode = sre::Resource::loadText(fn.second);
        std::fstream fos(path + filenameObjectO["value"].get<std::string>(), std::ios_base::out);
        fos << glslCode;
        a.push_back(filenameObject);
    }
    return v;
}
