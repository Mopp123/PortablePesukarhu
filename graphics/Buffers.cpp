#include "Buffers.h"
#include "../core/Debug.h"
#include "Context.h"
#include "platform/web/WebBuffers.h"
#include <cstdlib>


namespace pk
{
    int32_t get_shader_data_type_component_count(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::None:
                Debug::log(
                    "@get_shader_data_type_component_count Invalid shaderDataType <NONE>",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return 0;

            case ShaderDataType::Int:
                return 1;
            case ShaderDataType::Int2:
                return 2;
            case ShaderDataType::Int3:
                return 3;
            case ShaderDataType::Int4:
                return 4;

            case ShaderDataType::Float:
                return 1;
            case ShaderDataType::Float2:
                return 2;
            case ShaderDataType::Float3:
                return 3;
            case ShaderDataType::Float4:
                return 4;

            default:
                Debug::log(
                    "@get_shader_data_type_component_count Invalid shaderDataType",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return 0;
        }
    }

    Buffer::~Buffer()
    {
        if (_data)
            free(_data);
    }

    Buffer::Buffer(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags) :
        _dataElemSize(elementSize),
        _dataLength(dataLength)
    {
        Debug::log("Allocating new buffer<raw> with total size: " + std::to_string(_dataElemSize * _dataLength));
        _data = calloc(_dataLength, _dataElemSize);
        if (data)
        {
            Debug::log("    Assigning buffer<raw> data");
            memcpy(_data, data, _dataElemSize * _dataLength);
        }
        Debug::log("    Success");
    }

    Buffer* Buffer::create(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags)
    {
        const uint32_t api = Context::get_api_type();
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new web::WebBuffer(data, elementSize, dataLength, bufferUsageFlags);
            default:
                Debug::log(
                    "Attempted to create Buffer but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
