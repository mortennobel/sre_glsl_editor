#include "Settings.hpp"
#include "picojson.h"
#include <fstream>

using namespace picojson;

Settings Settings::load(std::string filepath){
    picojson::value v;
    std::fstream fis(filepath, std::ios_base::in);
    fis >> v;
    if (std::cin.fail()) {
        std::cerr << picojson::get_last_error() << std::endl;
        return {};
    }
    auto & root = v.get<object>();
    Settings settings;
    settings.selectedMesh = (int)root["selectedMesh"].get<double>();
    settings.perspectiveCamera = (int)root["perspectiveCamera"].get<double>();
    return settings;
}

void Settings::save(std::string filepath){
    using namespace picojson;
    value v(object_type, true);
    auto & root = v.get<object>();
    root["selectedMesh"] = value((double)selectedMesh);
    root["perspectiveCamera"] = value(perspectiveCamera);
    std::fstream fos(filepath, std::ios_base::in);
    fos << v;

}
