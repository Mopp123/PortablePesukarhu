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

        //Buffer* _pVertexBuffer = nullptr;
        //Buffer* _pIndexBuffer = nullptr;

        VertexBufferLayout _vertexBufferLayout;
        VertexBufferLayout _instanceBufferLayout;

        DescriptorSetLayout _textureDescSetLayout;

        //Mesh* _pTestMesh = nullptr;

        std::unordered_map<PK_id, Mesh*> _batchMeshMapping;
        BatchContainer _batchContainer;

    public:
        StaticRenderer();
        ~StaticRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render(const Camera& cam);

        virtual void flush();

    protected:
        virtual void initPipeline();
        virtual void freeDescriptorSets();
    };
}
