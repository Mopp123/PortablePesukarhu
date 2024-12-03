#include "RenderCommand.h"
#include "Context.h"
#include "Pesukarhu/core/Debug.h"
#include "platform/web/WebRenderCommand.h"

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
        switch(Context::get_api_type())
        {
            case GRAPHICS_API_WEBGL:
                return new web::WebRenderCommand;
            default:
                Debug::log(
                    "Invalid graphics API(" + std::to_string(Context::get_api_type()) + ") assigned to create RenderCommand instance",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
