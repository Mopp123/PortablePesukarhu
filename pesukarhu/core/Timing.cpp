#include "Timing.h"


namespace pk
{
	float Timing::s_deltaTime = 0.0f;

	Timing::Timing()
	{
	}

	void Timing::update()
	{
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - _startFrameTime;
		s_deltaTime = duration.count();
		_startFrameTime = std::chrono::high_resolution_clock::now();
	}

	float Timing::get_delta_time()
	{
		return s_deltaTime;
	}
}
