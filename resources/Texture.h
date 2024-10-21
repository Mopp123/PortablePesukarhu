#pragma once

#include "Resource.h"
#include "Image.h"
#include <cstdint>


namespace pk
{
    enum TextureSamplerAddressMode
    {
        PK_SAMPLER_ADDRESS_MODE_REPEAT,
        PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
    };


    enum TextureSamplerFilterMode
    {
        PK_SAMPLER_FILTER_MODE_LINEAR,
        PK_SAMPLER_FILTER_MODE_NEAR
    };


    class TextureSampler
    {
    private:
        TextureSamplerFilterMode _filterMode;
        TextureSamplerAddressMode _addressMode;
        uint32_t _mipLevelCount;

    public:
        TextureSampler(
                TextureSamplerFilterMode filterMode = TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR,
                TextureSamplerAddressMode addressMode = TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_REPEAT,
                uint32_t mipLevelCount = 1
                ) :
            _filterMode(filterMode), _addressMode(addressMode), _mipLevelCount(mipLevelCount)
        {}

        virtual ~TextureSampler() {}

        inline TextureSamplerFilterMode		getFilterMode() const { return _filterMode; }
        inline TextureSamplerAddressMode	getAddressMode() const { return _addressMode; }
        inline uint32_t						getMipLevelCount() const { return _mipLevelCount; }
    };


    class Texture : public Resource
    {
    protected:
        TextureSampler _sampler;
        uint32_t _imgResourceID = 0;

    public:
        Texture(const Texture&) = delete;
        virtual ~Texture();

        virtual void load() {}
        virtual void save() {}

        virtual void update(
            void* pData,
            size_t dataSize,
            size_t width,
            size_t height,
            uint32_t glTextureUnit // atm needed for gl fuckery..
        ) {}

        // NOTE: Why pImgData is not const here?
        static Texture* create(
            uint32_t imgResourceID,
            TextureSampler sampler
        );

        // TODO: ?
        // virtual void update(void* data, int slot = 0) = 0;
        inline const TextureSampler& getSampler() const { return _sampler; }

    protected:
        Texture(TextureSampler sampler, uint32_t imgResourceID);
    };


    // TODO: Delete from here and maybe do some Tex atlas component?
    class TextureAtlas
    {
    private:
        uint32_t _textureResourceID = 0;
        int _rowCount = 1;

    public:
        TextureAtlas(
            uint32_t textureResourceID,
            int rowCount
        );
        ~TextureAtlas();

        inline int getRowCount() const { return _rowCount; }
    };
}
