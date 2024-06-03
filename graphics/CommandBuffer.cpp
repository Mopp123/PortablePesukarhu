#include "CommandBuffer.h"
#include "../core/Debug.h"
#include "Context.h"
#include "platform/web/WebCommandBuffer.h"

namespace pk
{

    CommandBuffer* create()
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new web::WebCommandBuffer;
            default:
                Debug::log(
                    "Attempted to create command buffer but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
