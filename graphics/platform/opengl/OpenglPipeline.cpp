#include "graphics/platform/opengl/OpenglPipeline.h"
#include "core/Debug.h"


namespace pk
{
    namespace opengl
    {
        OpenglPipeline::OpenglPipeline(
            const std::vector<VertexBufferLayout>& vertexBufferLayouts,
            const std::vector<DescriptorSetLayout>& descriptorLayouts,
            ShaderVersion shaderVersion,
            const Shader* pVertexShader, const Shader* pFragmentShader,
            float viewportWidth, float viewportHeight,
            const Rect2D viewportScissor,
            CullMode cullMode,
            FrontFace frontFace,
            bool enableDepthTest,
            DepthCompareOperation depthCmpOp
        ) :
            // Kind of dumb...
            // inherited class doesnt really do anything with these,
            // but we propably want to enforce filling these for all implementations.. maybe..
            Pipeline(
                vertexBufferLayouts,
                descriptorLayouts,
                pVertexShader, pFragmentShader,
                viewportWidth, viewportHeight,
                viewportScissor,
                cullMode,
                frontFace,
                enableDepthTest,
                depthCmpOp
            ),

            _vertexBufferLayouts(vertexBufferLayouts),
            _descriptorLayouts(descriptorLayouts),
            _viewportWidth(viewportWidth),
            _viewportHeight(viewportHeight),

            _shaderProgram(
                shaderVersion,
                (const OpenglShader*)pVertexShader,
                (const OpenglShader*)pFragmentShader
            ),

            _cullMode(cullMode),
            _frontFace(frontFace),

            _enableDepthTest(enableDepthTest),
            _depthCmpOp(depthCmpOp)
        {
        }
    }
}
