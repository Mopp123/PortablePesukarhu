#pragma once

#include "Pesukarhu/graphics/shaders/Shader.h"
#include "Pesukarhu/utils/pkmath.h"
#include <vector>


namespace pk
{
    namespace opengl
    {
        // NOTE: Used to find attrib and uniform locations with ESSL1 so no need to
        // have this anywhere else..
        enum class ShaderResourceType
        {
            ATTRIB,
            UNIFORM
        };


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
            inline const std::string& getSource() const { return _source; }
            inline void clearSource() { _source.clear(); }
        };


        class OpenglShaderProgram
        {
        private:
            const OpenglShader* _pVertexShader;
            const OpenglShader* _pFragmentShader;
            uint32_t _programID = 0;

            // Attrib locations in order of occurance in source code
            // NOTE: if GLSL Layout Qualifiers available this shouldn't be used!
            std::vector<int32_t> _attribLocations;
            std::vector<int32_t> _uniformLocations;

        public:
            OpenglShaderProgram(ShaderVersion shaderVersion, const OpenglShader* pVertexShader, const OpenglShader* pFragmentShader);
            ~OpenglShaderProgram();

            inline uint32_t getID() const { return _programID; }

            // TODO: delete commented out?
            //  -> reason: atm we started getting these automatically
            //  and started just returning the _attribLocations and _uniformLocations
            //int32_t getAttribLocation(const char* name) const;
            //int32_t getUniformLocation(const char* name) const;
            void setUniform(int location, const mat4& matrix) const;
            void setUniform(int location, const vec3& v) const;
            void setUniform(int location, float val) const;
            void setUniform1i(int location, int val) const;

            inline const std::vector<int32_t>& getAttribLocations() const { return _attribLocations; }
            inline const std::vector<int32_t>& getUniformLocations() const { return _uniformLocations; }

        private:
            // Finds attrib and/or uniform locations from shader source
            // NOTE: Works only with Opengl ES Shading language v1 (no layout qualifiers)
            void findLocationsESSL1(const std::string shaderSource);
        };
    }
}
