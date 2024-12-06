#include "Pipeline.h"
#include "Context.h"
#include "pesukarhu/core/Debug.h"
#include "platform/opengl/OpenglPipeline.h"

namespace pk
{
    Pipeline* Pipeline::create()
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
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

