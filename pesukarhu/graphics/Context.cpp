#include "Context.h"
#include "platform/web/WebContext.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    GraphicsAPI Context::s_graphicsAPI = GraphicsAPI::PK_GRAPHICS_API_NONE;

    Context::~Context()
    {
    }

    Context* Context::create(GraphicsAPI graphicsAPI)
    {
        s_graphicsAPI = graphicsAPI;
        switch (s_graphicsAPI)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return new web::WebContext;
            default:
                Debug::log(
                    "Failed to create graphics context. Selected invalid graphics api type(" + std::to_string(s_graphicsAPI) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }

    GraphicsAPI Context::get_graphics_api()
    {
        return s_graphicsAPI;
    }
}
