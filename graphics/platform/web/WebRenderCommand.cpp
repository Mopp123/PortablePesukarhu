#include "WebRenderCommand.h"
#include <GL/glew.h>
#include "core/Debug.h"
#include "graphics/platform/opengl/OpenglPipeline.h"


namespace pk
{
    namespace web
    {
        void WebRenderCommand::beginFrame()
        {}

        void WebRenderCommand::beginRenderPass()
        {
            // NOTE: Not sure in which order these should be done..


	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glClearColor(0.0f, 0.0f, 0.0f, 1);
        }

        void WebRenderCommand::endRenderPass()
        {}

        void WebRenderCommand::endFrame()
        {}

        // NOTE: atm just quick hack and only opengl specific!!!
        void WebRenderCommand::resizeViewport(int width, int height)
        {
            glViewport(0, 0, width, height);
            glFrontFace(GL_CCW); // Not sure why this is here?
        }

        // the new stuff...
        void WebRenderCommand::beginCmdBuffer(CommandBuffer* pCmdBuf)
        {
            Debug::log(
                "@WebRenderCommand::beginCmdBuffer NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::endCmdBuffer(CommandBuffer* pCmdBuf)
        {
            Debug::log(
                "@WebRenderCommand::endCmdBuffer NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        // NOTE: the whole "scissor" thing is ignored atm
        void WebRenderCommand::setViewport(
            CommandBuffer* pCmdBuf,
            float x,
            float y,
            float width,
            float height,
            float minDepth,
            float maxDepth
        )
        {
            Debug::log(
                "@WebRenderCommand::setViewport NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::bindPipeline(
            CommandBuffer* pCmdBuf, // Might cause some problems tho.. figure that out.. bitch..
            PipelineBindPoint pipelineBindPoint,
            Pipeline* pPipeline
        )
        {
            // TODO: make sure no nullptrs provided?
            // TODO: some debug define with more checks like above!
            ((WebCommandBuffer*)pCmdBuf)->_pPipeline = (opengl::OpenglPipeline*)pPipeline;
            Debug::log(
                "@WebRenderCommand::bindPipeline NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::bindIndexBuffer(
            CommandBuffer* pCmdBuf,
            Buffer* pBuffer,
            size_t offset,
            IndexType indexType
        )
        {
            Debug::log(
                "@WebRenderCommand::bindIndexBuffer NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::bindVertexBuffers(
            CommandBuffer* pCmdBuf,
            uint32_t firstBinding,
            uint32_t bindingCount,
            const std::vector<Buffer*>& buffers
        )
        {
            Debug::log(
                "@WebRenderCommand::bindVertexBuffer NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<DescriptorSet*>& descriptorSets
        )
        {
            Debug::log(
                "@WebRenderCommand::bindDescriptorSets NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::draw(
            CommandBuffer* pCmdBuf,
            uint32_t vertexCount,
            uint32_t instanceCount,
            uint32_t firstVertex,
            uint32_t firstInstance
        )
        {
            Debug::log(
                "@WebRenderCommand::draw NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        void WebRenderCommand::drawIndexed(
            CommandBuffer* pCmdBuf,
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        )
        {
            Debug::log(
                "@WebRenderCommand::drawIndexed NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
    }
}
