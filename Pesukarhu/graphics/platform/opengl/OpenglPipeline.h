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
            friend class Pipeline;

            std::vector<VertexBufferLayout> _vertexBufferLayouts;
            std::vector<DescriptorSetLayout> _descriptorLayouts;
            float _viewportWidth = 0.0f;
            float _viewportHeight = 0.0f;

            OpenglShaderProgram* _pShaderProgram = nullptr;

            CullMode _cullMode;
            FrontFace _frontFace;

            bool _enableDepthTest = false;
            DepthCompareOperation _depthCmpOp;

            bool _enableColorBlending = false;

        public:
            OpenglPipeline(const OpenglPipeline&) = delete;
            ~OpenglPipeline() {}

            // TODO: Make shader modules' sources exist until opengl shader program created and
            // vertex attrib + uniform locations have been found!
            //
            // NOTE: Default copying should work for those layouts here..
            virtual void init(
                const std::vector<VertexBufferLayout>& vertexBufferLayouts,
                const std::vector<DescriptorSetLayout>& descriptorLayouts,
                const Shader* pVertexShader, const Shader* pFragmentShader,
                float viewportWidth, float viewportHeight,
                const Rect2D viewportScissor,
                CullMode cullMode,
                FrontFace frontFace,
                bool enableDepthTest,
                DepthCompareOperation depthCmpOp,
                bool enableColorBlending,
                uint32_t pushConstantSize,
	        uint32_t pushConstantStageFlags
            ) override;

            virtual void cleanUp() override;

            const std::vector<VertexBufferLayout>& getVertexBufferLayouts() const { return _vertexBufferLayouts; }
            const std::vector<DescriptorSetLayout>& getDescriptorSetLayouts(int index) const { return _descriptorLayouts; }

            inline const OpenglShaderProgram* getShaderProgram() const { return _pShaderProgram; }

            inline CullMode getCullMode() const { return _cullMode; }
            inline FrontFace getFrontFace() const { return _frontFace; }

            inline bool getEnableDepthTest() const { return _enableDepthTest; }
            inline DepthCompareOperation getDepthCompareOperation() const { return _depthCmpOp; }

            inline bool getEnableColorBlending() const { return _enableColorBlending; }

        protected:
            OpenglPipeline() {}
        };
    }
}
