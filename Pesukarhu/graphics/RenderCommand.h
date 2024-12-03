#pragma once

#include "CommandBuffer.h"
#include "Pipeline.h"
#include "Buffers.h"
#include "Descriptors.h"
#include "Pesukarhu/utils/pkmath.h"
#include <vector>


namespace pk
{
    class RenderCommand
    {
    private:
        static RenderCommand* s_pInstance;

    public:
        RenderCommand(const RenderCommand& other) = delete;
        virtual ~RenderCommand() {}

        virtual void beginRenderPass(vec4 clearColor) = 0;
        virtual void endRenderPass() = 0;

        // NOTE: atm just quick hack and only opengl specific!!!
        // TODO: figure out could the setViewport "cmd" replace this?
        virtual void resizeViewport(int width, int height) = 0;

        virtual void beginCmdBuffer(CommandBuffer* pCmdBuf) = 0;
        virtual void endCmdBuffer(CommandBuffer* pCmdBuf) = 0;

        virtual void setViewport(
            CommandBuffer* pCmdBuf,
            float x,
            float y,
            float width,
            float height,
            float minDepth,
            float maxDepth
        ) = 0;

        virtual void bindPipeline(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            Pipeline* pPipeline
        ) = 0;

        virtual void bindIndexBuffer(
            CommandBuffer* pCmdBuf,
            const Buffer* pBuffer,
            size_t offset,
            IndexType indexType
        ) = 0;

        virtual void bindVertexBuffers(
            CommandBuffer* pCmdBuf,
            uint32_t firstBinding,
            uint32_t bindingCount,
            const std::vector<Buffer*>& buffers
        ) = 0;

        // TODO: deal with "pipelineLayouts"
        virtual void bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<const DescriptorSet*>& descriptorSets
        ) = 0;

        virtual void pushConstants(
            CommandBuffer* pCmdBuf,
            //pipelineLayout,
            ShaderStageFlagBits shaderStageFlags,
            uint32_t offset,
            uint32_t size,
            const void* pValues,
            std::vector<UniformInfo> glUniformInfo // Only used on opengl side
        ) = 0;

        virtual void draw(
            CommandBuffer* pCmdBuf,
            uint32_t vertexCount,
            uint32_t instanceCount,
            uint32_t firstVertex,
            uint32_t firstInstance
        ) = 0;

        virtual void drawIndexed(
            CommandBuffer* pCmdBuf,
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        ) = 0;

        static RenderCommand* get();

    protected:
        RenderCommand() {}

    private:
        static RenderCommand* create();
    };
}
