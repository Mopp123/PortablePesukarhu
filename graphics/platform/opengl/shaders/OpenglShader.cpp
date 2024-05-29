#include "OpenglShader.h"
#include "core/Debug.h"
#include "graphics/platform/opengl/OpenglContext.h"
#include <vector>
#include <sstream>


namespace pk
{
    namespace opengl
    {
        // NOTE: Doesn't work properly if source contains /**/ kind of comments!
        // NOTE: NOT COMPLETE YET!!!
        // Supposed to be used to automatically get locations of uniforms
        std::vector<std::string> get_uniform_locations(const std::string shaderSource)
        {
            std::string line = "";
            std::istringstream in(shaderSource);
            std::vector<std::string> uniforms;

            while (getline(in, line))
            {
                std::vector<std::string> components;
                size_t nextDelim = 0;
                while ((nextDelim = line.find(" ")) != std::string::npos)
                {
                    std::string component = line.substr(0, nextDelim);
                    if (component != " " && component != "\t" && component != "\0")
                        components.push_back(component);
                    line.erase(0, nextDelim + 1);
                }
                components.push_back(line);
                if (components.size() >= 3)
                {
                    if (components[0] == "uniform")
                    {
                        std::string& name = components[2];
                        size_t endpos = name.find(";");
                        name.erase(endpos, 1);
                        uniforms.push_back(name);
                    }
                }
            }
            return uniforms;
        }


        // NOTE: Not tested after moving this here from platform/web/shaders and
        // modifying a bit... may not work?
        OpenglShader::OpenglShader(const std::string& source, ShaderStageFlagBits stage):
            Shader(source, stage),
            _source(source)
        {
            GLenum stageType = opengl::to_gl_shader(stage);
            _shaderID = glCreateShader(stageType);

            if (_shaderID == 0)
            {
                Debug::log(
                    "Failed to create shader stage(type: " + std::to_string(stageType) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            const char* source_cstr = source.c_str();
            glShaderSource(_shaderID, 1, &source_cstr, NULL);

            glCompileShader(_shaderID);

            GLint compileStatus = 0;
            glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &compileStatus);

            if (!compileStatus)
            {
                GLint infoLogLength = 0;
                glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

                if (infoLogLength > 0)
                {
                    char* infoLog = new char[infoLogLength];

                    glGetShaderInfoLog(_shaderID, infoLogLength, &infoLogLength, infoLog);

                    Debug::log(
                        "Failed to compile shader stage(type: " + std::to_string(stageType) + ")\n"
                        "InfoLog:\n" + infoLog, Debug::MessageType::PK_FATAL_ERROR
                    );
                    delete[] infoLog;
                }
                glDeleteShader(_shaderID);
            }
            Debug::log(
                "Shader stage created successfully(type: " + std::to_string(stageType) + ")"
            );
        }

        OpenglShader::~OpenglShader()
        {
            // NOTE: DANGER! You need to unbind current shader
            // and detach this module from its' "program"
            // before destructing this!
            // (OpenglShaderProgram is responsible for detaching in its destructor)
            glDeleteShader(_shaderID);
        }


        OpenglShaderProgram::OpenglShaderProgram(
            OpenglShader* pVertexShader,
            OpenglShader* pFragmentShader
        )
        {
            _pVertexShader = pVertexShader;
            _pFragmentShader = pFragmentShader;

            _programID = glCreateProgram();
            const uint32_t vertexShaderID = _pVertexShader->getID();
            const uint32_t fragmentShaderID = _pFragmentShader->getID();

            if (vertexShaderID && fragmentShaderID && _programID)
            {
                glAttachShader(_programID, vertexShaderID);
                glAttachShader(_programID, fragmentShaderID);

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

                        Debug::log(
                            "Failed to link OpenglShader\nInfoLog:\n" + info_str,
                            Debug::MessageType::PK_FATAL_ERROR
                        );
                        delete[] infoLog;
                    }
                }
                else
                {
                    glValidateProgram(_programID);
                    Debug::log("OpenglShader created successfully");
                }
            }
            else
            {
                Debug::log(
                    "Failed to create OpenglShader object",
                    Debug::MessageType::PK_FATAL_ERROR
                );
            }
        }

        OpenglShaderProgram:: ~OpenglShaderProgram()
        {
            glUseProgram(0);
            glDetachShader(_programID, _pVertexShader->getID());
            glDetachShader(_programID, _pFragmentShader->getID());
            // NOTE: Not sure if vertex and fragment shaders need deleting before this?
            // ..those are detached so you'd think thats not required..
            // ...but never fucking know what happens with opengl...
            glDeleteProgram(_programID);
        }

        int OpenglShaderProgram::getAttribLocation(const char* name) const
        {
            return glGetAttribLocation(_programID, name);
        }

        int OpenglShaderProgram::getUniformLocation(const char* name) const
        {
            return glGetUniformLocation(_programID, name);
        }

        void OpenglShaderProgram::setUniform(int location, const mat4& matrix) const
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, matrix.getRawArray());
        }

        void OpenglShaderProgram::setUniform(int location, const vec3& v) const
        {
            glUniform3f(location, v.x, v.y, v.z);
        }

        void OpenglShaderProgram::setUniform(int location, float val) const
        {
            glUniform1fv(location, 1, &val);
        }

        void OpenglShaderProgram::setUniform1i(int location, int val) const
        {
            glUniform1i(location, val);
        }
    }
}
