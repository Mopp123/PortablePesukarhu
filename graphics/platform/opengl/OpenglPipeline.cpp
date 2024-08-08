#include <GL/glew.h>
#include "graphics/platform/opengl/OpenglPipeline.h"
#include "core/Debug.h"
#include "graphics/Context.h"


namespace pk
{
    namespace opengl
    {
        void OpenglPipeline::init(
            const std::vector<VertexBufferLayout>& vertexBufferLayouts,
            const std::vector<DescriptorSetLayout>& descriptorLayouts,
            const Shader* pVertexShader, const Shader* pFragmentShader,
            float viewportWidth, float viewportHeight,
            const Rect2D viewportScissor,
            CullMode cullMode,
            FrontFace frontFace,
            bool enableDepthTest,
            DepthCompareOperation depthCmpOp,
            uint32_t pushConstantSize,
	          uint32_t pushConstantStageFlags
        )
        {
            _vertexBufferLayouts = vertexBufferLayouts;
            uint32_t vbLayoutConflictLocation = 0;
            if (!VertexBufferLayout::are_valid(_vertexBufferLayouts, &vbLayoutConflictLocation))
            {
                Debug::log(
                    "@OpenglPipeline::init "
                    "Invalid Vertex Buffer Layouts!"
                    "Layouts contained multiple elements with same location: " + std::to_string(vbLayoutConflictLocation),
                    Debug::MessageType::PK_FATAL_ERROR
                );
            }

            _descriptorLayouts = descriptorLayouts;
            _viewportWidth = viewportWidth;
            _viewportHeight = viewportHeight;

            if (_pShaderProgram)
            {
                Debug::log(
                    "@OpenglPipeline::init Attempted to init pipeline but shader program "
                    "already exists. cleanUp() required before initializing opengl pipelines!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }

            PK_byte graphicsAPI = Context::get_api_type();
            ShaderVersion shaderVersion = graphicsAPI == GRAPHICS_API_WEBGL ? ShaderVersion::ESSL1 : ShaderVersion::GLSL3;
            _pShaderProgram = new OpenglShaderProgram(
                shaderVersion,
                (const OpenglShader*)pVertexShader,
                (const OpenglShader*)pFragmentShader
            );

            _cullMode = cullMode;
            _frontFace = frontFace;

            _enableDepthTest = enableDepthTest;
            _depthCmpOp = depthCmpOp;
        }

        void OpenglPipeline::cleanUp()
        {
            // NOTE: gl shader program detaches its' "shader modules" in its' destructor
            // so no need to worry about those here
            delete _pShaderProgram;
            _pShaderProgram = nullptr;
        }
    }
}
