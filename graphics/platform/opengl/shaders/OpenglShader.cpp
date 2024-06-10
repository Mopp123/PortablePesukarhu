#include "OpenglShader.h"
#include "core/Debug.h"
#include "graphics/platform/opengl/OpenglContext.h"
#include <vector>
#include <sstream>
#include <set>


namespace pk
{
    namespace opengl
    {
        static std::set<std::string> s_ESSLBasicTypes = {
            "bool",
            "int",
            "uint",
            "float",
            "double",
            "ivec2",
            "ivec3",
            "ivec4",
            "vec2",
            "vec3",
            "vec4",
            "mat4",
            "sampler2D"
        };


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
            ShaderVersion shaderVersion,
            const OpenglShader* pVertexShader,
            const OpenglShader* pFragmentShader
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
                    // NOTE: Below not tested!! May not work!
                    if (shaderVersion == ShaderVersion::ESSL1)
                    {
                        Debug::log(
                            "\t->Version was ESSL1. Attempting to get attrib and uniform locations..."
                        );
                        findLocationsESSL1(_pVertexShader->getSource());
                        findLocationsESSL1(_pFragmentShader->getSource());
                    }
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

        // NOTE: Works only with Opengl ES Shading language v1
        //  * NOT TESTED, MAY NOT WORK!
        //  * Doesn't work properly if source contains /**/ kind of comments!
        // Supposed to be used to automatically get locations of attribs and uniforms
        // NOTE: why source not in as ref?
        void OpenglShaderProgram::findLocationsESSL1(const std::string shaderSource)
        {
            std::string line = "";
            std::istringstream in(shaderSource);

            std::unordered_map<std::string, std::vector<std::string>> structList;
            bool recordStruct = false;
            std::string recordStructName = "";

            while (getline(in, line))
            {
                std::vector<std::string> components;
                size_t nextDelim = 0;
                // remove ';' completely, we dont need that here for anythin..
                size_t endpos = line.find(";");
                if (endpos != std::string::npos)
                    line.erase(endpos, 1);

                while ((nextDelim = line.find(" ")) != std::string::npos)
                {
                    std::string component = line.substr(0, nextDelim);
                    if (component != " " && component != "\t" && component != "\0")
                        components.push_back(component);
                    line.erase(0, nextDelim + 1);
                }
                components.push_back(line);

                if (!recordStruct)
                {
                    if (components.size() == 2)
                    {
                        if (components[0] == "struct")
                        {
                            recordStructName = components[1];
                            recordStruct = true;
                        }
                    }
                    if (components.size() >= 3)
                    {
                        if (components[0] == "attribute")
                        {
                            _attribLocations.push_back(glGetAttribLocation(_programID, components[2].c_str()));
                        }
                        else if (components[0] == "uniform")
                        {
                            const std::string& type = components[1];
                            if (s_ESSLBasicTypes.find(type) != s_ESSLBasicTypes.end())
                            {
                                _uniformLocations.push_back(glGetUniformLocation(_programID, components[2].c_str()));
                            }
                            else
                            {
                                // If uniform struct, need to find all like: "uniformstructname.values"
                                const std::string& structName = components[1];
                                if (structList.find(structName) != structList.end())
                                {
                                    const std::string& uniformName = components[2];
                                    for (std::string s : structList[structName])
                                    {
                                        std::string finalUniformName = uniformName + "." + s;
                                        _uniformLocations.push_back(glGetUniformLocation(_programID, finalUniformName.c_str()));
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (components.size() > 0)
                    {
                        if (components[0] == "}")
                        {
                            recordStructName.clear();
                            recordStruct = false;
                        }
                        else if (components[0] != "{" && components.size() >= 2)
                        {
                            structList[recordStructName].push_back(components[1]);
                        }
                    }
                }
            }
        }

        // TODO: delete commented out?
        //int32_t OpenglShaderProgram::getAttribLocation(const char* name) const
        //{
        //    return glGetAttribLocation(_programID, name);
        //}

        //int32_t OpenglShaderProgram::getUniformLocation(const char* name) const
        //{
        //    return glGetUniformLocation(_programID, name);
        //}

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
