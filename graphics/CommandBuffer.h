#pragma once


namespace pk
{
    class CommandBuffer
    {
    public:
        CommandBuffer(const CommandBuffer&) = delete;
        virtual ~CommandBuffer() {}

        static CommandBuffer* create();

    protected:
        CommandBuffer() {}
    };
}
