#include "Shader.h"
#include "graphics/Context.h"
#include "graphics/platform/opengl/shaders/OpenglShader.h"
#include "core/Debug.h"


namespace pk
{
    Shader* Shader::create(const std::string& shaderSource, ShaderStageFlagBits stage)
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
}
