#include "OpenglBuffers.h"
#include <GL/glew.h>
#include "pesukarhu/core/Debug.h"


namespace pk
{
    namespace opengl
    {
        // TODO: Delete this -> NOT TO BE CONFUSED WITH THE NEW "BufferUsage"
        static GLenum to_opengl_usage(VertexBufferUsage u)
        {
            switch (u)
            {
                case PK_BUFFER_USAGE_STATIC: return GL_STATIC_DRAW;
                case PK_BUFFER_USAGE_DYNAMIC: return GL_DYNAMIC_DRAW;
                default:
                    Debug::log(
                        "@to_opengl_usage "
                        "Invalid vertex buffer usage value!",
                        Debug::MessageType::PK_ERROR
                    );
                    return GL_STATIC_DRAW;
            }
        }


        static GLenum to_opengl_buffer_update_frequency(BufferUpdateFrequency f)
        {
            switch (f)
            {
                case BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STATIC:	return GL_STATIC_DRAW;
                case BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_DYNAMIC: return GL_DYNAMIC_DRAW;
                case BufferUpdateFrequency::BUFFER_UPDATE_FREQUENCY_STREAM: return GL_STREAM_DRAW;
                default:
                    Debug::log(
                        "@to_opengl_buffer_update_frequency "
                        "Invalid buffer update frequency: " + std::to_string(f),
                        Debug::MessageType::PK_FATAL_ERROR
                    );
                    return GL_STATIC_DRAW;
            }
        }


        OpenglBuffer::OpenglBuffer(
                void* data,
                size_t elementSize,
                size_t dataLength,
                uint32_t bufferUsageFlags,
                BufferUpdateFrequency bufferUpdateFrequency,
                bool saveDataHostSide
        ) :
            Buffer(data, elementSize, dataLength, bufferUsageFlags, bufferUpdateFrequency)
        {
            uint32_t invalidMask = BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT & BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT;
            if (bufferUsageFlags & invalidMask)
            {
                Debug::log(
                        "Attempted to create OpenglBuffer with invalid bufferUsageFlags. OpenglBuffer can have only a single bufferUsageFlag bit set!",
                        Debug::MessageType::PK_FATAL_ERROR
                        );
                return;
            }

            // in gl terms its the glBufferData's "usage"
            GLenum glBufferUpdateFrequency = to_opengl_buffer_update_frequency(bufferUpdateFrequency);

            if (bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER_BIT)
            {
                Debug::log("Creating OpenglBuffer<vertex>");
                glGenBuffers(1, &_id);
                glBindBuffer(GL_ARRAY_BUFFER, _id);
                glBufferData(GL_ARRAY_BUFFER, _dataElemSize * _dataLength, _data, glBufferUpdateFrequency);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            else if (bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER_BIT)
            {
                _isIndexBuffer = true;
                Debug::log("Creating OpenglBuffer<index>");
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
                    "OpenglBuffer not marked to be saved on host side. "
                    "Deleting host side buffer"
                );
                #endif
                free(_data);
                _data = nullptr;
            }
            Debug::log("    Success");
        }

        OpenglBuffer::~OpenglBuffer()
        {
            // For some reason I had earlier skipped all index buffer deletion..
            // I recall there may have been some problem with that some time ago.. don't remember
            // TODO: Investigate this!
            if (!_isIndexBuffer)
                glDeleteBuffers(1, &_id);
            Debug::log("OpenglBuffer deleted");
        }

        // NOTE: Not sure does updating index buffers work, not tested!
        void OpenglBuffer::update(const void* data, size_t dataSize)
        {
            #ifdef PK_DEBUG_FULL
            if (!_data)
            {
                Debug::log(
                    "@OpenglBuffer::update(1) Buffer data was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            #endif
            if (_dataLength * _dataElemSize >= dataSize)
            {
                memcpy(_data, data, dataSize);
                // Used when calling the actual glBufferData or glBufferSubData
                _updateOffset = 0;
                _updateSize = dataSize;
                // Don't need to call any glBufferData or subData for uniform buffers on gl side
                // UNTIL uniform buffer support..
                if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                    _shouldUpdate = true;
            }
        }

        void OpenglBuffer::update(const void* data, size_t offset, size_t dataSize)
        {
            #ifdef PK_DEBUG_FULL
            if (!_data)
            {
                Debug::log(
                    "@OpenglBuffer::update(2) Buffer data was nullptr",
                    Debug::MessageType::PK_FATAL_ERROR
                );
            }
            #endif
            if (_dataLength * _dataElemSize >= offset + dataSize)
            {
                memcpy(((PK_byte*)_data) + offset, data, dataSize);
                // Used when calling the actual glBufferData or glBufferSubData
                _updateOffset = offset;
                _updateSize = dataSize;
                // Don't need to call any glBufferData or subData for uniform buffers on gl side
                // UNTIL uniform buffer support..
                if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                    _shouldUpdate = true;
            }
        }

        void OpenglBuffer::clearHostSideBuffer()
        {
            memset(_data, 0, _dataLength * _dataElemSize);
            // Don't need to call any glBufferData or subData for uniform buffers on gl side
            // UNTIL uniform buffer support..
            if (!(_bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                _shouldUpdate = true;
        }
    }
}
