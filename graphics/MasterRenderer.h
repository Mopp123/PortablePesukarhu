#pragma once

#include <map>
#include "Renderer.h"
#include "../ecs/components/Camera.h"
#include "Swapchain.h"
#include "Buffers.h"
#include "Descriptors.h"


namespace pk
{
    // The most basic kind of uniform buffer data that almost all renderers require
    struct CommonUniforms
    {
        mat4 projectionMatrix;
        mat4 viewMatrix;
        vec4 camPos;
    };

    struct DirectionalLightProperties
    {
        vec4 direction;
        vec4 color;
    };


    class MasterRenderer
    {
    private:
        Swapchain* _pSwapchain = nullptr;
        std::map<ComponentType, Renderer*> _renderers;

        DescriptorSetLayout _commonDescriptorSetLayout;
        Buffer* _pCommonUniformBuffer = nullptr;
        DescriptorSet* _pCommonDescriptorSet = nullptr;

        DescriptorSetLayout _environmentDescriptorSetLayout;
        Buffer* _pEnvironmentUniformBuffer = nullptr;
        DescriptorSet* _pEnvironmentDescriptorSet = nullptr;

        DescriptorSetLayout _directionalLightDescriptorSetLayout;
        Buffer* _pDirectionalLightUniformBuffer = nullptr;
        DescriptorSet* _pDirectionalLightDescriptorSet = nullptr;

    public:
        MasterRenderer();
        ~MasterRenderer();
        void addRenderer(ComponentType renderableComponentType, Renderer* renderer);

        void render(
            const mat4& persProjMatrix,
            const mat4& viewMatrix,
            const vec3& cameraPos
        );
        void flush();

        Renderer* const getRenderer(ComponentType renderableType);

        // Frees all renderers' descriptor sets
        void freeDescriptorSets();

        inline const DescriptorSetLayout getCommonDescriptorSetLayout() const { return _commonDescriptorSetLayout; }
        inline const DescriptorSet* getCommonDescriptorSet() const { return _pCommonDescriptorSet; }

        inline const DescriptorSetLayout getEnvironmentDescriptorSetLayout() const { return _environmentDescriptorSetLayout; }
        inline const DescriptorSet* getEnvironmentDescriptorSet() const { return _pEnvironmentDescriptorSet; }

        inline const DescriptorSetLayout getDirectionalLightDescriptorSetLayout() const { return _directionalLightDescriptorSetLayout; }
        inline const DescriptorSet* getDirectionalLightDescriptorSet() const { return _pDirectionalLightDescriptorSet; }
    private:
        void handleWindowResize();
    };
}
