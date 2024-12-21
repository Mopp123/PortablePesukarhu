#include "OpenglRenderCommand.h"
#include <GL/glew.h>
#include "pesukarhu/core/Debug.h"
#include "pesukarhu/graphics/Buffers.h"
#include "OpenglPipeline.h"
#include "OpenglCommandBuffer.h"
#include "shaders/OpenglShader.h"
#include "OpenglContext.h"
#include "OpenglBuffers.h"
#include "pesukarhu/resources/platform/opengl/OpenglTexture.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace opengl
    {
        static GLenum binding_to_gl_texture_unit(uint32_t binding)
        {
            const OpenglContext * const pContext = (OpenglContext*)Application::get()->getGraphicsContext();
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

        void OpenglRenderCommand::beginRenderPass(vec4 clearColor)
        {
            GL_FUNC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            GL_FUNC(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));
        }

        void OpenglRenderCommand::endRenderPass()
        {}

        // the new stuff...
        void OpenglRenderCommand::beginCmdBuffer(CommandBuffer* pCmdBuf)
        {
        }

        void OpenglRenderCommand::endCmdBuffer(CommandBuffer* pCmdBuf)
        {
            // unbind all
            GL_FUNC(glActiveTexture(GL_TEXTURE0));
            GL_FUNC(glBindTexture(GL_TEXTURE_2D, 0));
            GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, 0));

            // Make sure this cmd buf is unable to touch any pipeline until calling bindPipeline() again
            ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline = nullptr;
            ((OpenglCommandBuffer*)pCmdBuf)->_drawIndexedType = IndexType::INDEX_TYPE_NONE;
        }

        // NOTE: This should be handled by pipeline!
        // -> not necessary to have separate specific cmd
        //  -> as in Vulkan you can specify this in Pipeline as well...
        // NOTE: the whole "scissor" thing is ignored atm
        void OpenglRenderCommand::setViewport(
            CommandBuffer* pCmdBuf,
            float x,
            float y,
            float width,
            float height,
            float minDepth,
            float maxDepth
        )
        {
            GL_FUNC(glViewport(x, y, (int)width, (int)height));
            OpenglPipeline* pPipeline = ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline;
            if (!pPipeline)
            {
                Debug::log(
                    "@OpenglRenderCommand::setViewport "
                    "No pipeline assigned for command buffer!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            // NOTE: Why the fuck is the front face specified here!!??!? Has nothing to do with viewport!?
            FrontFace frontFace = pPipeline->getFrontFace();
            GL_FUNC(glFrontFace(frontFace == FrontFace::FRONT_FACE_COUNTER_CLOCKWISE ? GL_CCW : GL_CW));
        }

        void OpenglRenderCommand::bindPipeline(
            CommandBuffer* pCmdBuf, // Might cause some problems tho.. figure that out.. bitch..
            PipelineBindPoint pipelineBindPoint,
            Pipeline* pPipeline
        )
        {
            // TODO: make sure no nullptrs provided?
            opengl::OpenglPipeline* glPipeline = (opengl::OpenglPipeline*)pPipeline;
            ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline = glPipeline;

            GL_FUNC(glUseProgram(glPipeline->getShaderProgram()->getID()));

            if (glPipeline->getEnableDepthTest())
            {
                GL_FUNC(glEnable(GL_DEPTH_TEST));
            }
            else
            {
                GL_FUNC(glDisable(GL_DEPTH_TEST));
            }

            switch(glPipeline->getDepthCompareOperation())
            {
                case DepthCompareOperation::COMPARE_OP_NEVER:
                    GL_FUNC(glDepthFunc(GL_NEVER));
                    break;
                case DepthCompareOperation::COMPARE_OP_LESS:
                    GL_FUNC(glDepthFunc(GL_LESS));
                    break;
                case DepthCompareOperation::COMPARE_OP_EQUAL:
                    GL_FUNC(glDepthFunc(GL_EQUAL));
                    break;
                case DepthCompareOperation::COMPARE_OP_LESS_OR_EQUAL:
                    GL_FUNC(glDepthFunc(GL_LEQUAL));
                    break;
                case DepthCompareOperation::COMPARE_OP_GREATER:
                    GL_FUNC(glDepthFunc(GL_GREATER));
                    break;
                case DepthCompareOperation::COMPARE_OP_NOT_EQUAL:
                    GL_FUNC(glDepthFunc(GL_NOTEQUAL));
                    break;
                case DepthCompareOperation::COMPARE_OP_GREATER_OR_EQUAL:
                    GL_FUNC(glDepthFunc(GL_GEQUAL));
                    break;
                case DepthCompareOperation::COMPARE_OP_ALWAYS:
                    GL_FUNC(glDepthFunc(GL_ALWAYS));
                    break;
            }

            switch (glPipeline->getCullMode())
            {
                case CullMode::CULL_MODE_NONE:
                    GL_FUNC(glDisable(GL_CULL_FACE));
                    break;
                case CullMode::CULL_MODE_FRONT:
                    GL_FUNC(glEnable(GL_CULL_FACE));
                    GL_FUNC(glCullFace(GL_FRONT));
                    break;
                case CullMode::CULL_MODE_BACK:
                    GL_FUNC(glEnable(GL_CULL_FACE));
                    GL_FUNC(glCullFace(GL_BACK));
                    break;
                default:
                    Debug::log(
                        "@OpenglRenderCommand::bindPipeline invalid pipeline cull mode",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    break;
            }

            if (glPipeline->getEnableColorBlending())
            {
                GL_FUNC(glEnable(GL_BLEND));
                // TODO: allow specifying this
                GL_FUNC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            }
            else
            {
                GL_FUNC(glDisable(GL_BLEND));
            }
        }

        void OpenglRenderCommand::bindIndexBuffer(
            CommandBuffer* pCmdBuf,
            const Buffer* pBuffer,
            size_t offset,
            IndexType indexType
        )
        {
            if (indexType != IndexType::INDEX_TYPE_UINT16 && indexType != IndexType::INDEX_TYPE_UINT32)
            {
                Debug::log(
                    "@OpenglRenderCommand::bindIndexBuffer "
                    "invalid indexType: " + std::to_string(indexType) + " "
                    "indexType is required to be either INDEX_TYPE_UINT16 or INDEX_TYPE_UINT32",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            // quite dumb, but we need to be able to pass this to "drawIndexed" func somehow..
            ((OpenglCommandBuffer*)pCmdBuf)->_drawIndexedType = indexType;

            // NOTE: DANGER! :D
            GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((OpenglBuffer*)pBuffer)->getID()));
        }

        void OpenglRenderCommand::bindVertexBuffers(
            CommandBuffer* pCmdBuf,
            uint32_t firstBinding,
            uint32_t bindingCount,
            const std::vector<Buffer*>& buffers
        )
        {
            opengl::OpenglPipeline* pipeline = ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline;
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
                        "@OpenglRenderCommand::bindVertexBuffers "
                        "No layout exists for inputted buffer",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                if (buffer->getBufferUsage() != BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT)
                    Debug::log(
                        "@OpenglRenderCommand::bindVertexBuffers "
                        "Invalid buffer usage: " + std::to_string(buffer->getBufferUsage()) + " "
                        "OpenGL rendering context can only have buffers with single type of usage!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
            #endif
                // NOTE: DANGER! ..again
                OpenglBuffer* pGLBuffer = (OpenglBuffer*)buffer;
                GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, pGLBuffer->getID()));

                // Update gl buf immediately if marked
                if (pGLBuffer->_shouldUpdate)
                {
                    // Doesn't work atm because _updateOffset is just the latest offset..
                    // TODO: Some way to call glBufferData and glBufferSubData immediately when buffer::update is called!
                    /*
                    if (pWebBuffer->getUpdateFrequency() == BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC)
                    {
                        glBufferSubData(
                            GL_ARRAY_BUFFER,
                            pWebBuffer->_updateOffset,
                            pWebBuffer->_updateSize,
                            pWebBuffer->_data
                        );
                    }*/
                    GL_FUNC(glBufferData(
                        GL_ARRAY_BUFFER,
                        pGLBuffer->getTotalSize(),
                        pGLBuffer->_data,
                        pGLBuffer->getUpdateFrequency() == BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW
                    ));
                    pGLBuffer->_shouldUpdate = false;
                    pGLBuffer->_updateOffset = 0;
                    pGLBuffer->_updateSize = 0;
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
                        GL_FUNC(glEnableVertexAttribArray(location));
                        GL_FUNC(glVertexAttribDivisor(
                            location,
                            vbLayoutIt->getInputRate() == VertexInputRate::VERTEX_INPUT_RATE_INSTANCE ? 1 : 0
                        ));
                        GL_FUNC(glVertexAttribPointer(
                            location,
                            get_shader_data_type_component_count(elemShaderDataType),
                            opengl::to_gl_data_type(elemShaderDataType),
                            GL_FALSE,
                            stride,
                            (const void*)toNext
                        ));
                        toNext += get_shader_data_type_size(elemShaderDataType);
                    }
                    // Special case on matrices since on opengl those are set as 4 vec4s
                    else
                    {
                        for (int i = 0; i < 4; ++i)
                        {
                            GL_FUNC(glEnableVertexAttribArray(location + i));
                            GL_FUNC(glVertexAttribDivisor(
                                location + i,
                                vbLayoutIt->getInputRate() == VertexInputRate::VERTEX_INPUT_RATE_INSTANCE ? 1 : 0
                            ));
                            GL_FUNC(glVertexAttribPointer(
                                location + i,
                                4,
                                opengl::to_gl_data_type(ShaderDataType::Float4),
                                GL_FALSE,
                                stride,
                                (const void*)toNext
                            ));
                            toNext += get_shader_data_type_size(ShaderDataType::Float4);
                        }
                    }
                }
                vbLayoutIt++;
            }
        }

        void OpenglRenderCommand::bindDescriptorSets(
            CommandBuffer* pCmdBuf,
            PipelineBindPoint pipelineBindPoint,
            // PipelineLayout pipelineLayout,
            uint32_t firstDescriptorSet,
            const std::vector<const DescriptorSet*>& descriptorSets
        )
        {
            opengl::OpenglPipeline* pipeline = ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline;
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
                        "@OpenglRenderCommand::bindDescriptorSets "
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
                                    GL_FUNC(glUniform1f(shaderUniformLocations[uboInfo.locationIndex], val));
                                    break;
                                }
                                case ShaderDataType::Float2:
                                {
                                    vec2 vec;
                                    valSize = sizeof(vec2);
                                    memcpy(&vec, pCurrentData, valSize);

                                    GL_FUNC(glUniform2f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y
                                    ));
                                    break;
                                }
                                case ShaderDataType::Float3:
                                {
                                    vec3 vec;
                                    valSize = sizeof(vec3);
                                    memcpy(&vec, pCurrentData, valSize);
                                    GL_FUNC(glUniform3f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y,
                                        vec.z
                                    ));
                                    break;
                                }
                                case ShaderDataType::Float4:
                                {
                                    vec4 vec;
                                    valSize = sizeof(vec4);
                                    memcpy(&vec, pCurrentData, valSize);

                                    GL_FUNC(glUniform4f(
                                        shaderUniformLocations[uboInfo.locationIndex],
                                        vec.x,
                                        vec.y,
                                        vec.z,
                                        vec.w
                                    ));
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
                                        GL_FUNC(glUniformMatrix4fv(
                                            shaderUniformLocations[uboInfo.locationIndex + i],
                                            1,
                                            GL_FALSE,
                                            (const float*)pData
                                        ));
                                    }
                                    break;
                                }

                                default:
                                    Debug::log(
                                        "@OpenglRenderCommand::bindDescriptorSets "
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
                                    "@OpenglRenderCommand::bindDescriptorSets "
                                    "Texture at binding index: " + std::to_string(textureBindingIndex) + " was nullptr",
                                    Debug::MessageType::PK_FATAL_ERROR
                                );
                            }
                            #endif
                            GL_FUNC(glUniform1i(shaderUniformLocations[layoutInfo.locationIndex], binding.getBinding()));
                            // well following is quite fucking dumb.. dunno how could do this better
                            GL_FUNC(glActiveTexture(binding_to_gl_texture_unit(binding.getBinding())));
                            GL_FUNC(glBindTexture(
                                GL_TEXTURE_2D,
                                ((opengl::OpenglTexture*)textures[textureBindingIndex])->getGLTexID()
                            ));
                            ++textureBindingIndex;
                        }
                    }
                }
            }
        }

        void OpenglRenderCommand::pushConstants(
            CommandBuffer* pCmdBuf,
            //pipelineLayout,
            ShaderStageFlagBits shaderStageFlags,
            uint32_t offset,
            uint32_t size,
            const void* pValues,
            std::vector<UniformInfo> glUniformInfo // Only used on opengl side
        )
        {
            opengl::OpenglPipeline* pipeline = ((OpenglCommandBuffer*)pCmdBuf)->_pPipeline;
            const opengl::OpenglShaderProgram* pShaderProgram = pipeline->getShaderProgram();
            const std::vector<int32_t>& shaderUniformLocations = pShaderProgram->getUniformLocations();

            PK_byte* pBuf = (PK_byte*)pValues;
            size_t bufOffset = 0;
            for (const UniformInfo& uInfo : glUniformInfo)
            {
                if (bufOffset >= size)
                {
                    Debug::log(
                        "@OpenglRenderCommand::pushConstants "
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
                        GL_FUNC(glUniform1i(shaderUniformLocations[uInfo.locationIndex], val));
                        break;
                    }
                    case ShaderDataType::Float:
                    {
                        float val = *(float*)pCurrentData;
                        GL_FUNC(glUniform1f(shaderUniformLocations[uInfo.locationIndex], val));
                        break;
                    }
                    case ShaderDataType::Float2:
                    {
                        vec2 vec = *(vec2*)pCurrentData;
                        GL_FUNC(glUniform2f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y
                        ));
                        break;
                    }
                    case ShaderDataType::Float3:
                    {
                        vec3 vec = *(vec3*)pCurrentData;;
                        GL_FUNC(glUniform3f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y,
                            vec.z
                        ));
                        break;
                    }
                    case ShaderDataType::Float4:
                    {
                        vec4 vec = *(vec4*)pCurrentData;
                        GL_FUNC(glUniform4f(
                            shaderUniformLocations[uInfo.locationIndex],
                            vec.x,
                            vec.y,
                            vec.z,
                            vec.w
                        ));
                        break;
                    }
                    case ShaderDataType::Mat4:
                    {
                        mat4 matrix = *(mat4*)pCurrentData;
                        GL_FUNC(glUniformMatrix4fv(
                            shaderUniformLocations[uInfo.locationIndex],
                            1,
                            GL_FALSE,
                            (const float*)&matrix
                        ));
                        break;
                    }

                    default:
                        Debug::log(
                            "@OpenglRenderCommand::pushConstants "
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

        void OpenglRenderCommand::draw(
            CommandBuffer* pCmdBuf,
            uint32_t vertexCount,
            uint32_t instanceCount,
            uint32_t firstVertex,
            uint32_t firstInstance
        )
        {
            #ifdef PK_DEBUG_FULL
            Debug::log(
                "@OpenglRenderCommand::draw NOT IMPLEMENTED!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            #endif
        }

        void OpenglRenderCommand::drawIndexed(
            CommandBuffer* pCmdBuf,
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        )
        {
            const IndexType& indexType = ((OpenglCommandBuffer*)pCmdBuf)->_drawIndexedType;
            // NOTE: Don't remember why not giving the ptr to the indices here..
            //glDrawElements(GL_TRIANGLES, indexCount, index_type_to_glenum(indexType), 0);
            GL_FUNC(glDrawElementsInstanced(
                GL_TRIANGLES,
                indexCount,
                index_type_to_glenum(indexType),
                0,
                instanceCount
            ));
        }
    }
}
