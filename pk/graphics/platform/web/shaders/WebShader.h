#pragma once

#include "../../../shaders/Shader.h"
#include "../../../../Common.h"

namespace pk
{
	namespace web
	{
		class WebShader : public Shader
		{
		public:

			PK_uint _vertexShaderID = 0;
			PK_uint _fragmentShaderID = 0;
			PK_uint _programID = 0;

		public:

			WebShader(const std::string& vertexSource, const std::string& fragmentSource);
			~WebShader();

			PK_int getAttribLocation(const char* name);

			inline PK_uint getProgramID() const { return _programID; }


		private:

			PK_uint createShaderStage(const std::string& source, GLenum type);

		};
	}
}