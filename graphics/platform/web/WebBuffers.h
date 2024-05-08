#pragma once

#include "../../Buffers.h"


namespace pk
{
    namespace web
    {
        class WebVertexBuffer : public VertexBuffer
        {
        private:
            uint32_t _id = 0;

        public:
            WebVertexBuffer(const std::vector<float>& data, VertexBufferUsage usage);
            ~WebVertexBuffer();

            virtual void update(const std::vector<float>& newData, int offset, int size) override;

            inline uint32_t getID() const { return _id; }
        };


        class WebIndexBuffer : public IndexBuffer
        {
        private:
            uint32_t _id = 0;

        public:
            WebIndexBuffer(const std::vector<unsigned short>& data);
            ~WebIndexBuffer();

            inline uint32_t getID() const { return _id; }
        };


        class WebBuffer : public Buffer
        {
        private:
            friend class Buffer;

            uint32_t _id = 0;
            bool _isIndexBuffer = false;

        public:
            ~WebBuffer();
            inline uint32_t getID() const { return _id; }

        private:
            WebBuffer(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags);
        };
    }
}
