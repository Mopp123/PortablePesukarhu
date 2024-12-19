#pragma once

#include <chrono>

namespace pk
{
	class Timing
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> _startFrameTime;
		static float s_deltaTime;

	public:
		Timing();
		void update();
		static float get_delta_time();
	};
}
