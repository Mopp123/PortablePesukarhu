#include "CommandBuffer.h"
#include "pesukarhu/core/Debug.h"
#include "Context.h"
#include "platform/opengl/OpenglCommandBuffer.h"


namespace pk
{

    std::vector<CommandBuffer*> CommandBuffer::create(int count)
    {
        std::vector<CommandBuffer*> outBuffers(count);
        memset(outBuffers.data(), 0, sizeof(CommandBuffer*) * count);

        #ifdef PK_BUILD_WEB
            for (int i = 0; i < count; ++i)
                outBuffers[i] = new opengl::OpenglCommandBuffer;
        #elif defined(PK_BUILD_DESKTOP)
            for (int i = 0; i < count; ++i)
                outBuffers[i] = new opengl::OpenglCommandBuffer;
        #else
            Debug::log(
                "@CommandBuffer::create "
                "Failed to create Command Buffers. Invalid build target! "
                "Available targets: web, desktop(linux)",
                Debug::MessageType::PK_FATAL_ERROR
            );
        #endif
        return outBuffers;
    }
}
