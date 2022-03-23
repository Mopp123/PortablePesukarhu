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

		vec2() {}
		vec2(float x, float y) :  x(x), y(y) {}
		vec2(const vec2& other) : x(other.x), y(other.y) {}

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

		float dotp(const vec2& other) const
		{
			return ((x * other.x) + (y * other.y));
		}

		void normalize()
		{
			const float len = length();
			x /= len;
			y /= len;
		}
	};

	class vec3
	{
	public:
		float x = 0;
		float y = 0;
		float z = 0;

		vec3() {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		vec3(const vec3& other) : x(other.x), y(other.y), z(other.z) {}

		float length() const
		{
			return std::sqrtf((x * x) + (y * y) + (z * z));
		}

		friend vec3 operator+(const vec3& left, const vec3& right)
		{
			return { left.x + right.x, left.y + right.y, left.z + right.z };
		}
		friend vec3 operator-(const vec3& left, const vec3& right)
		{
			return { left.x - right.x, left.y - right.y, left.z - right.z };
		}
		friend vec3 operator*(const vec3& left, const vec3& right)
		{
			return { left.x * right.x, left.y * right.y, left.z * right.z };
		}
		friend vec3 operator*(const vec3& left, float right)
		{
			return { left.x * right, left.y * right, left.z * right };
		}
		friend vec3 operator/(const vec3& left, float right)
		{
			return { left.x / right, left.y / right, left.z / right };
		}

		float dotp(const vec3& other) const
		{
			return ((x * other.x) + (y * other.y) + (z * other.z));
		}

		void normalize()
		{
			const float len = length();
			x /= len;
			y /= len;
			z /= len;
		}
	};


	class mat4
	{
	private:

		float _e[16];

	public:


		mat4() { memset(_e, 0, sizeof(float) * 16); }
		mat4(const mat4& other) { memcpy(_e, other._e, sizeof(float) * 16); }

		void setIdentity()
		{
			memset(_e, 0, sizeof(float) * 16);
			for (int i = 0; i < 4; ++i)
				_e[i + i * 4] = 1.0f;
		}


		float operator[](int i) const { return _e[i]; }
		float& operator[](int i) { return _e[i]; }
	};

	inline mat4 create_proj_mat_ortho(float left, float right, float top, float bottom, float zNear, float zFar)
	{
		mat4 result;
		result.setIdentity();
		
		result[0] = 2.0f / (right - left);
		result[1 + 1 * 4] = 2.0f / (top - bottom);
		result[2 + 2 * 4] = -2.0f / (zFar - zNear);
		result[3 + 3 * 4] = 1.0f;
		result[0 + 3 * 4] = -((right + left) / (right - left));
		result[1 + 3 * 4] = -((top + bottom) / (top - bottom));
		result[2 + 3 * 4] = -((zFar + zNear) / (zFar - zNear));
		
		return result;
	}



	// MISC-----

	// Returns next closest power of 2 value from v
	inline unsigned int get_next_pow2(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}
}