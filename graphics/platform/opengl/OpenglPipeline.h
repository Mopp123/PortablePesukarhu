#pragma once

#include "graphics/Pipeline.h"
#include "shaders/OpenglShader.h"


namespace pk
{
    namespace opengl
    {
        class OpenglPipeline : public Pipeline
        {
        private:
            std::vector<VertexBufferLayout> _vertexBufferLayouts;
            std::vector<DescriptorSetLayout> _descriptorLayouts;
            float _viewportWidth = 0.0f;
            float _viewportHeight = 0.0f;

            OpenglShaderProgram _shaderProgram;

        public:
            OpenglPipeline(const OpenglPipeline&) = delete;
            ~OpenglPipeline() {}

        protected:
            // TODO: Make shader modules' sources exist until opengl shader program created and
            // vertex attrib + uniform locations have been found!
            //
            // NOTE: Default copying should work for those layouts here..
            OpenglPipeline(
                const std::vector<VertexBufferLayout>& vertexBufferLayouts,
                const std::vector<DescriptorSetLayout>& descriptorLayouts,
                const Shader* pVertexShader, const Shader* pFragmentShader,
                float viewportWidth, float viewportHeight,
                const Rect2D viewportScissor
            );
        };
    }
}
