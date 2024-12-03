#pragma once

#include <GL/glew.h>
#include "Pesukarhu/graphics/shaders/Shader.h"
#include "Pesukarhu/graphics/Buffers.h"


// NOTE: web platform is also using some opengl stuff declared here!
namespace pk
{
    namespace opengl
    {
        GLenum to_gl_shader(ShaderStageFlagBits stage);
        GLenum to_gl_data_type(ShaderDataType shaderDataType);
        std::string gl_error_to_string(GLenum error);
    }
}
