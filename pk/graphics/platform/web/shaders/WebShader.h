#pragma once

#include "../../../shaders/Shader.h"
#include "../../../../Common.h"

#include "../../../../utils/pkmath.h"


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

			PK_int getAttribLocation(const char* name) const;
			PK_int getUniformLocation(const char* name) const;
			void setUniform(PK_int location, const mat4& matrix) const;
			void setUniform(PK_int location, const vec3& v) const;
			void setUniform(PK_int location, float val) const;

			inline PK_uint getProgramID() const { return _programID; }


		private:

			PK_uint createShaderStage(const std::string& source, GLenum type);

		};
	}
}