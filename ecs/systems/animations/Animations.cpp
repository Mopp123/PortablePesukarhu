#include "Animations.h"
#include "../../../core/Debug.h"
#include "../../../core/Timing.h"


namespace pk
{

	Animation::Animation(std::vector<int> frames, float speed):
		_frames(frames), _speed(speed)
	{}

	Animation::Animation(int frameCount, float speed):
		_speed(speed)
	{
		for (int i = 0; i < frameCount; ++i)
			_frames.push_back(i);
	}

	Animation::~Animation()
	{}

	void Animation::update()
	{
		if(_isPlaying)
		{
			if (_changeFrameCooldown <= 0.0f)
			{
				_changeFrameCooldown = _maxChangeFrameCooldown;
				_currentFrameIndex++;
				if (_currentFrameIndex > _frames.size() - 1)
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
			if (_currentFrameIndex < _frames.size())
				_currentFrame = _frames[_currentFrameIndex];
		}
	}

	void Animation::play()
	{
		_isPlaying = true;
		_currentFrameIndex = 0;
	}

	void Animation::stop()
	{
		_isPlaying = false;
	}

	const int Animation::getCurrentFrame() const
	{
		return _currentFrame;
	}

}
