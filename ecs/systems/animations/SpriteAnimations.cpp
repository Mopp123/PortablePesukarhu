
#include "SpriteAnimations.h"
#include "../../../core/Debug.h"
#include "../../../core/Timing.h"


namespace pk
{

	SpriteAnimator::SpriteAnimator(const vec2& anchorOffset, const std::vector<vec2>& frameOffsets, float speed):
		SpriteAnimator(frameOffsets, speed)
	{
		_anchorOffset = anchorOffset;
		_currentOffset = _frameTexOffsets[0] + _anchorOffset;
	}

	SpriteAnimator::SpriteAnimator(const std::vector<vec2>& frameOffsets, float speed) :
		_frameTexOffsets(frameOffsets), _speed(speed)
	{
		if (_frameTexOffsets.empty())
			Debug::log("Created SpriteAnimator with no frame offsets", Debug::MessageType::PK_ERROR);
		
		_currentOffset = _frameTexOffsets[0] + _anchorOffset;
	}

	SpriteAnimator::~SpriteAnimator()
	{}

	void SpriteAnimator::update()
	{
		if(_isPlaying)
		{
			if (_changeFrameCooldown <= 0.0f)
			{
				_changeFrameCooldown = _maxChangeFrameCooldown;
				_currentFrameIndex++;
				if (_currentFrameIndex > _frameTexOffsets.size() - 1)
				{
					if (_enableLooping)
					{
						play();
					}
					else
					{
						stop();
						return;
					}
				}
			}
			_changeFrameCooldown -= _speed * Timing::get_delta_time();
			if (_currentFrameIndex < _frameTexOffsets.size())
				_currentOffset = _frameTexOffsets[_currentFrameIndex] + _anchorOffset;
		}
	}

	void SpriteAnimator::play()
	{
		_isPlaying = true;
		_currentFrameIndex = 0;
	}

	void SpriteAnimator::stop()
	{
		_isPlaying = false;
	}

	const vec2& SpriteAnimator::getCurrentTexOffset() const
	{
		return _currentOffset;
	}

}
