#pragma once

#include "utils/pkmath.h"
#include <vector>


namespace pk
{
    struct Joint
    {
        vec3 translation;
        quat rotation;
        vec3 scale;
        mat4 matrix;
        mat4 inverseMatrix; // Created only for the bind pose
    };


    struct Pose
    {
        std::vector<Joint> joints;
        std::vector<std::vector<int>> jointChildMapping;
    };
}
