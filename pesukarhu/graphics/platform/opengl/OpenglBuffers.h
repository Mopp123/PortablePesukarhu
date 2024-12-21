#pragma once

#include "pesukarhu/graphics/Buffers.h"


namespace pk
{
    namespace opengl
    {
        class OpenglRenderCommand;


        class OpenglBuffer : public Buffer
        {
        private:
            friend class Buffer;
            friend class OpenglRenderCommand;

            uint32_t _id = 0;
            bool _isIndexBuffer = false;
            // Used to call glBufferSubData on updated internal _data when
            // the buffer gets bound. This is to use opengl binding and unbinding
            // calls less frequently
            bool _shouldUpdate = false;
            size_t _updateSize = 0;
            size_t _updateOffset = 0; // Doesn't work atm!

        public:
            OpenglBuffer(const OpenglBuffer&) = delete;
            ~OpenglBuffer();
            virtual void update(const void* data, size_t dataSize);
            virtual void update(const void* data, size_t offset, size_t dataSize);
            virtual void clearHostSideBuffer();

            inline uint32_t getID() const { return _id; }
            inline bool shouldUpdate() const { return _shouldUpdate; }

        protected:
            OpenglBuffer(
                void* data,
                size_t elementSize,
                size_t dataLength,
                uint32_t bufferUsageFlags,
                BufferUpdateFrequency bufferUpdateFrequency,
                bool saveDataHostSide
            );
        };
    }
}
