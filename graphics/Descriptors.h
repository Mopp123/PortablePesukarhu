#pragma once

#include "Buffers.h"
#include "resources/Texture.h"
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


    /*  Specifies Uniform buffer struct's layout
     *  NOTE: This is only used by web platform (webgl1/ES2)
     *  since no actual uniform buffers available
     *
     *  Example:
     *
     *  struct DirectionalLight
     *  {
     *      vec3 direction;
     *      vec3 color;
     *  };
     *
     *  above should specify UniformBufferInfo's layout as:
     *  {
     *      ShaderDataType::Float3, ShaderDataType::Float3
     *  }
     *
     *  location index means how many"th" uniform in shader's all uniforms this one is at
     * */
    struct UniformInfo
    {
        int locationIndex;
        ShaderDataType type;
        int arrayLen = 1;
    };


    class DescriptorSetLayoutBinding
    {
    private:
        uint32_t _binding = 0;
        DescriptorType _type = DescriptorType::DESCRIPTOR_TYPE_NONE;
        unsigned int _shaderStageFlags = 0;

        std::vector<UniformInfo> _uniformInfo;

    public:
        // NOTE: Don't remember why I allowed having multiple shader stage flags...
        DescriptorSetLayoutBinding(
            uint32_t binding,
            uint32_t descriptorCount,
            DescriptorType type,
            unsigned int shaderStageFlags,
            std::vector<UniformInfo> uniformInfo = {}
        ) :
            _binding(binding),
            _type(type),
            _shaderStageFlags(shaderStageFlags),
            _uniformInfo(uniformInfo)
        {}

        DescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& other) :
            _binding(other._binding),
            _type(other._type),
            _shaderStageFlags(other._shaderStageFlags),
            _uniformInfo(other._uniformInfo)
        {}

        virtual ~DescriptorSetLayoutBinding() {}

        inline uint32_t getBinding() const { return _binding; }
        inline DescriptorType getType() const { return _type; }
        inline uint32_t getShaderStageFlags() const { return _shaderStageFlags; }
        inline const std::vector<UniformInfo>& getUniformInfo() const { return _uniformInfo; }
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

        inline const std::vector<DescriptorSetLayoutBinding>& getBindings() const { return _bindings; }
    };


    class DescriptorSet
    {
    private:
        DescriptorSetLayout _layout;
        std::vector<const Buffer*> _pBuffers;
        std::vector<const Texture_new*> _pTextures;

    public:
        DescriptorSet(
            DescriptorSetLayout layout,
            uint32_t descriptorCount, // Don't remember what this was exactly..? and why..?
            std::vector<const Buffer*> pBuffers
        ) :
            _layout(layout),
            _pBuffers(pBuffers)
        {}

        DescriptorSet(
            DescriptorSetLayout layout,
            uint32_t descriptorCount, // Don't remember what this was exactly..? and why..?
            std::vector<const Texture_new*> pTextures
        ) :
            _layout(layout),
            _pTextures(pTextures)
        {}

        DescriptorSet(
            DescriptorSetLayout layout,
            uint32_t descriptorCount, // Don't remember what this was exactly..? and why..?
            std::vector<const Texture_new*> pTextures,
            std::vector<const Buffer*> pBuffers
        ) :
            _layout(layout),
            _pBuffers(pBuffers),
            _pTextures(pTextures)
        {}

        virtual ~DescriptorSet() {}

        bool isValid(const DescriptorSetLayout& layout) const;

        inline const DescriptorSetLayout& getLayout() const { return _layout; }
        inline const std::vector<const Buffer*>& getBuffers() const { return _pBuffers; }
        inline const std::vector<const Texture_new*>& getTextures() const { return _pTextures; }
    };
}
