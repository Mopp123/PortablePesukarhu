
#include "WebBuffers.h"
#include <GL/glew.h>

#include "../../../core/Debug.h"


namespace pk
{
    namespace web
    {
        // TODO: Delete this -> NOT TO BE CONFUSED WITH THE NEW "BufferUsage"
        static GLenum to_webgl_usage(VertexBufferUsage u)
        {
            switch (u)
            {
                case PK_BUFFER_USAGE_STATIC:	return GL_STATIC_DRAW;
                case PK_BUFFER_USAGE_DYNAMIC:	return GL_DYNAMIC_DRAW;
                default:
                                                Debug::log("(location: WebVertexBuffer::to_webgl_usage) Invalid vertex buffer usage value!", Debug::MessageType::PK_ERROR);
                                                return GL_STATIC_DRAW;
            }
        }


        WebVertexBuffer::WebVertexBuffer(const std::vector<float>& data, VertexBufferUsage usage) :
            VertexBuffer(data, usage)
        {
            glGenBuffers(1, &_id);
            glBindBuffer(GL_ARRAY_BUFFER, _id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], to_webgl_usage(usage));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        WebVertexBuffer::~WebVertexBuffer()
        {
            glDeleteBuffers(1, &_id);
        }

        void WebVertexBuffer::update(const std::vector<float>& newData, int offset, int size)
        {
            glBindBuffer(GL_ARRAY_BUFFER, _id);
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, &newData[0]);
        }


        WebIndexBuffer::WebIndexBuffer(const std::vector<unsigned short>& data) :
            IndexBuffer(data)
        {
            glGenBuffers(1, &_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * data.size(), &data[0], to_webgl_usage(VertexBufferUsage::PK_BUFFER_USAGE_STATIC));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        WebIndexBuffer::~WebIndexBuffer()
        {}

        // New way of creating buffers!
        // TODO: Adapt everywhere to this!
        WebBuffer::WebBuffer(
                void* data,
                size_t elementSize,
                size_t dataLength,
                uint32_t bufferUsageFlags,
                bool saveDataHostSide
        ) :
            Buffer(data, elementSize, dataLength, bufferUsageFlags)
        {
            uint32_t invalidMask = BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT & BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT;
            if (bufferUsageFlags & invalidMask)
            {
                Debug::log(
                        "Attempted to create WebBuffer with invalid bufferUsageFlags. WebBuffer can have only a single bufferUsageFlag bit set!",
                        Debug::MessageType::PK_FATAL_ERROR
                        );
                return;
            }

            if (bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT)
            {
                Debug::log("Creating WebBuffer<vertex>");
                glGenBuffers(1, &_id);
                glBindBuffer(GL_ARRAY_BUFFER, _id);
                glBufferData(GL_ARRAY_BUFFER, _dataElemSize * _dataLength, _data, GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            else if (bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT)
            {
                _isIndexBuffer = true;
                Debug::log("Creating WebBuffer<index>");
                glGenBuffers(1, &_id);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _dataElemSize * _dataLength, _data, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            if (!saveDataHostSide)
            {
                #ifdef PK_DEBUG_FULL
                if (bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                    Debug::log(
                        "Uniform buffer not marked for saving on host side! "
                        "Using context api as opengl doesn't allow this!",
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                Debug::log(
                    "WebBuffer not marked to be saved on host side. "
                    "Deleting host side buffer"
                );
                #endif
                free(_data);
                _data = nullptr;
            }
            Debug::log("    Success");
        }

        WebBuffer::~WebBuffer()
        {
            // For some reason I had earlier skipped all index buffer deletion..
            // I recall there may have been some problem with that some time ago.. don't remember
            // TODO: Investigate this!
            if (!_isIndexBuffer)
                glDeleteBuffers(1, &_id);
            Debug::log("WebBuffer deleted");
        }

        // NOTE: Not sure does updating index buffers work, not tested!!!
        void WebBuffer::update(const void* data, size_t dataSize)
        {
            #ifdef PK_DEBUG_FULL
            if (!_data)
            {
                Debug::log(
                    "@WebBuffer::update(1) Buffer data was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            #endif
            if (_dataLength * _dataElemSize >= dataSize)
            {
                memcpy(_data, data, dataSize);
                // Don't need to call any glBufferData or subData for uniform buffers on gl side
                // UNTIL uniform buffer support..
                if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                    _shouldUpdate = true;
            }
        }

        void WebBuffer::update(const void* data, size_t offset, size_t dataSize)
        {
            #ifdef PK_DEBUG_FULL
            if (!_data)
            {
                Debug::log(
                    "@WebBuffer::update(2) Buffer data was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
            }
            #endif
            if (_dataLength * _dataElemSize >= offset + dataSize)
            {
                memcpy(((PK_byte*)_data) + offset, data, dataSize);
                // Don't need to call any glBufferData or subData for uniform buffers on gl side
                // UNTIL uniform buffer support..
                if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                    _shouldUpdate = true;
            }
        }

        void WebBuffer::clearHostSideBuffer()
        {
            memset(_data, 0, _dataLength * _dataElemSize);
            // Don't need to call any glBufferData or subData for uniform buffers on gl side
            // UNTIL uniform buffer support..
            if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                _shouldUpdate = true;
        }
    }
}
