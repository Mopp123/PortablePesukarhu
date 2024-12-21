#include "Shader.h"
#include "pesukarhu/graphics/Context.h"
#include "pesukarhu/graphics/platform/opengl/shaders/OpenglShader.h"
#include "pesukarhu/utils/FileUtils.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    std::string Shader::s_shaderRootWeb = "assets/shaders/";
    std::string Shader::s_shaderRootOpengl = "assets/shaders/opengl/";

    Shader* Shader::create_from_source(const std::string& shaderSource, ShaderStageFlagBits stage)
    {
        const GraphicsAPI api = Context::get_graphics_api();
        switch(api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return new opengl::OpenglShader(shaderSource, stage);
            case GraphicsAPI::PK_GRAPHICS_API_OPENGL:
                return new opengl::OpenglShader(shaderSource, stage);
            default:
                Debug::log(
                    "Attempted to create shader but invalid graphics context api(" + std::to_string(api) + ")",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
        }
    }

    Shader* Shader::create_from_file(const std::string& filepath, ShaderStageFlagBits stage)
    {
        Debug::log("___TEST___creating shader from file: \n" + filepath);

        const std::string source = load_text_file(filepath);
        if (source.empty())
        {
            Debug::log(
                "@Shader::create(2) "
                "Failed to get shader source from file: " + filepath,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        return create_from_source(source, stage);
    }

    std::string Shader::get_shader_path(const std::string& shaderFile)
    {
        GraphicsAPI api = Application::get()->getGraphicsContext()->get_graphics_api();
        switch (api)
        {
            case GraphicsAPI::PK_GRAPHICS_API_WEBGL:
                return s_shaderRootWeb + shaderFile;
            case GraphicsAPI::PK_GRAPHICS_API_OPENGL:
                return s_shaderRootOpengl + shaderFile;
            default:
                Debug::log(
                    "@Shader::get_shader_path "
                    "Invalid graphics api: " + std::to_string(api),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return "";
        }
    }
}
