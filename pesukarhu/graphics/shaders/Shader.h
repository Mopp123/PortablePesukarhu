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

        static std::string s_shaderRootWeb;
        static std::string s_shaderRootOpengl;

    public:
        // NOTE: need to support old way of using shaders for now!
        // TODO: Switch everythin to use new system eventually..
        Shader() {}
        virtual ~Shader() {}

        static Shader* create_from_source(const std::string& shaderSource, ShaderStageFlagBits stage);
        static Shader* create_from_file(const std::string& filepath, ShaderStageFlagBits stage);

        // shaderFile should be just the name of the shader file
        // Returns actual full path to the shader file depending on used GraphicsAPI
        static std::string get_shader_path(const std::string& shaderFile);

    protected:
        Shader(const std::string& shaderSource, ShaderStageFlagBits stage) :
            _stage(stage)
        {}
    };
}
