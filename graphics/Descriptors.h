#pragma once

#include "Buffers.h"
#include "shaders/Shader.h"
#include <vector>

namespace pk
{
    enum DescriptorType
    {
        DESCRIPTOR_TYPE_NONE = 0x0,
        DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 0x1,
        DESCRIPTOR_TYPE_UNIFORM_BUFFER = 0x2
    };


    class DescriptorSetLayoutBinding
    {
    private:
        uint32_t _binding = 0;
        DescriptorType _type = DescriptorType::DESCRIPTOR_TYPE_NONE;
        unsigned int _shaderStageFlags = 0;

    public:
        // NOTE: Don't remember why I allowed having multiple shader stage flags...
        DescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, DescriptorType type, unsigned int shaderStageFlags) :
            _binding(binding),
            _type(type),
            _shaderStageFlags(shaderStageFlags)
        {}

        DescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& other) :
            _binding(other._binding),
            _type(other._type),
            _shaderStageFlags(other._shaderStageFlags)
        {}

        virtual ~DescriptorSetLayoutBinding() {}

        inline uint32_t getBinding() const { return _binding; }
        inline DescriptorType getType() const { return _type; }
        inline uint32_t getShaderStageFlags() const { return _shaderStageFlags; }
    };


    class DescriptorSetLayout
    {
    private:
        std::vector<DescriptorSetLayoutBinding> _bindings;

    public:
        // NOTE: Not sure if copying works here properly, not tested..
        DescriptorSetLayout(const std::vector<DescriptorSetLayoutBinding>& layoutBindings) :
            _bindings(layoutBindings)
        {}
        virtual ~DescriptorSetLayout() {}
        // Don't remember why earlier I had separate func for destroying...
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
