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

    class CommandBuffer
    {
    private:
        friend class web::WebCommandBuffer;

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
