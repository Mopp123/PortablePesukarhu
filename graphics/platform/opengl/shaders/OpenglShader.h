#pragma once

#include "graphics/shaders/Shader.h"
#include "utils/pkmath.h"


namespace pk
{
    namespace opengl
    {
        // TODO: Create and link the "combined gl shader program" in the pipeline creation
        // -> this is to have completely separate "shader module structure" for different
        // stages like in vulkan..
        class OpenglShader : public Shader
        {
        private:
            // We want to store the "shader source string" to get uniform names and locations!
            // We'll clear that after we get those uniforms
            std::string _source;
            uint32_t _shaderID = 0;

        public:
            OpenglShader(const std::string& source, ShaderStageFlagBits stage);
            ~OpenglShader();
            inline uint32_t getID() const { return _shaderID; }
        };


        class OpenglShaderProgram
        {
        private:
            const OpenglShader* _pVertexShader;
            const OpenglShader* _pFragmentShader;
            uint32_t _programID = 0;

        public:
            OpenglShaderProgram(const OpenglShader* pVertexShader, const OpenglShader* pFragmentShader);
            ~OpenglShaderProgram();

            inline uint32_t getID() const { return _programID; }

            int getAttribLocation(const char* name) const;
            int getUniformLocation(const char* name) const;
            void setUniform(int location, const mat4& matrix) const;
            void setUniform(int location, const vec3& v) const;
            void setUniform(int location, float val) const;
            void setUniform1i(int location, int val) const;
        };
    }
}
