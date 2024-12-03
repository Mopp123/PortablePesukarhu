#pragma once

#include "Pesukarhu/resources/Texture.h"
#include "Pesukarhu/resources/Mesh.h"
#include "Pesukarhu/graphics/Renderer.h"
#include "Pesukarhu/graphics/Pipeline.h"
#include "Pesukarhu/graphics/Buffers.h"
#include "Pesukarhu/graphics/Descriptors.h"
#include "Pesukarhu/graphics/CommandBuffer.h"
#include "Pesukarhu/graphics/RenderCommand.h"
#include "Pesukarhu/graphics/shaders/Shader.h"

#include "Pesukarhu/ecs/components/renderable/TerrainRenderable.h"


namespace pk
{
    struct TerrainRenderData
    {
        mat4 transformationMatrix = mat4(1.0f);
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

        // *Using renderable's terrain mesh resource id as kind of like "batch identifier"
        std::unordered_map<PK_id, TerrainRenderData> _toRender;
        // "identifiers" of terrains submitted on current frame
        // *Used to determine if terrain was deleted and dont want to render it anymore
        std::set<PK_id> _submittedTerrains;


    public:
        TerrainRenderer();
        ~TerrainRenderer();

        virtual void submit(
            const Component* const renderableComponent,
            const mat4& transformation,
            void* pCustomData,
            size_t customDataSize
        );
        virtual void render();

        virtual void flush();

        virtual void freeDescriptorSets();

        virtual void onSceneSwitch();

    protected:
        virtual void initPipeline();

    private:
        TerrainRenderData createTerrainRenderData(
            const TerrainRenderable* pRenderable,
            mat4 transformationMatrix
        );

        void createRenderDataDescriptorSets(
            const TerrainRenderable * const pRenderable,
            TerrainRenderData& target
        );
    };
}
