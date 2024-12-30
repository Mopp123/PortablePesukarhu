#pragma once

#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"
#include "pesukarhu/graphics/shaders/Shader.h"
#include "pesukarhu/graphics/Buffers.h"

#ifdef PK_DEBUG_FULL
#define GL_FUNC(func)	func;																				\
    switch (glGetError()){																						\
        case GL_NO_ERROR:break;																						\
        case GL_INVALID_ENUM:					Debug::log("Opengl Error: GL_INVALID_ENUM", Debug::MessageType::PK_FATAL_ERROR);break;					\
        case GL_INVALID_VALUE:					Debug::log("Opengl Error: GL_INVALID_VALUE", Debug::MessageType::PK_FATAL_ERROR);break;					\
        case GL_INVALID_OPERATION:				Debug::log("Opengl Error: GL_INVALID_OPERATION", Debug::MessageType::PK_FATAL_ERROR);break;				\
        case GL_INVALID_FRAMEBUFFER_OPERATION:	Debug::log("Opengl Error: GL_INVALID_FRAMEBUFFER_OPERATION", Debug::MessageType::PK_FATAL_ERROR);break;	\
        case GL_OUT_OF_MEMORY:					Debug::log("Opengl Error: GL_OUT_OF_MEMORY", Debug::MessageType::PK_FATAL_ERROR);break;					\
        case GL_STACK_UNDERFLOW:				Debug::log("Opengl Error: GL_STACK_UNDERFLOW", Debug::MessageType::PK_FATAL_ERROR);break;				\
        case GL_STACK_OVERFLOW:					Debug::log("Opengl Error: GL_STACK_OVERFLOW", Debug::MessageType::PK_FATAL_ERROR);break;				\
    default: break;}
#else
    #define GL_FUNC(func)	func;
#endif

// NOTE: web platform is also using some opengl stuff declared here!
namespace pk
{
    namespace opengl
    {
        // these uints were changed from GLenum
        unsigned int to_gl_shader(ShaderStageFlagBits stage);
        unsigned int to_gl_data_type(ShaderDataType shaderDataType);
        std::string gl_error_to_string(unsigned int error);

        class OpenglContext : public Context
        {
        private:
            int32_t _maxTextureUnits = 0;

            // NOTE: atm testing if works using same VAO for everything so
            // shit stays more consistent through different APIs
            static unsigned int s_VAO;

        public:
            OpenglContext(desktop::DesktopWindow* pWindow);
            OpenglContext(const OpenglContext&) = delete;
            ~OpenglContext();


            static unsigned int getVAO();
            inline int32_t getMaxTextureUnits() const { return _maxTextureUnits; }
        };
    }
}
