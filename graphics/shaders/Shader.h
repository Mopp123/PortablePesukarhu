#pragma once

#include <string>


namespace pk
{
    enum ShaderStageFlagBits
    {
        SHADER_STAGE_VERTEX_BIT = 0x1,
        SHADER_STAGE_FRAGMENT_BIT = 0x2
    };


    class Shader
    {
    public:
        Shader() {}
        virtual ~Shader() {}
    };
}
