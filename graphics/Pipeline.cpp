#include "Pipeline.h"
#include "Context.h"
#include "core/Debug.h"
#include "platform/opengl/OpenglPipeline.h"

namespace pk
{
    Pipeline* Pipeline::create(
        const std::vector<VertexBufferLayout>& vertexBufferLayouts,
        const std::vector<DescriptorSetLayout>& descriptorLayouts,
        const Shader* pVertexShader, const Shader* pFragmentShader,
        float viewportWidth, float viewportHeight,
        const Rect2D viewportScissor
    )
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new opengl::OpenglPipeline(
                    vertexBufferLayouts,
                    descriptorLayouts,
                    pVertexShader, pFragmentShader,
                    viewportWidth, viewportHeight,
                    viewportScissor
                );
            default:
                Debug::log(
                    "Attempted to create graphics pipeline but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}

