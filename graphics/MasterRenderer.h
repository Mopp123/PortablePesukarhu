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
    };

    class MasterRenderer
    {
    private:
        Swapchain* _pSwapchain = nullptr;
        std::map<ComponentType, Renderer*> _renderers;

        DescriptorSetLayout _commonDescriptorSetLayout;
        Buffer* _pCommonUniformBuffer = nullptr;
        DescriptorSet* _pCommonDescriptorSet = nullptr;

    public:
        MasterRenderer();
        ~MasterRenderer();
        void addRenderer(ComponentType renderableComponentType, Renderer* renderer);

        void render(const Camera& cam);

        Renderer* const getRenderer(ComponentType renderableType);

        inline const DescriptorSetLayout getCommonDescriptorSetLayout() const { return _commonDescriptorSetLayout; }
        inline const DescriptorSet* getCommonDescriptorSet() const { return _pCommonDescriptorSet; }

    private:
        void handleWindowResize();
    };
}
