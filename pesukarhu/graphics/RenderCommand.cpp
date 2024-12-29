#include "RenderCommand.h"
#include "Context.h"
#include "pesukarhu/core/Debug.h"

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
        #ifdef PK_BUILD_WEB
                return new opengl::OpenglRenderCommand;
        #elif defined(PK_BUILD_DESKTOP)
                return new opengl::OpenglRenderCommand;
        #else
            Debug::log(
                "@RenderCommand::create "
                "Failed to create RenderCommand. Invalid build target! "
                "Available targets: web, desktop(linux)",
                Debug::MessageType::PK_FATAL_ERROR
            );
        #endif
    }
}
