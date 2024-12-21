#include "RenderCommand.h"
#include "Context.h"
#include "pesukarhu/core/Debug.h"
#include "platform/web/WebRenderCommand.h"
#include "platform/opengl/OpenglRenderCommand.h"


namespace pk
{

    RenderCommand* RenderCommand::s_pInstance = nullptr;

    RenderCommand* RenderCommand::get()
    {
        if (s_pInstance)
        {
            return s_pInstance;
        }
        s_pInstance = create();
        return s_pInstance;
    }

    RenderCommand* RenderCommand::create()
    {
        const GraphicsAPI api = Context::get_graphics_api();
        switch(api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return new web::WebRenderCommand;
            case GraphicsAPI::PK_GRAPHICS_API_OPENGL:
                return new opengl::OpenglRenderCommand;
            default:
                Debug::log(
                    "Invalid graphics API(" + std::to_string(api) + ") assigned to create RenderCommand instance",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
