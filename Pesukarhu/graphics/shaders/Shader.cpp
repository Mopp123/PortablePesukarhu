#include "Shader.h"
#include "graphics/Context.h"
#include "graphics/platform/opengl/shaders/OpenglShader.h"
#include "core/Debug.h"
#include "utils/FileUtils.h"


namespace pk
{
    Shader* Shader::create_from_source(const std::string& shaderSource, ShaderStageFlagBits stage)
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new opengl::OpenglShader(shaderSource, stage);
            default:
                Debug::log(
                    "Attempted to create shader but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }

    Shader* Shader::create_from_file(const std::string& filepath, ShaderStageFlagBits stage)
    {
        const std::string source = load_text_file(filepath);
        if (source.empty())
        {
            Debug::log(
                "@Shader::create(2) "
                "Failed to get shader source from file: " + filepath,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return create_from_source(source, stage);
    }
}
