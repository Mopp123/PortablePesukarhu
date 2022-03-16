#pragma once

#include <cmath>
#include <cstring>

namespace pk
{
	class vec2
	{
	public:
		float x = 0;
		float y = 0;

		vec2(float x, float y) :  x(x), y(y) {}

		float length() const
		{
			return std::sqrtf((x * x) + (y * y));
		}

		friend vec2 operator+(const vec2& left, const vec2& right)
		{
			return { left.x + right.x, left.y + right.y };
		}
		friend vec2 operator-(const vec2& left, const vec2& right)
		{
			return { left.x - right.x, left.y - right.y };
		}
		friend vec2 operator*(const vec2& left, const vec2& right)
		{
			return { left.x * right.x, left.y * right.y };
		}
		friend vec2 operator*(const vec2& left, float right)
		{
			return { left.x * right, left.y * right };
		}
		friend vec2 operator/(const vec2& left, float right)
		{
			return { left.x / right, left.y / right };
		}
	};


	class mat4
	{
	private:

		float _e[16];

	public:

		mat4() { memset(_e, 0, sizeof(float) * 16); }

		float operator[](int i) const { return _e[i]; }
		float& operator[](int i) { return _e[i]; }


	};
}