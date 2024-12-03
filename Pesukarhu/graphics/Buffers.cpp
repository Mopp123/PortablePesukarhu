#include "Buffers.h"
#include "Pesukarhu/core/Debug.h"
#include "Context.h"
#include "platform/web/WebBuffers.h"
#include <cstdlib>
#include <set>


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


    size_t get_shader_data_type_size(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::None:
                Debug::log(
                    "@get_shader_data_type_size Invalid shaderDataType <NONE>",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return 0;

            case ShaderDataType::Int:
                return sizeof(int);
            case ShaderDataType::Int2:
                return sizeof(int) * 2;
            case ShaderDataType::Int3:
                return sizeof(int) * 3;
            case ShaderDataType::Int4:
                return sizeof(int) * 4;

            case ShaderDataType::Float:
                return sizeof(float);
            case ShaderDataType::Float2:
                return sizeof(float) * 2;
            case ShaderDataType::Float3:
                return sizeof(float) * 3;
            case ShaderDataType::Float4:
                return sizeof(float) * 4;

            case ShaderDataType::Mat4:
                return sizeof(float) * 4 * 4;

            default:
                Debug::log(
                    "@get_shader_data_type_size Invalid shaderDataType",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return 0;
        }
    }


    std::string shader_data_type_to_string(ShaderDataType type)
    {
        switch (type)
        {
            case None : return "None";
            case Int : return "Int";
            case Int2 : return "Int2";
            case Int3 : return "Int3";
            case Int4 : return "Int4";
            case Float : return "Float";
            case Float2 : return "Float2";
            case Float3 : return "Float3";
            case Float4 : return "Float4";
            case Mat4 : return "Mat4";
            default : return "Invalid type";
        }
    }


    VertexBufferLayout::VertexBufferLayout(std::vector<VertexBufferElement> elems, VertexInputRate inputRate) :
        _elements(elems),
        _inputRate(inputRate)
    {
        for (const VertexBufferElement& elem : _elements)
            _stride += get_shader_data_type_size(elem.getType());
    }

    VertexBufferLayout::VertexBufferLayout(const VertexBufferLayout& other) :
        _elements(other._elements),
        _inputRate(other._inputRate),
        _stride(other._stride)
    {}

    bool VertexBufferLayout::isValid() const
    {
        std::set<uint32_t> attribLocations;
        for (const VertexBufferElement& elem : _elements)
        {
            const uint32_t location = elem.getLocation();
            if (attribLocations.find(location) == attribLocations.end())
                attribLocations.insert(location);
            else
                return false;
        }
        return true;
    }

    bool VertexBufferLayout::are_valid(
        const std::vector<VertexBufferLayout>& layoutGroup,
        uint32_t* pOutConflictLocation
    )
    {
        std::set<uint32_t> attribLocations;
        for (const VertexBufferLayout& layout : layoutGroup)
        {
            for (const VertexBufferElement& elem : layout.getElements())
            {
                const uint32_t location = elem.getLocation();
                if (attribLocations.find(location) == attribLocations.end())
                {
                    attribLocations.insert(location);
                }
                else
                {
                    if (pOutConflictLocation)
                        *pOutConflictLocation = location;
                    return false;
                }
            }
        }
        return true;
    }


    Buffer::Buffer(
        void* data,
        size_t elementSize,
        size_t dataLength,
        uint32_t bufferUsageFlags,
        BufferUpdateFrequency bufferUpdateFrequency
    ) :
        _dataElemSize(elementSize),
        _dataLength(dataLength),
        _bufferUsageFlags(bufferUsageFlags),
        _updateFrequency(bufferUpdateFrequency)
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

    Buffer::~Buffer()
    {
        if (_data)
            free(_data);
    }

    Buffer* Buffer::create(
        void* data,
        size_t elementSize,
        size_t dataLength,
        uint32_t bufferUsageFlags,
        BufferUpdateFrequency bufferUpdateFrequency,
        bool saveDataHostSide
    )
    {
        const uint32_t api = Context::get_api_type();
        // Atm ubos have to be available on host
        // (no stagning buffers for uniform buffers supported)
        bool saveHostSide = bufferUsageFlags & BufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER_BIT ? true : saveDataHostSide;
        switch(api)
        {
            case GRAPHICS_API_WEBGL:
                return new web::WebBuffer(
                    data,
                    elementSize,
                    dataLength,
                    bufferUsageFlags,
                    bufferUpdateFrequency,
                    saveHostSide
                );
            default:
                Debug::log(
                    "Attempted to create Buffer but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }
}
