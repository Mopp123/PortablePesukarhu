#include "Timing.h"


namespace pk
{
	float Timing::s_deltaTime = 0.0f;

	Timing::Timing()
	{
	}

	void Timing::update()
	{
		using namespace std::chrono_literals;

		s_deltaTime = (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - _time_startFrame)).count();
		_time_startFrame = std::chrono::high_resolution_clock::now();
	}

	float Timing::get_delta_time()
	{
		return s_deltaTime;
	}
}
