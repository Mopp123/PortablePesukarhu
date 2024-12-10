#include "CommandBuffer.h"
#include "pesukarhu/core/Debug.h"
#include "Context.h"
#include "platform/web/WebCommandBuffer.h"

namespace pk
{

    std::vector<CommandBuffer*> CommandBuffer::create(int count)
    {
        const uint32_t api = Context::get_api_type();
        std::vector<CommandBuffer*> outBuffers(count);
        memset(outBuffers.data(), 0, sizeof(CommandBuffer*) * count);
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
            {
                for (int i = 0; i < count; ++i)
                    outBuffers[i] = new web::WebCommandBuffer;
            } break;
            default:
            {
                Debug::log(
                    "Attempted to create command buffer but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
            } break;
        }

        return outBuffers;
    }
}
