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

    void Animation::setFrames(const std::vector<int>& frames)
    {
        _frames.clear();
        for (int i = 0; i < frames.size(); ++i)
            _frames.push_back(frames[i]);
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

    void Animation::reset()
    {
        _changeFrameCooldown = 0.0f;
        _currentFrameIndex = 0;
        _currentFrame = _frames[0];
    }

    void Animation::copyFrom(const Animation& other)
    {
        _currentFrameIndex = other._currentFrameIndex;
        _changeFrameCooldown = other._changeFrameCooldown;
        _maxChangeFrameCooldown = other._maxChangeFrameCooldown;
        _anchorOffset = other._anchorOffset;
        _frames = other._frames;
        _currentFrame = other._currentFrame;
        _speed = other._speed;
        _isPlaying = other._isPlaying;
        _enableLooping = other._enableLooping;
    }

    const int Animation::getCurrentFrame() const
    {
        return _currentFrame;
    }

}
