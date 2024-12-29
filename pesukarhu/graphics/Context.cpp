#include "Context.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Debug.h"

#ifdef PK_BUILD_WEB
    #include "platform/web/WebContext.h"
#elif defined(PK_BUILD_DESKTOP)
    #include "pesukarhu/core/platform/desktop/DesktopWindow.h"
    #include "platform/opengl/OpenglContext.h"
#endif


namespace pk
{
    GraphicsAPI Context::s_graphicsAPI = GraphicsAPI::PK_GRAPHICS_API_NONE;

    Context::~Context()
    {
    }

    Context* Context::create(PlatformName platform, GraphicsAPI graphicsAPI, Window* pWindow)
    {
        s_graphicsAPI = graphicsAPI;
        #ifdef PK_BUILD_WEB
            return new web::WebContext;
        #elif defined(PK_BUILD_DESKTOP)
            return new opengl::OpenglContext((desktop::DesktopWindow*)pWindow);
        #else
            Debug::log(
                "@Context::create "
                "Failed to create Graphics Context. Invalid build target! "
                "Available targets: web, desktop(linux)",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        #endif
    }

    GraphicsAPI Context::get_graphics_api()
    {
        return s_graphicsAPI;
    }
}
