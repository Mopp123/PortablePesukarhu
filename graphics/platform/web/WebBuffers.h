#pragma once

#include "../../Buffers.h"


namespace pk
{
    namespace web
    {
        class WebRenderCommand;

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
            friend class WebRenderCommand;

            uint32_t _id = 0;
            bool _isIndexBuffer = false;
            // Used to call glBufferSubData on updated internal _data when
            // the buffer gets bound. This is to use opengl binding and unbinding
            // calls less frequently
            bool _shouldUpdate = false;

        public:
            WebBuffer(const WebBuffer&) = delete;
            ~WebBuffer();
            virtual void update(const void* data, size_t dataSize);
            virtual void update(const void* data, size_t offset, size_t dataSize);
            virtual void clearHostSideBuffer();

            inline uint32_t getID() const { return _id; }
            inline bool shouldUpdate() const { return _shouldUpdate; }

        protected:
            WebBuffer(
                void* data,
                size_t elementSize,
                size_t dataLength,
                uint32_t bufferUsageFlags,
                bool saveDataHostSide
            );
        };
    }
}
