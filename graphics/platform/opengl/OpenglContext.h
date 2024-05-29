#pragma once

#include <GL/glew.h>
#include "graphics/shaders/Shader.h"


// NOTE: web platform is also using some opengl stuff declared here!
namespace pk
{
    namespace opengl
    {
        GLenum to_gl_shader(ShaderStageFlagBits stage);
    }
}
