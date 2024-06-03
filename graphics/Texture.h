#pragma once

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

	class Texture
	{
	protected:

		TextureSampler _sampler;
		int _width = 0;
		int _height = 0;
		int _channels = 3;
		// Can be used to specify how many "individual sprites" inside a "texture atlas", containing multiple sprites
		int _tiling = 1;
	public:

		Texture(TextureSampler sampler, int tiling = 1) :
			_sampler(sampler), _tiling(tiling)
		{}
		Texture(TextureSampler sampler, int width, int height, int channels) :
			_sampler(sampler), _width(width), _height(height), _channels(channels)
		{}
		virtual ~Texture()
		{}
		virtual void update(void* data, int slot = 0) = 0;

		inline const TextureSampler& getSampler() const { return _sampler; }
		inline int getChannels() const { return _channels; }
		inline int getTiling() const { return _tiling ;}

	};
}
