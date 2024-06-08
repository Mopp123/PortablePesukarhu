#include "WebRenderCommand.h"
#include <GL/glew.h>
#include <GL/glext.h>
#include "core/Debug.h"
#include "graphics/Buffers.h"
#include "graphics/platform/opengl/OpenglPipeline.h"
#include "WebBuffers.h"
#include "graphics/platform/opengl/shaders/OpenglShader.h"
#include "graphics/platform/opengl/OpenglContext.h"


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
        }

        void WebRenderCommand::endCmdBuffer(CommandBuffer* pCmdBuf)
        {
            // Make sure this cmd buf is unable to touch any pipeline until calling bindPipeline() again
            ((WebCommandBuffer*)pCmdBuf)->_pPipeline = nullptr;
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
            glViewport(0, 0, (int)width, (int)height);
            // TODO: Make this configurable through pipeline
            glFrontFace(GL_CCW);
        }

        void WebRenderCommand::bindPipeline(
            CommandBuffer* pCmdBuf, // Might cause some problems tho.. figure that out.. bitch..
            PipelineBindPoint pipelineBindPoint,
            Pipeline* pPipeline
        )
        {
            // TODO: make sure no nullptrs provided?
            opengl::OpenglPipeline* glPipeline = (opengl::OpenglPipeline*)pPipeline;
            ((WebCommandBuffer*)pCmdBuf)->_pPipeline = glPipeline;

            glUseProgram(glPipeline->getShaderProgram().getID());

            if (glPipeline->getEnableDepthTest())
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);

            switch(glPipeline->getDepthCompareOperation())
            {
                case DepthCompareOperation::COMPARE_OP_NEVER:
                    glDepthFunc(GL_NEVER);
                    break;
                case DepthCompareOperation::COMPARE_OP_LESS:
                    glDepthFunc(GL_LESS);
                    break;
                case DepthCompareOperation::COMPARE_OP_EQUAL:
                    glDepthFunc(GL_EQUAL);
                    break;
                case DepthCompareOperation::COMPARE_OP_LESS_OR_EQUAL:
                    glDepthFunc(GL_LEQUAL);
                    break;
                case DepthCompareOperation::COMPARE_OP_GREATER:
                    glDepthFunc(GL_GREATER);
                    break;
                case DepthCompareOperation::COMPARE_OP_NOT_EQUAL:
                    glDepthFunc(GL_NOTEQUAL);
                    break;
                case DepthCompareOperation::COMPARE_OP_GREATER_OR_EQUAL:
                    glDepthFunc(GL_GEQUAL);
                    break;
                case DepthCompareOperation::COMPARE_OP_ALWAYS:
                    glDepthFunc(GL_ALWAYS);
                    break;
            }

            switch (glPipeline->getCullMode())
            {
                case CullMode::CULL_MODE_NONE:
                    glDisable(GL_CULL_FACE);
                    break;
                case CullMode::CULL_MODE_FRONT:
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                    break;
                case CullMode::CULL_MODE_BACK:
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    break;
                default:
                    Debug::log(
                        "@WebRenderCommand::bindPipeline invalid pipeline cull mode",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }
        }

        void WebRenderCommand::bindIndexBuffer(
            CommandBuffer* pCmdBuf,
            Buffer* pBuffer,
            size_t offset,
            IndexType indexType
        )
        {
            // NOTE: DANGER! :D
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((WebBuffer*)pBuffer)->getID());
        }

        void WebRenderCommand::bindVertexBuffers(
            CommandBuffer* pCmdBuf,
            uint32_t firstBinding,
            uint32_t bindingCount,
            const std::vector<Buffer*>& buffers
        )
        {
            opengl::OpenglPipeline* pipeline = ((WebCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram& shaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderAttribLocations = shaderProgram.getAttribLocations();
            const std::vector<VertexBufferLayout>& vbLayouts = pipeline->getVertexBufferLayouts();

            // Not sure if this stuff works here well...
            std::vector<VertexBufferLayout>::const_iterator vbIt = vbLayouts.begin();

            int i = 0;
            for (Buffer* buffer : buffers)
            {
            #ifdef PK_DEBUG_FULL
                // Crash is intended here..
                if (vbIt == vbLayouts.end())
                    Debug::log(
                        "@WebRenderCommand::bindVertexBuffers No layout exists for inputted buffer",
                        Debug::MessageType::PK_FATAL_ERROR
                    )
            #endif
                // NOTE: DANGER! ..again
                glBindBuffer(GL_ARRAY_BUFFER, ((WebBuffer*)buffer)->getID());

                // Currently assuming that each pipeline's vb layout's index
                // corresponds to the order of inputted buffers vector
                // TODO: Some safeguards 'n error handling if this goes fucked..
                int32_t stride = (int32_t)buffer->getDataElemSize() * (int32_t)buffer->getDataLength();
                for (const VertexBufferElement& elem : vbIt->getElements())
                {
                    int32_t location = shaderAttribLocations[i];
                    ShaderDataType elemShaderDataType = elem.getType();

                    glEnableVertexAttribArray(location);
                    // TODO: element's Shader data type to gl type
                    glVertexAttribPointer(
                        location,
                        get_shader_data_type_component_count(elemShaderDataType),
                        opengl::to_gl_data_type(elemShaderDataType),
                        GL_FALSE,
                        stride,
                        0 // Don't remember why this was 0??
                    );
                }
                ++i;
                vbIt++;
            }
        }

        void WebRenderCommand::bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<DescriptorSet*>& descriptorSets
        )
        {
            #ifdef PK_DEBUG_FULL
            Debug::log(
                "@WebRenderCommand::bindDescriptorSets NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            #endif
        }

        void WebRenderCommand::draw(
            CommandBuffer* pCmdBuf,
            uint32_t vertexCount,
            uint32_t instanceCount,
            uint32_t firstVertex,
            uint32_t firstInstance
        )
        {
            #ifdef PK_DEBUG_FULL
            Debug::log(
                "@WebRenderCommand::draw NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            #endif
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
            #ifdef PK_DEBUG_FULL
            Debug::log(
                "@WebRenderCommand::drawIndexed NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            #endif
        }
    }
}
