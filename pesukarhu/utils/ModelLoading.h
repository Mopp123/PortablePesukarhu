#pragma once

#include "pesukarhu/resources/Model.h"
#include <string>


namespace pk
{
    Model* load_model_gltf(const std::string& filepath);
}
