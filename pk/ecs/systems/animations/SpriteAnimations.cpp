
#include "SpriteAnimations.h"
#include "../../../core/Debug.h"
#include "../../../core/Timing.h"

namespace pk
{

	SpriteAnimator::SpriteAnimator(const std::vector<vec2>& frameOffsets, float speed) :
		_frameTexOffsets(frameOffsets), _speed(speed)
	{
		if (_frameTexOffsets.empty())
			Debug::log("Created SpriteAnimator with no frame offsets", Debug::MessageType::PK_ERROR);
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

				if (_currentFrameIndex >= _frameTexOffsets.size())
				{
					if (_enableLooping)
					{
						play();
					}
					else
					{
						stop();
						_currentFrameIndex = _frameTexOffsets.size() - 1;
						return;
					}
				}
			}
			
			_changeFrameCooldown -= _speed * Timing::get_delta_time();
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
		return _frameTexOffsets[_currentFrameIndex];
	}

}