#pragma once

#include "Buffers.h"
#include "Descriptors.h"
#include "shaders/Shader.h"


namespace pk
{
    // TODO: put this kind of shit in some relevant place..
    // *I put this here just to get rid of VkRect2D
    struct Rect2D
    {
        int32_t offsetX;
        int32_t offsetY;
        uint32_t width;
        uint32_t height;
    };


    enum PipelineBindPoint
    {
        PIPELINE_BIND_POINT_NONE = 0,
        PIPELINE_BIND_POINT_COMPUTE = 1,
        PIPELINE_BIND_POINT_GRAPHICS = 2
    };


    enum CullMode
    {
        CULL_MODE_NONE = 0,
        CULL_MODE_FRONT = 1,
        CULL_MODE_BACK = 2
    };


    enum FrontFace
    {
        FRONT_FACE_COUNTER_CLOCKWISE = 0,
        FRONT_FACE_CLOCKWISE = 1
    };


    enum DepthCompareOperation
    {
        COMPARE_OP_NEVER = 0,
        COMPARE_OP_LESS = 1,
        COMPARE_OP_EQUAL = 2,
        COMPARE_OP_LESS_OR_EQUAL = 3,
        COMPARE_OP_GREATER = 4,
        COMPARE_OP_NOT_EQUAL = 5,
        COMPARE_OP_GREATER_OR_EQUAL = 6,
        COMPARE_OP_ALWAYS = 7,
    };


    class Pipeline
    {
    public:
        Pipeline(const Pipeline&) = delete;
        virtual ~Pipeline() {}

        static Pipeline* create(
            const std::vector<VertexBufferLayout>& vertexBufferLayouts,
            const std::vector<DescriptorSetLayout>& descriptorLayouts,
            const Shader* pVertexShader, const Shader* pFragmentShader,
            float viewportWidth, float viewportHeight,
            const Rect2D viewportScissor,
            CullMode cullMode,
            FrontFace frontFace,
            bool enableDepthTest,
            DepthCompareOperation depthCmpOp
        );

    protected:
        // TODO:
        // * RenderPass
        // * deal with push constants..
        Pipeline(
            //const RenderPass& renderPass,
            const std::vector<VertexBufferLayout>& vertexBufferLayouts,
            const std::vector<DescriptorSetLayout>& descriptorLayouts,
            const Shader* pVertexShader, const Shader* pFragmentShader,
            float viewportWidth, float viewportHeight,
            const Rect2D viewportScissor,
            //uint32_t pushConstantSize = 0, VkShaderStageFlags pushConstantStageFlags = VK_SHADER_STAGE_VERTEX_BIT
            CullMode cullMode,
            FrontFace frontFace,
            bool enableDepthTest,
            DepthCompareOperation depthCmpOp
        )
        {}

        // Originally used on vulkan project to recreate pipeline on window resize/other swapchain inadequate event..
        // NOTE: NOT TO BE CONFUSED IN THIS PROJECT'S GENERAL SYSTEM OF "create" FUNCS
        // TO CREATE API SPECIFIC SYSTEMS!
        /*
        void cleanUp();
        void create(
                const RenderPass& renderPass,
                const std::vector<VertexBufferLayout>& vertexBufferLayouts,
                const std::vector<DescriptorSetLayout>& descriptorLayouts,
                const Shader& vertexShader, const Shader& fragmentShader,
                float viewportWidth, float viewportHeight, const VkRect2D& viewportScissor,
                uint32_t pushConstantSize = 0,
                VkShaderStageFlags pushConstantStageFlags = VK_SHADER_STAGE_VERTEX_BIT
                );
        */
    };
}
