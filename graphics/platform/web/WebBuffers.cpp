
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
        WebBuffer::WebBuffer(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags) :
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
    }
}
