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
                        "@to_gl_shader "
                        "Invalid stage: " + std::to_string((uint32_t)stage),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
            }
        }


        GLenum to_gl_data_type(ShaderDataType shaderDataType)
        {
            switch (shaderDataType)
            {
                case ShaderDataType::None:
                    Debug::log(
                        "@to_gl_data_type Invalid shaderDataType <NONE>",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;

                case ShaderDataType::Int:
                    return GL_INT;
                case ShaderDataType::Int2:
                    return GL_INT;
                case ShaderDataType::Int3:
                    return GL_INT;
                case ShaderDataType::Int4:
                    return GL_INT;

                case ShaderDataType::Float:
                    return GL_FLOAT;
                case ShaderDataType::Float2:
                    return GL_FLOAT;
                case ShaderDataType::Float3:
                    return GL_FLOAT;
                case ShaderDataType::Float4:
                    return GL_FLOAT;

                default:
                    Debug::log(
                        "@to_gl_data_type Invalid shaderDataType",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
            }
        }
    }
}
