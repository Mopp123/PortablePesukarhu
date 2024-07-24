#pragma once

#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Texture.h"
#include "graphics/Mesh.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"

#include <unordered_map>


namespace pk
{
    class StaticRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;
        VertexBufferLayout _instanceBufferLayout;

        DescriptorSetLayout _textureDescSetLayout;

        std::unordered_map<PK_id, Mesh*> _batchMeshCache;
        BatchContainer _batchContainer;

    public:
        StaticRenderer();
        ~StaticRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render(const Camera& cam);

        virtual void flush();

        virtual void freeDescriptorSets();

    protected:
        virtual void initPipeline();
    };
}
