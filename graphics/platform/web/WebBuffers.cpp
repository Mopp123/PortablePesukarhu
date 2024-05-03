
#include "WebBuffers.h"
#include <GL/glew.h>

#include "../../../core/Debug.h"

namespace pk
{
	namespace web
	{


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
	}
}
