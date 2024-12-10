#pragma once

#include "pesukarhu/graphics/RenderCommand.h"
#include "WebCommandBuffer.h"


namespace pk
{
    namespace web
    {
        class WebRenderCommand : public RenderCommand
        {
        public:
            ~WebRenderCommand() {}
            virtual void beginRenderPass(vec4 clearColor);
            virtual void endRenderPass();

            // NOTE: atm just quick hack and only opengl specific!!!
            virtual void resizeViewport(int width, int height);


            virtual void beginCmdBuffer(CommandBuffer* pCmdBuf);
            virtual void endCmdBuffer(CommandBuffer* pCmdBuf);

            virtual void setViewport(
                CommandBuffer* pCmdBuf,
                float x,
                float y,
                float width,
                float height,
                float minDepth,
                float maxDepth
            );

            virtual void bindPipeline(
                CommandBuffer* pCmdBuf,
                PipelineBindPoint pipelineBindPoint,
                Pipeline* pPipeline
            );

            virtual void bindIndexBuffer(
                CommandBuffer* pCmdBuf,
                const Buffer* pBuffer,
                size_t offset,
                IndexType indexType
            );

            virtual void bindVertexBuffers(
                CommandBuffer* pCmdBuf,
                uint32_t firstBinding,
                uint32_t bindingCount,
                const std::vector<Buffer*>& buffers
            );

            virtual void bindDescriptorSets(
                CommandBuffer* pCmdBuf,
                PipelineBindPoint pipelineBindPoint,
                // PipelineLayout pipelineLayout,
                uint32_t firstDescriptorSet,
                const std::vector<const DescriptorSet*>& descriptorSets
            );

            virtual void pushConstants(
                CommandBuffer* pCmdBuf,
                //pipelineLayout,
                ShaderStageFlagBits shaderStageFlags,
                uint32_t offset,
                uint32_t size,
                const void* pValues,
                std::vector<UniformInfo> glUniformInfo // Only used on opengl side
            );

            virtual void draw(
                CommandBuffer* pCmdBuf,
                uint32_t vertexCount,
                uint32_t instanceCount,
                uint32_t firstVertex,
                uint32_t firstInstance
            );

            virtual void drawIndexed(
                CommandBuffer* pCmdBuf,
                uint32_t indexCount,
                uint32_t instanceCount,
                uint32_t firstIndex,
                int32_t vertexOffset,
                uint32_t firstInstance
            );
        };
    }
}
