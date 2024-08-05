#pragma once

#include "utils/pkmath.h"
#include <vector>


namespace pk
{
    struct Joint
    {
        vec3 pos;
        quat rotation;
        mat4 matrix;
    };


    struct Pose
    {
        std::vector<Joint> joints;
        std::vector<std::vector<int>> jointChildMapping;
    };
}
