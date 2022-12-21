#pragma once

#include "../System.h"
#include "../../../utils/pkmath.h"
#include <vector>


namespace pk
{
    class Animation : public Updateable
    {
    private:
        int _currentFrameIndex = 0;

        float _changeFrameCooldown = 0.0f;
        float _maxChangeFrameCooldown = 1.0f;

        vec2 _anchorOffset = {0, 0};
        std::vector<int> _frames;
        int _currentFrame = 0;

        float _speed = 0.0f;

        bool _isPlaying = false;
        bool _enableLooping = false;

    public:
        Animation(std::vector<int> frames, float speed);
        Animation(int frameCount, float speed);
        ~Animation();

        virtual void update();

        //void getRaw(const std::vector<int>& frames)

        void play();
        void stop();
        void reset();
        // Copies another Animation's data into this
        void copyFrom(const Animation& other);
        void copyStateFrom(const Animation& other);

        void setFrames(const std::vector<int>& frames);
        const int getCurrentFrame() const;
        const int getCurrentFrameIndex() const { return _currentFrameIndex; }
        inline std::vector<int>& getFrames() { return _frames; }

        inline void enableLooping(bool arg) { _enableLooping = arg; }
        inline bool isPlaying() const { return _isPlaying; }
    };
}
