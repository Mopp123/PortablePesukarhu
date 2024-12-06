#include "Swapchain.h"
#include "Context.h"
#include "platform/opengl/OpenglSwapchain.h"
#include "pesukarhu/core/Debug.h"
#include <string>


namespace pk
{

    Swapchain* Swapchain::create()
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new opengl::OpenglSwapchain;
            default:
                Debug::log(
                    "@Swapchain::create invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
