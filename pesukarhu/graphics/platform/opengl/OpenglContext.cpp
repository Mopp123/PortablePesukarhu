#include <GL/glew.h>
#include "OpenglContext.h"
#include "pesukarhu/core/Debug.h"
#include <string>


namespace pk
{
    namespace opengl
    {
        unsigned int to_gl_shader(ShaderStageFlagBits stage)
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


        unsigned int to_gl_data_type(ShaderDataType shaderDataType)
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


        std::string gl_error_to_string(unsigned int error)
        {
            switch (error)
            {
                case GL_INVALID_ENUM :                  return "GL_INVALID_ENUM";
                case GL_INVALID_VALUE :                 return "GL_INVALID_VALUE";
                case GL_INVALID_OPERATION :             return "GL_INVALID_OPERATION";
                case GL_STACK_OVERFLOW :                return "GL_STACK_OVERFLOW";
                case GL_STACK_UNDERFLOW :               return "GL_STACK_UNDERFLOW";
                case GL_OUT_OF_MEMORY :                 return "GL_OUT_OF_MEMORY";
                case GL_INVALID_FRAMEBUFFER_OPERATION : return "GL_INVALID_FRAMEBUFFER_OPERATION";

                default:
                    Debug::log(
                        "@gl_enum_to_string "
                        "Invalid erro GLenum: " + std::to_string(error)
                    );
            }
            return "<not found>";
        }


        OpenglContext::OpenglContext(desktop::DesktopWindow* pWindow)
        {
            glfwMakeContextCurrent(pWindow->getGLFWwindow());

            // NOTE: Below was the "regular" glewInit() before.
            // Found glewContextInit() after long dig in the glew github repo
            // which apparently deals with the Wayland/x11 issue...
            // Not sure tho, might fuck up on some systems...
            // https://github.com/etlegacy/etlegacy/pull/1966
            // https://github.com/nigels-com/glew/issues/172
            GLenum initStatus = glewContextInit();
            if (initStatus != GLEW_OK)
            {
                std::string glewErrStr = (const char*)glewGetErrorString(initStatus);
                Debug::log(
                    "@OpenglContext::OpenglContext "
                    "glewInit failed! GLEW error: " + glewErrStr,
                    Debug::MessageType::PK_FATAL_ERROR
                );
                glfwTerminate();
            }
        }

        OpenglContext::~OpenglContext()
        {
        }
    }
}
