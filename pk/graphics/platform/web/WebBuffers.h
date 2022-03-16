#pragma once


#include "../../Buffers.h"

namespace pk
{
	namespace web
	{

		class WebVertexBuffer : public VertexBuffer
		{
		private:

			PK_uint _id = 0;

		public:

			WebVertexBuffer(const std::vector<PK_float>& data, VertexBufferUsage usage);
			~WebVertexBuffer();


			inline PK_uint getID() const { return _id; }
		};

		class WebIndexBuffer : public IndexBuffer
		{
		private:

			PK_uint _id = 0;

		public:

			WebIndexBuffer(const std::vector<PK_ushort>& data);
			~WebIndexBuffer();

			inline PK_uint getID() const { return _id; }

		};
	}
}