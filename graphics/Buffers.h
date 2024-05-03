#pragma once


#include <vector>
// Not sure is it just YCM issues with emscripten why syntax error if not including this
#include <cstdint>

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

    enum VertexBufferUsage
    {
        PK_BUFFER_USAGE_STATIC,
        PK_BUFFER_USAGE_DYNAMIC
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

        IndexBuffer(const std::vector<uint32_t>& data) {}
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

    public:

        VertexBufferLayout(std::vector<VertexBufferElement> elems) :
            _elements(elems)
        {}

        inline const std::vector<VertexBufferElement>& getElements() const { return _elements; }
    };
}
