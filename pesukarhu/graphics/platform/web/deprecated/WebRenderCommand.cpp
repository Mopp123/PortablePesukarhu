#include "WebRenderCommand.h"
#include <GL/glew.h>
#include "pesukarhu/core/Debug.h"
#include "pesukarhu/graphics/Buffers.h"
#include "pesukarhu/graphics/platform/opengl/OpenglPipeline.h"
#include "pesukarhu/graphics/platform/opengl/shaders/OpenglShader.h"
#include "pesukarhu/graphics/platform/opengl/OpenglContext.h"
#include "pesukarhu/resources/platform/opengl/OpenglTexture.h"
#include "pesukarhu/core/Application.h"

#include "WebContext.h"
#include "pesukarhu/graphics/platform/opengl/OpenglBuffers.h"


namespace pk
{
    namespace web
    {
        static GLenum binding_to_gl_texture_unit(uint32_t binding)
        {
            const WebContext * const pContext = (WebContext*)Application::get()->getGraphicsContext();
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
                case 7:
                    return GL_TEXTURE7;
                case 8:
                    return GL_TEXTURE8;
                case 9:
                    return GL_TEXTURE9;
                default:
                    Debug::log(
                        "@binding_to_gl_texture_unit "
                        "Max texture units(" + std::to_string(pContext->getMaxTextureUnits()) + ") exceeded!"
                        "Requested unit: " + std::to_string(binding),
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

        void WebRenderCommand::beginRenderPass(vec4 clearColor)
        {
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        }

        void WebRenderCommand::endRenderPass()
        {}

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
            glViewport(x, y, (int)width, (int)height);
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

            glUseProgram(glPipeline->getShaderProgram()->getID());

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

            if (glPipeline->getEnableColorBlending())
            {
                glEnable(GL_BLEND);
                // TODO: allow specifying this
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                glDisable(GL_BLEND);
            }
        }

        void WebRenderCommand::bindIndexBuffer(
            CommandBuffer* pCmdBuf,
            const Buffer* pBuffer,
            size_t offset,
            IndexType indexType
        )
        {
            if (indexType != IndexType::INDEX_TYPE_UINT16 && indexType != IndexType::INDEX_TYPE_UINT32)
            {
                Debug::log(
                    "@WebRenderCommand::bindIndexBuffer "
                    "invalid indexType: " + std::to_string(indexType) + " "
                    "indexType is required to be either INDEX_TYPE_UINT16 or INDEX_TYPE_UINT32",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            // quite dumb, but we need to be able to pass this to "drawIndexed" func somehow..
            ((WebCommandBuffer*)pCmdBuf)->_drawIndexedType = indexType;

            // NOTE: DANGER! :D
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((OpenglBuffer*)pBuffer)->getID());
        }

        void WebRenderCommand::bindVertexBuffers(
            CommandBuffer* pCmdBuf,
            uint32_t firstBinding,
            uint32_t bindingCount,
            const std::vector<Buffer*>& buffers
        )
        {
            opengl::OpenglPipeline* pipeline = ((WebCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram* pShaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderAttribLocations = pShaderProgram->getAttribLocations();
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
                OpenglBuffer* pOpenglBuffer = (OpenglBuffer*)buffer;
                glBindBuffer(GL_ARRAY_BUFFER, pOpenglBuffer->getID());

                // Update gl buf immediately if marked
                if (pOpenglBuffer->_shouldUpdate)
                {
                    // Doesn't work atm because _updateOffset is just the latest offset..
                    // TODO: Some way to call glBufferData and glBufferSubData immediately when buffer::update is called!
                    /*
                    if (pOpenglBuffer->getUpdateFrequency() == BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC)
                    {
                        glBufferSubData(
                            GL_ARRAY_BUFFER,
                            pOpenglBuffer->_updateOffset,
                            pOpenglBuffer->_updateSize,
                            pOpenglBuffer->_data
                        );
                    }*/
                    glBufferData(
                        GL_ARRAY_BUFFER,
                        pOpenglBuffer->getTotalSize(),
                        pOpenglBuffer->_data,
                        pOpenglBuffer->getUpdateFrequency() == BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW
                    );
                    pOpenglBuffer->_shouldUpdate = false;
                    pOpenglBuffer->_updateOffset = 0;
                    pOpenglBuffer->_updateSize = 0;
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

                    if (elemShaderDataType != ShaderDataType::Mat4)
                    {
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
                    // Special case on matrices since on opengl those are set as 4 vec4s
                    else
                    {
                        for (int i = 0; i < 4; ++i)
                        {
                            glEnableVertexAttribArray(location + i);
                            glVertexAttribDivisor(
                                location + i,
                                vbLayoutIt->getInputRate() == VertexInputRate::VERTEX_INPUT_RATE_INSTANCE ? 1 : 0
                            );
                            glVertexAttribPointer(
                                location + i,
                                4,
                                opengl::to_gl_data_type(ShaderDataType::Float4),
                                GL_FALSE,
                                stride,
                                (const void*)toNext
                            );
                            toNext += get_shader_data_type_size(ShaderDataType::Float4);
                        }
                    }
                }
                vbLayoutIt++;
            }
        }

        void WebRenderCommand::bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<const DescriptorSet*>& descriptorSets
        )
        {
            opengl::OpenglPipeline* pipeline = ((WebCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram* pShaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderUniformLocations = pShaderProgram->getUniformLocations();

            int descriptorSetIndex = 0;
            for (const DescriptorSet* descriptorSet : descriptorSets)
            {
                const DescriptorSetLayout& layout = descriptorSet->getLayout();
                // Validate that this descriptor set is layout compliant
                if (!descriptorSet->isValid(layout))
                {
                    Debug::log(
                        "@WebRenderCommand::bindDescriptorSets "
                        "Descriptor set at index: " + std::to_string(descriptorSetIndex) + " "
                        "not layout compliant",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    continue;
                }
                ++descriptorSetIndex;

                const std::vector<const Buffer*>& buffers = descriptorSet->getBuffers();
                const std::vector<const Texture*>& textures = descriptorSet->getTextures();
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
                                    int val = *(int*)pCurrentData;
                                    valSize = sizeof(int);
                                    glUniform1i(shaderUniformLocations[uboInfo.locationIndex], val);
                                    break;
                                }
                                case ShaderDataType::Float:
                                {
                                    float val = *(float*)pCurrentData;
                                    valSize = sizeof(float);
                                    glUniform1f(shaderUniformLocations[uboInfo.locationIndex], val);
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
                                    valSize = sizeof(mat4) * uboInfo.arrayLen;
                                    /*
                                    glUniformMatrix4fv(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        uboInfo.arrayLen,
                                        GL_FALSE,
                                        (const float*)pCurrentData
                                    );
                                    */
                                    for (int i = 0; i < uboInfo.arrayLen; ++i)
                                    {
                                        const PK_byte* pData = pCurrentData + i * sizeof(mat4);
                                        mat4 test;
                                        memcpy(&test, pData, sizeof(mat4));
                                        //Debug::log("___TEST___sending mat to: " + std::to_string(shaderUniformLocations[uboInfo.locationIndex + i]));
                                        glUniformMatrix4fv(
                                            shaderUniformLocations[uboInfo.locationIndex + i],
                                            1,
                                            GL_FALSE,
                                            (const float*)pData
                                        );
                                    }
                                    break;
                                }

                                default:
                                    Debug::log(
                                        "@WebRenderCommand::bindDescriptorSets "
                                        "Unsupported ShaderDataType: " + std::to_string(uboInfo.type) + " "
                                        "using location index: " + std::to_string(uboInfo.locationIndex) + " "
                                        "Currently implemented types are: "
                                        "Int, Float, Float2, Float3, Float4, Mat4",
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
                            #ifdef PK_DEBUG_FULL
                            if (!textures[textureBindingIndex])
                            {
                                Debug::log(
                                    "@WebRenderCommand::bindDescriptorSets "
                                    "Texture at binding index: " + std::to_string(textureBindingIndex) + " was nullptr",
                                    Debug::MessageType::PK_FATAL_ERROR
                                );
                            }
                            #endif
                            glUniform1i(shaderUniformLocations[layoutInfo.locationIndex], binding.getBinding());
                            // well following is quite fucking dumb.. dunno how could do this better
                            glActiveTexture(binding_to_gl_texture_unit(binding.getBinding()));
                            glBindTexture(
                                GL_TEXTURE_2D,
                                ((opengl::OpenglTexture*)textures[textureBindingIndex])->getGLTexID()
                            );
                            ++textureBindingIndex;
                        }
                    }
                }
            }
        }

        void WebRenderCommand::pushConstants(
            CommandBuffer* pCmdBuf,
            //pipelineLayout,
            ShaderStageFlagBits shaderStageFlags,
            uint32_t offset,
            uint32_t size,
            const void* pValues,
            std::vector<UniformInfo> glUniformInfo // Only used on opengl side
        )
        {
            opengl::OpenglPipeline* pipeline = ((WebCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram* pShaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderUniformLocations = pShaderProgram->getUniformLocations();

            PK_byte* pBuf = (PK_byte*)pValues;
            size_t bufOffset = 0;
            for (const UniformInfo& uInfo : glUniformInfo)
            {
                if (bufOffset >= size)
                {
                    Debug::log(
                        "@WebRenderCommand::pushConstants "
                        "Buffer offset out of bounds!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return;
                }
                const PK_byte* pCurrentData = pBuf + bufOffset;
                switch (uInfo.type)
                {
                    case ShaderDataType::Int:
                    {
                        int val = *(int*)pCurrentData;
                        glUniform1i(shaderUniformLocations[uInfo.locationIndex], val);
                        break;
                    }
                    case ShaderDataType::Float:
                    {
                        float val = *(float*)pCurrentData;
                        glUniform1f(shaderUniformLocations[uInfo.locationIndex], val);
                        break;
                    }
                    case ShaderDataType::Float2:
                    {
                        vec2 vec = *(vec2*)pCurrentData;
                        glUniform2f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y
                        );
                        break;
                    }
                    case ShaderDataType::Float3:
                    {
                        vec3 vec = *(vec3*)pCurrentData;;
                        glUniform3f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y,
                            vec.z
                        );
                        break;
                    }
                    case ShaderDataType::Float4:
                    {
                        vec4 vec = *(vec4*)pCurrentData;
                        glUniform4f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y,
                            vec.z,
                            vec.w
                        );
                        break;
                    }
                    case ShaderDataType::Mat4:
                    {
                        mat4 matrix = *(mat4*)pCurrentData;
                        glUniformMatrix4fv(
                            shaderUniformLocations[uInfo.locationIndex],
                            1,
                            GL_FALSE,
                            (const float*)&matrix
                        );
                        break;
                    }

                    default:
                        Debug::log(
                            "@WebRenderCommand::pushConstants "
                            "Unsupported ShaderDataType. "
                            "Currently implemented types are: "
                            "Int, Float, Float2, Float3, Float4, Mat4",
                            Debug::MessageType::PK_FATAL_ERROR
                        );
                        break;
                }
                bufOffset += get_shader_data_type_size(uInfo.type);
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
