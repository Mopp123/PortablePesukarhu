#include "Pipeline.h"
#include "Context.h"
#include "pesukarhu/core/Debug.h"
#include "platform/opengl/OpenglPipeline.h"

namespace pk
{
    Pipeline* Pipeline::create()
    {
        const GraphicsAPI api = Context::get_graphics_api();
        switch(api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return new opengl::OpenglPipeline;
            case GraphicsAPI::PK_GRAPHICS_API_OPENGL:
                return new opengl::OpenglPipeline;
            default:
                Debug::log(
                    "Attempted to create graphics pipeline but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}

