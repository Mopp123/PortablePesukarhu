#pragma once

#include "utils/pkmath.h"
#include <vector>


namespace pk
{
    struct Joint
    {
        vec3 pos;
        quat rotation;
    };


    struct Pose
    {
        std::vector<Joint> joints;
        std::vector<std::vector<uint32_t>> jointChildMapping;
    };
}
