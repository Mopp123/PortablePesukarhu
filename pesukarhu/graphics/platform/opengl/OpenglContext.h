#pragma once

#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"
#include "pesukarhu/graphics/shaders/Shader.h"
#include "pesukarhu/graphics/Buffers.h"


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
        public:
            OpenglContext(desktop::DesktopWindow* pWindow);
            OpenglContext(const OpenglContext&) = delete;
            ~OpenglContext();
        };
    }
}
