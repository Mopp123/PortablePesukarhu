#include "WebRenderCommand.h"
#include <GL/glew.h>
#include "core/Debug.h"
#include "graphics/Buffers.h"
#include "graphics/platform/opengl/OpenglPipeline.h"
#include "WebBuffers.h"
#include "graphics/platform/opengl/shaders/OpenglShader.h"
#include "graphics/platform/opengl/OpenglContext.h"
#include "graphics/platform/opengl/OpenglTexture.h"


namespace pk
{
    namespace web
    {
        static GLenum binding_to_gl_texture_slot(uint32_t binding)
        {
            switch (binding)
            {
                case 0:
                    return GL_TEXTURE0;
                case 1:
                    return GL_TEXTURE1;
                case 2:
                    return GL_TEXTURE2;
                case 3:
                    return GL_TEXTURE3;
                case 4:
                    return GL_TEXTURE4;
                case 5:
                    return GL_TEXTURE5;
                case 6:
                    return GL_TEXTURE6;
                default:
                    Debug::log(
                        "@binding_to_gl_texture_slot Max binding number exceeded",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
            }
        }


        GLenum index_type_to_glenum(IndexType type)
        {
            switch (type)
            {
                case IndexType::INDEX_TYPE_NONE:
                    Debug::log(
                        "@index_type_to_glenum IndexType was not set",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
                case IndexType::INDEX_TYPE_UINT16:
                    return GL_UNSIGNED_SHORT;
                case IndexType::INDEX_TYPE_UINT32:
                    return GL_UNSIGNED_INT;
                default:
                    Debug::log(
                        "@index_type_to_glenum Invalid index type",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return 0;
            }
        }

        void WebRenderCommand::beginRenderPass()
        {
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glClearColor(0.0f, 0.0f, 0.0f, 1);
        }

        void WebRenderCommand::endRenderPass()
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
            // unbind all
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Make sure this cmd buf is unable to touch any pipeline until calling bindPipeline() again
            ((WebCommandBuffer*)pCmdBuf)->_pPipeline = nullptr;
            ((WebCommandBuffer*)pCmdBuf)->_drawIndexedType = IndexType::INDEX_TYPE_NONE;
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
            FrontFace frontFace = ((WebCommandBuffer*)pCmdBuf)->_pPipeline->getFrontFace();
            glFrontFace(frontFace == FrontFace::FRONT_FACE_COUNTER_CLOCKWISE ? GL_CCW : GL_CW);
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
            // quite dumb, but we need to be able to pass this to "drawIndexed" func somehow..
            ((WebCommandBuffer*)pCmdBuf)->_drawIndexedType = indexType;

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
            std::vector<VertexBufferLayout>::const_iterator vbLayoutIt = vbLayouts.begin();

            for (Buffer* buffer : buffers)
            {
            #ifdef PK_DEBUG_FULL
                // Crash is intended here..
                if (vbLayoutIt == vbLayouts.end())
                    Debug::log(
                        "@WebRenderCommand::bindVertexBuffers No layout exists for inputted buffer",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                if (buffer->getBufferUsage() != BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT)
                    Debug::log(
                        "@WebRenderCommand::bindVertexBuffers Invalid buffer usage: " + std::to_string(buffer->getBufferUsage()) + " "
                        "Web rendering context can only have buffers with single type of usage!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
            #endif
                // NOTE: DANGER! ..again
                WebBuffer* pWebBuffer = (WebBuffer*)buffer;
                glBindBuffer(GL_ARRAY_BUFFER, pWebBuffer->getID());

                // Update gl buf immediately if marked
                if (pWebBuffer->_shouldUpdate)
                {
                    // TODO: Allow specifying GLenum usage(GL_STATIC_DRAW, etc..) (not to be confused with the BufferUsage)
                    glBufferData(GL_ARRAY_BUFFER, pWebBuffer->getTotalSize(), pWebBuffer->_data, GL_STATIC_DRAW);
                    pWebBuffer->_shouldUpdate = false;
                }

                // Currently assuming that each pipeline's vb layout's index
                // corresponds to the order of inputted buffers vector
                // TODO: Some safeguards 'n error handling if this goes fucked..

                size_t stride = vbLayoutIt->getStride();
                size_t toNext = 0;

                for (const VertexBufferElement& elem : vbLayoutIt->getElements())
                {
                    int32_t location = shaderAttribLocations[elem.getLocation()];
                    ShaderDataType elemShaderDataType = elem.getType();

                    glEnableVertexAttribArray(location);
                    glVertexAttribDivisor(
                        location,
                        vbLayoutIt->getInputRate() == VertexInputRate::VERTEX_INPUT_RATE_INSTANCE ? 1 : 0
                    );
                    glVertexAttribPointer(
                        location,
                        get_shader_data_type_component_count(elemShaderDataType),
                        opengl::to_gl_data_type(elemShaderDataType),
                        GL_FALSE,
                        stride,
                        (const void*)toNext
                    );
                    toNext += get_shader_data_type_size(elemShaderDataType);
                }
                vbLayoutIt++;
            }
        }

        // NOTE: Descriptor sets has to be given in the same order as
        // corresponding uniforms are in the actual shader code for this to work!!!
        // UPDATE TO ABOVE:
        //  -> this may not be the case after added the UniformInfo's locationIndex
        void WebRenderCommand::bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<DescriptorSet*>& descriptorSets
        )
        {
            opengl::OpenglPipeline* pipeline = ((WebCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram& shaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderUniformLocations = shaderProgram.getUniformLocations();

            for (const DescriptorSet* descriptorSet : descriptorSets)
            {
                const DescriptorSetLayout& layout = descriptorSet->getLayout();
                const std::vector<Buffer*>& buffers = descriptorSet->getBuffers();
                const std::vector<Texture_new*>& textures = descriptorSet->getTextures();
                // Not to be confused with binding number.
                // This just index of the layout's bindings vector
                int bufferBindingIndex = 0;
                int textureBindingIndex = 0;

                for (const DescriptorSetLayoutBinding& binding : layout.getBindings())
                {
                    const std::vector<UniformInfo>& uniformInfo = binding.getUniformInfo();

                    if (binding.getType() == DescriptorType::DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                    {
                        // TODO: some boundary checking..
                        const Buffer* pBuf = buffers[bufferBindingIndex];
                        const PK_byte* pBufData = (const PK_byte*)pBuf->getData();
                        size_t uboOffset = 0;
                        for (const UniformInfo& uboInfo : uniformInfo)
                        {
                            size_t valSize = 0;
                            const PK_byte* pCurrentData = pBufData + uboOffset;
                            switch (uboInfo.type)
                            {
                                case ShaderDataType::Int:
                                {
                                    int val = (int)*pCurrentData;
                                    valSize = sizeof(int);
                                    glUniform1i(shaderUniformLocations[uboInfo.locationIndex], val);
                                    break;
                                }
                                case ShaderDataType::Float:
                                {
                                    float val = (float)*pCurrentData;
                                    valSize = sizeof(float);
                                    glUniform1fv(shaderUniformLocations[uboInfo.locationIndex], 1, &val);
                                    break;
                                }
                                case ShaderDataType::Float2:
                                {
                                    vec2 vec;
                                    valSize = sizeof(vec2);
                                    memcpy(&vec, pCurrentData, valSize);

                                    glUniform2f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y
                                    );
                                    break;
                                }
                                case ShaderDataType::Float3:
                                {
                                    vec3 vec;
                                    valSize = sizeof(vec3);
                                    memcpy(&vec, pCurrentData, valSize);
                                    glUniform3f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y,
                                        vec.z
                                    );
                                    break;
                                }
                                case ShaderDataType::Float4:
                                {
                                    vec4 vec;
                                    valSize = sizeof(vec4);
                                    memcpy(&vec, pCurrentData, valSize);

                                    glUniform4f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y,
                                        vec.z,
                                        vec.w
                                    );
                                    break;
                                }
                                case ShaderDataType::Mat4:
                                {
                                    mat4 matrix;
                                    valSize = sizeof(mat4);
                                    memcpy(&matrix, pCurrentData, valSize);
                                    glUniformMatrix4fv(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        1,
                                        GL_FALSE,
                                        (const float*)&matrix
                                    );
                                    break;
                                }

                                default:
                                    Debug::log(
                                        "@WebRenderCommand::bindDescriptorSets "
                                        "Unsupported ShaderDataType. "
                                        "Currently implemented types are: "
                                        "Int, Float, Float2, Float3, Float4",
                                        Debug::MessageType::PK_FATAL_ERROR
                                    );
                                    break;
                            }
                            uboOffset += valSize;
                        }
                        ++bufferBindingIndex;
                    }
                    else if (binding.getType() == DescriptorType::DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                    {
                        // TODO: some boundary checking..
                        for (const UniformInfo& layoutInfo : uniformInfo)
                        {
                            glUniform1i(shaderUniformLocations[layoutInfo.locationIndex], binding.getBinding());
                            // well following is quite fucking dumb.. dunno how could do this better
                            glActiveTexture(binding_to_gl_texture_slot(binding.getBinding()));
                            glBindTexture(
                                GL_TEXTURE_2D,
                                ((opengl::OpenglTexture*)textures[textureBindingIndex])->getID()
                            );
                            ++textureBindingIndex;
                        }
                    }
                }
            }
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
            const IndexType& indexType = ((WebCommandBuffer*)pCmdBuf)->_drawIndexedType;
            // NOTE: Don't remember why not giving the ptr to the indices here..
            //glDrawElements(GL_TRIANGLES, indexCount, index_type_to_glenum(indexType), 0);
            glDrawElementsInstanced(GL_TRIANGLES, indexCount, index_type_to_glenum(indexType), 0, instanceCount);
        }
    }
}
