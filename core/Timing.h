#pragma once

#include <chrono>

namespace pk
{
	class Timing
	{
	private:

		std::chrono::time_point<std::chrono::steady_clock> _time_startFrame;
		static float s_deltaTime;


	public:

		Timing();

		void update();
		
		static float get_delta_time();
	};
}