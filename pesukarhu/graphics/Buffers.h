#pragma once

#include <vector>
#include <string>
// Not sure is it just YCM issues with emscripten why syntax error if not including this
#include <cstdint>
#include <cstring>

#include "pesukarhu/Common.h"


namespace pk
{
    enum ShaderDataTypePrimitive
    {

    };

    // TODO: more specific names or enum class to "unfuck" possible issues
    enum ShaderDataType
    {
        None = 0,

        Int,
        Int2,
        Int3,
        Int4,

        Float,
        Float2,
        Float3,
        Float4,

        // NOTE: Only used by opengl
        Mat4
    };

    int32_t get_shader_data_type_component_count(ShaderDataType type);
    size_t get_shader_data_type_size(ShaderDataType type);
    std::string shader_data_type_to_string(ShaderDataType type);

    enum VertexInputRate
    {
        VERTEX_INPUT_RATE_VERTEX = 0,
        VERTEX_INPUT_RATE_INSTANCE = 1
    };

    // TODO: Get rid of this or rename!
    // Explanation: Switched to use Vulkan engine style BufferUsageFlagBits
    enum VertexBufferUsage
    {
        PK_BUFFER_USAGE_STATIC,
        PK_BUFFER_USAGE_DYNAMIC
    };


    enum BufferUsageFlagBits
    {
        BUFFER_USAGE_NONE = 0x0,
        BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x1,
        BUFFER_USAGE_INDEX_BUFFER_BIT = 0x2,
        BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x4
    };


    enum BufferUpdateFrequency
    {
        BUFFER_UPDATE_FREQUENCY_STATIC = 0,
        BUFFER_UPDATE_FREQUENCY_DYNAMIC,
        BUFFER_UPDATE_FREQUENCY_STREAM
    };

    enum IndexType
    {
        INDEX_TYPE_NONE = 0,
        INDEX_TYPE_UINT16 = 1,
        INDEX_TYPE_UINT32 = 2
    };


    class VertexBuffer
    {
    protected:
        VertexBufferUsage _usage = VertexBufferUsage::PK_BUFFER_USAGE_STATIC;

    public:
        VertexBuffer(const std::vector<float>& data, VertexBufferUsage usage) : _usage(usage) {}
        virtual ~VertexBuffer() {}
        virtual void update(const std::vector<float>& newData, int offset, int size) {}
    };


    class IndexBuffer
    {
    public:

        IndexBuffer(const std::vector<unsigned short>& data) {}
        virtual ~IndexBuffer() {}
    };


    class VertexBufferElement
    {
    protected:
        uint32_t _location = 0;
        ShaderDataType _type = ShaderDataType::Float;

    public:
        VertexBufferElement() = default;
        VertexBufferElement(const VertexBufferElement&) = default;

        VertexBufferElement(uint32_t location, ShaderDataType dataType) :
            _location(location),
            _type(dataType)
        {}

        inline uint32_t getLocation() const { return _location; }
        inline ShaderDataType getType() const { return _type; }
    };


    class VertexBufferLayout
    {
    protected:
        std::vector<VertexBufferElement> _elements;
        VertexInputRate _inputRate = VertexInputRate::VERTEX_INPUT_RATE_VERTEX;
        int32_t _stride = 0;

    public:
        // NOTE: Not sure if copying elems goes correctly here..
        VertexBufferLayout(std::vector<VertexBufferElement> elems, VertexInputRate inputRate);
        VertexBufferLayout(const VertexBufferLayout& other);

        bool isValid() const;
        // Can be used to make sure pipeline's layouts aren't in conflict with each others
        static bool are_valid(
            const std::vector<VertexBufferLayout>& layoutGroup,
            uint32_t* pOutConflictLocation = nullptr
        );

        inline const std::vector<VertexBufferElement>& getElements() const { return _elements; }
        inline VertexInputRate getInputRate() const { return _inputRate; }
        inline int32_t getStride() const { return _stride; }
    };


    class Buffer
    {
    protected:
        void* _data = nullptr;
        size_t _dataElemSize = 0; // size of a single entry in data
        size_t _dataLength = 0; // number of elements in the data
        uint32_t _bufferUsageFlags = 0;
        BufferUpdateFrequency _updateFrequency;

    public:
        Buffer(const Buffer&) = delete;
        virtual ~Buffer();

        // Replaces _data from beginning to dataSize.
        virtual void update(const void* data, size_t dataSize) = 0;
        virtual void update(const void* data, size_t offset, size_t dataSize) = 0;
        // Wouldn't necessarely need to be pure virtual..
        // webgl side just works the way atm this is required..
        virtual void clearHostSideBuffer() = 0;

        inline const void* getData() const { return _data; }
        inline size_t getDataElemSize() const { return _dataElemSize; }
        inline size_t getDataLength() const { return _dataLength; }
        inline uint32_t getBufferUsage() const { return _bufferUsageFlags; }
        inline size_t getTotalSize() const { return _dataElemSize * _dataLength; }
        inline BufferUpdateFrequency getUpdateFrequency() const { return _updateFrequency; }

        static Buffer* create(
            void* data,
            size_t elementSize,
            size_t dataLength,
            uint32_t bufferUsageFlags,
            BufferUpdateFrequency bufferUpdateFrequency,
            bool saveDataHostSide = false
        );

    protected:
        // *NOTE! "elementSize" single element's size in "data buffer"
        // *NOTE! "dataLength" number of "elements" in the "data buffer" (NOT total size)
        // *NOTE! "Data" gets just copied here! Ownership of the data doesn't get transferred here!
        // (This is to accomplish RAII(and resource lifetimes to be tied to objects' lifetimes) and copying to work correctly)
        Buffer(
            void* data,
            size_t elementSize,
            size_t dataLength,
            uint32_t bufferUsageFlags,
            BufferUpdateFrequency bufferUpdateFrequency
        );
    };
}
