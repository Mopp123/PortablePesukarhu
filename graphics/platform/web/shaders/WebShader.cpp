#include "WebShader.h"
#include "../../../../core/Debug.h"


namespace pk
{
    namespace web
    {
        WebShader::WebShader(const std::string& vertexSource, const std::string& fragmentSource)
        {

            _vertexShaderID = createShaderStage(vertexSource, GL_VERTEX_SHADER);
            _fragmentShaderID = createShaderStage(fragmentSource, GL_FRAGMENT_SHADER);

            _programID = glCreateProgram();

            if (_vertexShaderID && _fragmentShaderID && _programID)
            {
                glAttachShader(_programID, _vertexShaderID);
                glAttachShader(_programID, _fragmentShaderID);

                glLinkProgram(_programID);

                GLint linkStatus = 0;
                glGetProgramiv(_programID, GL_LINK_STATUS, &linkStatus);
                if (!linkStatus)
                {
                    GLint infoLogLength = 0;
                    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);

                    if (infoLogLength > 0)
                    {
                        char* infoLog = new char[infoLogLength];

                        glGetProgramInfoLog(_programID, infoLogLength, &infoLogLength, infoLog);

                        std::string info_str(infoLog);

                        Debug::log("Failed to link WebShader\nInfoLog:\n" + info_str, Debug::MessageType::PK_FATAL_ERROR);

                        delete[] infoLog;
                    }
                }
                else
                {
                    glValidateProgram(_programID);
                    Debug::log("WebShader created successfully");
                }
            }
            else
            {
                Debug::log("Failed to create WebShader object", Debug::MessageType::PK_FATAL_ERROR);
            }
        }

        WebShader::~WebShader()
        {
            glUseProgram(0);

            glDetachShader(_programID, _vertexShaderID);
            glDetachShader(_programID, _fragmentShaderID);

            glDeleteShader(_vertexShaderID);
            glDeleteShader(_fragmentShaderID);

            glDeleteProgram(_programID);
        }

        int WebShader::getAttribLocation(const char* name) const
        {
            return glGetAttribLocation(_programID, name);
        }

        int WebShader::getUniformLocation(const char* name) const
        {
            return glGetUniformLocation(_programID, name);
        }

        void WebShader::setUniform(int location, const mat4& matrix) const
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, matrix.getRawArray());
        }

        void WebShader::setUniform(int location, const vec3& v) const
        {
            glUniform3f(location, v.x, v.y, v.z);
        }

        void WebShader::setUniform(int location, float val) const
        {
            glUniform1fv(location, 1, &val);
        }

        void WebShader::setUniform1i(int location, int val) const
        {
            glUniform1i(location, val);
        }


        uint32_t WebShader::createShaderStage(const std::string& source, GLenum type)
        {
            GLuint shaderID = glCreateShader(type);

            if (shaderID == 0)
            {
                Debug::log("Failed to create shader stage(type: " + std::to_string(type) + ")", Debug::MessageType::PK_FATAL_ERROR);
                return 0;
            }

            const char* source_cstr = source.c_str();
            glShaderSource(shaderID, 1, &source_cstr, NULL);

            glCompileShader(shaderID);

            GLint compileStatus = 0;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);

            if (!compileStatus)
            {
                GLint infoLogLength = 0;
                glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

                if (infoLogLength > 0)
                {
                    char* infoLog = new char[infoLogLength];

                    glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, infoLog);

                    Debug::log("Failed to compile shader stage(type: " + std::to_string(type) + ")\nInfoLog:\n" + infoLog, Debug::MessageType::PK_FATAL_ERROR);

                    delete[] infoLog;
                }

                glDeleteShader(shaderID);
            }

            Debug::log("Shader stage created successfully(type: " + std::to_string(type) + ")");

            return shaderID;
        }

    }
}
