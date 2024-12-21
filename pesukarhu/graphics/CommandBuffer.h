#pragma once

#include <vector>


namespace pk
{
    // Need to be able to let derived CommandBuffer classes
    // to call original's private constructor
    // NOTE: Not tested if this fucks up things again..
    namespace web
    {
        class WebCommandBuffer;
    }
    namespace opengl
    {
        class OpenglCommandBuffer;
    }

    class CommandBuffer
    {
    private:
        friend class web::WebCommandBuffer;
        friend class opengl::OpenglCommandBuffer;

    public:
        CommandBuffer(const CommandBuffer&) = delete;
        virtual ~CommandBuffer() {}

        virtual void allocate() {}
        virtual void free() {}

        static std::vector<CommandBuffer*> create(int count);

    private:
        CommandBuffer() {}
    };
}
