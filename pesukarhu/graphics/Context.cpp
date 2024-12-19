#include "Context.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/platform/desktop/DesktopWindow.h"
#include "platform/web/WebContext.h"
#include "platform/opengl/OpenglContext.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    GraphicsAPI Context::s_graphicsAPI = GraphicsAPI::PK_GRAPHICS_API_NONE;

    Context::~Context()
    {
    }

    Context* Context::create(PlatformName platform, GraphicsAPI graphicsAPI, Window* pWindow)
    {
        s_graphicsAPI = graphicsAPI;
        switch (s_graphicsAPI)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return new web::WebContext;
            case GraphicsAPI::PK_GRAPHICS_API_OPENGL:
            {
                if (platform != PlatformName::PK_PLATFORM_LINUX && platform != PlatformName::PK_PLATFORM_WINDOWS)
                {
                    Debug::log(
                        "@Context::create "
                        "Unsupported Platform - GraphicsAPI combination! "
                        "Opengl not supported for selected platform: " + std::to_string(platform),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return nullptr;
                }
                // TODO: Some better validation that the window can actually be cast into
                // DesktopWindow -> maybe some type into Window?
                return new opengl::OpenglContext((desktop::DesktopWindow*)pWindow);
            }
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
