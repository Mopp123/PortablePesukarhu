#pragma once


#include <vector>
// Not sure is it just YCM issues with emscripten why syntax error if not including this
#include <cstdint>
#include <cstring>

// Don't remember why this was included in the first place!?
// TODO: Delete?
// #include <GL/glew.h>
#include "../Common.h"

namespace pk
{
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
        Float4
    };

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
        BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x1,
        BUFFER_USAGE_INDEX_BUFFER_BIT = 0x2
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

    public:
        VertexBufferLayout(std::vector<VertexBufferElement> elems, VertexInputRate inputRate) :
            _elements(elems),
            _inputRate(inputRate)
        {}

        inline const std::vector<VertexBufferElement>& getElements() const { return _elements; }
    };


    class Buffer
    {
    protected:
        void* _data = nullptr;
        size_t _dataElemSize = 0; // size of a single entry in data
        size_t _dataLength = 0; // number of elements in the data

    public:
        Buffer(const Buffer&) = delete;
        virtual ~Buffer();

        inline const void* getData() const { return _data; }
        inline size_t getDataLength() const { return _dataLength; }
        inline size_t getDataElemSize() const { return _dataElemSize; }

        // NOTE: Might not work.. not tested yet...
        static Buffer* create(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags);

    protected:
        // *NOTE! "elementSize" single element's size in "data buffer"
        // *NOTE! "dataLength" number of "elements" in the "data buffer" (NOT total size)
        // *NOTE! "Data" gets just copied here! Ownership of the data doesn't get transferred here!
        // (This is to accomplish RAII(and resource lifetimes to be tied to objects' lifetimes) and copying to work correctly)
        Buffer(void* data, size_t elementSize, size_t dataLength, uint32_t bufferUsageFlags);
    };
}
