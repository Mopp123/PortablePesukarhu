#pragma once

#include "Buffers.h"
#include "shaders/Shader.h"
#include <vector>

namespace pk
{
    enum DescriptorType
    {
        DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 0x1,
        DESCRIPTOR_TYPE_UNIFORM_BUFFER = 0x2
    };


    class DescriptorSetLayoutBinding
    {
    private:
        uint32_t _binding = 0;

    public:
        DescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, DescriptorType type, unsigned int shaderStageFlags) :
            _binding(binding)
        {}

        virtual ~DescriptorSetLayoutBinding() {}
        inline uint32_t getBinding() const { return _binding; }
    };


    class DescriptorSetLayout
    {
    private:
        // NOTE: Not sure yet how to handle layout bindings here.. or handle at all on this level?

    public:
        // If using just a single binding
        DescriptorSetLayout(uint32_t binding, uint32_t descriptorCount, DescriptorType type, unsigned int shaderStageFlags) {}
        // For using multiple bindings
        DescriptorSetLayout(const std::vector<DescriptorSetLayoutBinding>& layoutBindings) {}
        virtual ~DescriptorSetLayout() {}

        // Don't remember why earlier I had separate func for that...
        // void destroy();
    };


    class DescriptorSet
    {
    private:
        std::vector<Buffer*> _pBuffers;

    public:
        DescriptorSet(DescriptorSetLayout layout, uint32_t descriptorCount, std::vector<Buffer*> pBuffers) :
            _pBuffers(pBuffers)
        {}

        virtual ~DescriptorSet() {}
    };

}
