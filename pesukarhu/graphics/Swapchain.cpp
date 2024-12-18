#include "Swapchain.h"
#include "Context.h"
#include "platform/opengl/OpenglSwapchain.h"
#include "pesukarhu/core/Debug.h"
#include <string>


namespace pk
{

    Swapchain* Swapchain::create()
    {
        const GraphicsAPI api = Context::get_graphics_api();
        switch(api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
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
