#include "graphics/platform/opengl/OpenglPipeline.h"


namespace pk
{
    namespace opengl
    {
        OpenglPipeline::OpenglPipeline(
            const std::vector<VertexBufferLayout>& vertexBufferLayouts,
            const std::vector<DescriptorSetLayout>& descriptorLayouts,
            const Shader* pVertexShader, const Shader* pFragmentShader,
            float viewportWidth, float viewportHeight,
            const Rect2D viewportScissor
        ) :
            // Kind of dumb...
            // inherited class doesnt really do anything with these,
            // but we propably want to enforce filling these for all implementations.. maybe..
            Pipeline(
                vertexBufferLayouts,
                descriptorLayouts,
                pVertexShader, pFragmentShader,
                viewportWidth, viewportHeight,
                viewportScissor
            ),

            _vertexBufferLayouts(vertexBufferLayouts),
            _descriptorLayouts(descriptorLayouts),
            _viewportWidth(viewportWidth),
            _viewportHeight(viewportHeight),

            _shaderProgram(
                (const OpenglShader*)pVertexShader,
                (const OpenglShader*)pFragmentShader
            )
        {
        }
    }
}
