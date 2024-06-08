#pragma once

#include <string>


namespace pk
{
    enum ShaderStageFlagBits
    {
        SHADER_STAGE_NONE = 0,
        SHADER_STAGE_VERTEX_BIT = 0x1,
        SHADER_STAGE_FRAGMENT_BIT = 0x2
    };

    // Atm used for determining how to handle gl shader
    // attrib and uniform locations if required to handle
    // at all..
    enum class ShaderVersion
    {
        GLSL3,
        ESSL1
    };


    class Shader
    {
    protected:
        ShaderStageFlagBits _stage = ShaderStageFlagBits::SHADER_STAGE_NONE;

    public:
        // NOTE: need to support old way of using shaders for now!
        // TODO: Switch everythin to use new system eventually..
        Shader() {}
        virtual ~Shader() {}

        static Shader* create(const std::string& shaderSource, ShaderStageFlagBits stage);

    protected:
        Shader(const std::string& shaderSource, ShaderStageFlagBits stage) :
            _stage(stage)
        {}
    };
}
