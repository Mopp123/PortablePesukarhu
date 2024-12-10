#include "Context.h"
#include "platform/web/WebContext.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    PK_byte Context::s_graphicsAPI = GRAPHICS_API_NONE;

    Context::~Context()
    {
    }

    Context* Context::create(PK_byte graphicsAPIType)
    {
        s_graphicsAPI = graphicsAPIType;
        switch (s_graphicsAPI)
        {
            case GRAPHICS_API_WEBGL:
                return new web::WebContext;
            default:
                Debug::log(
                    "Failed to create graphics context. Selected invalid graphics api type(" + std::to_string(s_graphicsAPI) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }

    PK_byte Context::get_api_type()
    {
        return s_graphicsAPI;
    }
}
