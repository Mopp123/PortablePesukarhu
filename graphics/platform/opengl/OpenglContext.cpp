#include "OpenglContext.h"
#include "core/Debug.h"


namespace pk
{
    namespace opengl
    {
        GLenum to_gl_shader(ShaderStageFlagBits stage)
        {
            switch (stage)
            {
                case ShaderStageFlagBits::SHADER_STAGE_VERTEX_BIT:
                    return GL_VERTEX_SHADER;
                case ShaderStageFlagBits::SHADER_STAGE_FRAGMENT_BIT:
                    return GL_FRAGMENT_SHADER;
                default:
                    Debug::log(
                        "@GLenum to_gl_shader(ShaderStageFlagBits) "
                        "Invalid stage: " + std::to_string((uint32_t)stage),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
            }
        }
    }
}
