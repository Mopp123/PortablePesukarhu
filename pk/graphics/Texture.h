#pragma once

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
	public:

		Texture()
		{}
		virtual ~Texture()
		{}
	};
}