#pragma once

#include "resources/Texture.h"
#include "resources/Mesh.h"
#include "graphics/Renderer.h"
#include "graphics/Pipeline.h"
#include "graphics/Buffers.h"
#include "graphics/Descriptors.h"
#include "graphics/CommandBuffer.h"
#include "graphics/RenderCommand.h"
#include "graphics/shaders/Shader.h"

#include "ecs/components/renderable/TerrainRenderable.h"

namespace pk
{
    struct TerrainRenderData
    {
        std::vector<Buffer*> pVertexBuffer; // one for each swapchain img
        Buffer* pIndexBuffer = nullptr;

        std::vector<const Texture_new*> channelTextures;
        const Texture_new* pBlendmapTexture = nullptr;
        mat4 transformationMatrix = mat4(1.0f);
        TerrainRenderable* pRenderable = nullptr;
        std::vector<DescriptorSet*> materialDescriptorSet;
    };


    class TerrainRenderer : public Renderer
    {
    private:
        Shader* _pVertexShader = nullptr;
        Shader* _pFragmentShader = nullptr;

        VertexBufferLayout _vertexBufferLayout;

        // TODO: Requires recreating if swapchain img count changes!
        // *atm not using any ubos for terrain materials..
        // std::vector<Buffer*> _materialPropsUniformBuffers;
        DescriptorSetLayout _materialDescSetLayout;
        std::vector<DescriptorSet*> _materialDescriptorSet;

        //std::vector<const TerrainRenderable*> _toRender;

        // *Using renderable's pointer as kind of like "batch identifier"
        std::unordered_map<const TerrainRenderable*, TerrainRenderData> _toRender;
        // "identifiers" of terrains submitted on current frame
        // *Used to determine if terrain was deleted and dont want to render it anymore
        std::set<const TerrainRenderable*> _submittedTerrains;


    public:
        TerrainRenderer();
        ~TerrainRenderer();

        virtual void submit(const Component* const renderableComponent, const mat4& transformation);
        virtual void render();

        virtual void flush();

        virtual void freeDescriptorSets();

        virtual void handleSceneSwitch();

    protected:
        virtual void initPipeline();

    private:
        TerrainRenderData createTerrainRenderData(
            const TerrainRenderable* pRenderable,
            mat4 transformationMatrix
        );
        // NOTE: Vertex data is generated from 0,0,0 towards +z direction
        void createRenderDataBuffers(
            const std::vector<float>& heightmap,
            float tileWidth,
            TerrainRenderData& target
        );
        void createRenderDataDescriptorSets(TerrainRenderData& target);

        void deleteRenderDataBuffers();
    };
}
