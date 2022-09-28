#pragma once

#include "../System.h"
#include "../../../utils/pkmath.h"
#include <vector>

namespace pk
{

	class SpriteAnimator : public Updateable
	{
	private:
		int _currentFrameIndex = 0;
		
		float _changeFrameCooldown = 0.0f;
		float _maxChangeFrameCooldown = 1.0f;
		
		vec2 _anchorOffset = {0, 0};
		std::vector<vec2> _frameTexOffsets;
		vec2 _currentOffset;

		float _speed = 0.0f;

		bool _isPlaying = false;
		bool _enableLooping = false;

	public:
		SpriteAnimator(const vec2& anchorOffset, const std::vector<vec2>& frameOffsets, float speed);
		SpriteAnimator(const std::vector<vec2>& frameOffsets, float speed);
		~SpriteAnimator();

		virtual void update();

		void play();
		void stop();

		const vec2& getCurrentTexOffset() const;

		inline void enableLooping(bool arg) { _enableLooping = arg; }
		inline bool isPlaying() const { return _isPlaying; }
	};
}
