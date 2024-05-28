#pragma once

#include "../../../shaders/Shader.h"
#include "../../../../Common.h"
#include "../../../../utils/pkmath.h"
#include <GL/glew.h>


namespace pk
{
    namespace web
    {
        // TODO: SWITCH TO USE BELOW RATHER!
        class WebShader : public Shader
        {
        public:
            uint32_t _vertexShaderID = 0;
            uint32_t _fragmentShaderID = 0;
            uint32_t _programID = 0;

        public:
            WebShader(const std::string& vertexSource, const std::string& fragmentSource);
            ~WebShader();

            int getAttribLocation(const char* name) const;
            int getUniformLocation(const char* name) const;
            void setUniform(int location, const mat4& matrix) const;
            void setUniform(int location, const vec3& v) const;
            void setUniform(int location, float val) const;
            void setUniform1i(int location, int val) const;

            inline uint32_t getProgramID() const { return _programID; }
        };


        // TODO: Create and link the "combined gl shader program" in the pipeline creation
        // -> this is to have completely separate "shader module structure" for different
        // stages like in vulkan..
        class WebShader_NEW : public Shader
        {
        public:
            uint32_t _shaderID = 0;

        public:
            WebShader_NEW(const std::string& source);
            ~WebShader_NEW();

            int getAttribLocation(const char* name) const;
            int getUniformLocation(const char* name) const;
            void setUniform(int location, const mat4& matrix) const;
            void setUniform(int location, const vec3& v) const;
            void setUniform(int location, float val) const;
            void setUniform1i(int location, int val) const;

            inline uint32_t getID() const { return _shaderID; }


        private:
            void getUniformLocations();

        };
    }
}
