#pragma once

#include <GL/glew.h>
#include "../Common.h"

#include <vector>

namespace pk
{

	enum VertexBufferUsage
	{
		PK_BUFFER_USAGE_STATIC,
		PK_BUFFER_USAGE_DYNAMIC
	};


	class VertexBuffer
	{
	protected:
		std::vector<PK_float> _data;

		VertexBufferUsage _usage = VertexBufferUsage::PK_BUFFER_USAGE_STATIC;

	public:

		VertexBuffer(const std::vector<PK_float>& data, VertexBufferUsage usage) : _data(data), _usage(usage) {}
		virtual ~VertexBuffer() {}
		virtual void update(const std::vector<PK_float>& newData, int offset, int size) {}
		inline const std::vector<PK_float>& getData() const { return _data; }
	};


	class IndexBuffer
	{
	protected:
		std::vector<PK_ushort> _data;

	public:

		IndexBuffer(const std::vector<PK_ushort>& data) : _data(data) {}
		virtual ~IndexBuffer() {}
		inline const std::vector<PK_ushort>& getData() const { return _data; }
	};


	class VertexBufferElement
	{
	protected:

		PK_uint _location = 0;
		PK_ShaderDatatype _type = PK_ShaderDatatype::PK_FLOAT;

	public:

		VertexBufferElement(PK_uint location, PK_ShaderDatatype dataType) :
			_location(location),
			_type(dataType)
		{}

		inline PK_uint getLocation() const { return _location; }
		inline PK_ShaderDatatype getType() const { return _type; }
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