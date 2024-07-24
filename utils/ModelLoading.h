#pragma once

#include "graphics/Model.h"
#include <string>


namespace pk
{
    Model* load_model_glb(const std::string& filepath);
}
