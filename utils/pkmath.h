#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>


namespace pk
{
	class vec2
	{
	public:
		float x = 0.0f;
		float y = 0.0f;

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

	class vec4
	{
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;

		vec4() {}
		vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

		float length() const
		{
			return std::sqrtf((x * x) + (y * y) + (z * z) + (w * w));
		}

		friend vec4 operator+(const vec4& left, const vec4& right)
		{
			return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
		}
		friend vec4 operator-(const vec4& left, const vec4& right)
		{
			return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
		}
		friend vec4 operator*(const vec4& left, const vec4& right)
		{
			return { left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w };
		}
		friend vec4 operator*(const vec4& left, float right)
		{
			return { left.x * right, left.y * right, left.z * right, left.w * right };
		}
		friend vec4 operator/(const vec4& left, float right)
		{
			return { left.x / right, left.y / right, left.z / right, left.w / right };
		}

		float dotp(const vec4& other) const
		{
			return ((x * other.x) + (y * other.y) + (z * other.z) + (w * other.w));
		}

		void normalize()
		{
			const float len = length();
			x /= len;
			y /= len;
			z /= len;
			w /= len;
		}
	};


	class mat4
	{
	private:

		float _e[16];

	public:


		mat4() { memset(_e, 0, sizeof(float) * 16); }
		mat4(const mat4& other) { memcpy(_e, other._e, sizeof(float) * 16); }
		mat4(mat4&& other) { memcpy(_e, other._e, sizeof(float) * 16); }
		void operator=(const mat4& other) { memcpy(_e, other._e, sizeof(float) * 16); }

		void setIdentity()
		{
			memset(_e, 0, sizeof(float) * 16);
			for (int i = 0; i < 4; ++i)
				_e[i + i * 4] = 1.0f;
		}

		friend mat4 operator*(const mat4& left, const mat4& right)
		{
			mat4 result;

			for (int y = 0; y < 4; ++y)
			{
				for (int x = 0; x < 4; ++x)
					result[y + x * 4] =
					left[y + 0 * 4] * right[0 + x * 4] +
					left[y + 1 * 4] * right[1 + x * 4] +
					left[y + 2 * 4] * right[2 + x * 4] +
					left[y + 3 * 4] * right[3 + x * 4];
			}

			return result;
		}

		friend vec4 operator*(const mat4& left, const vec4& right)
		{
			return { left[0 + 0 * 4] * right.x + left[0 + 1 * 4] * right.y + left[0 + 2 * 4] * right.z + left[0 + 3 * 4] * right.w,  // x
					 left[1 + 0 * 4] * right.x + left[1 + 1 * 4] * right.y + left[1 + 2 * 4] * right.z + left[1 + 3 * 4] * right.w,  // y
					 left[2 + 0 * 4] * right.x + left[2 + 1 * 4] * right.y + left[2 + 2 * 4] * right.z + left[2 + 3 * 4] * right.w,  // z
					 left[3 + 0 * 4] * right.x + left[3 + 1 * 4] * right.y + left[3 + 2 * 4] * right.z + left[3 + 3 * 4] * right.w };// w
		}

		//	*Found from: https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		//		comment on the site about this :
		//			"This was lifted from MESA implementation of the GLU library."
		void inverse()
		{
			mat4 inverseMatrix;

			inverseMatrix._e[0] = _e[5] * _e[10] * _e[15] -
				_e[5] * _e[11] * _e[14] -
				_e[9] * _e[6] * _e[15] +
				_e[9] * _e[7] * _e[14] +
				_e[13] * _e[6] * _e[11] -
				_e[13] * _e[7] * _e[10];

			inverseMatrix._e[4] = -_e[4] * _e[10] * _e[15] +
				_e[4] * _e[11] * _e[14] +
				_e[8] * _e[6] * _e[15] -
				_e[8] * _e[7] * _e[14] -
				_e[12] * _e[6] * _e[11] +
				_e[12] * _e[7] * _e[10];

			inverseMatrix._e[8] = _e[4] * _e[9] * _e[15] -
				_e[4] * _e[11] * _e[13] -
				_e[8] * _e[5] * _e[15] +
				_e[8] * _e[7] * _e[13] +
				_e[12] * _e[5] * _e[11] -
				_e[12] * _e[7] * _e[9];

			inverseMatrix._e[12] = -_e[4] * _e[9] * _e[14] +
				_e[4] * _e[10] * _e[13] +
				_e[8] * _e[5] * _e[14] -
				_e[8] * _e[6] * _e[13] -
				_e[12] * _e[5] * _e[10] +
				_e[12] * _e[6] * _e[9];

			inverseMatrix._e[1] = -_e[1] * _e[10] * _e[15] +
				_e[1] * _e[11] * _e[14] +
				_e[9] * _e[2] * _e[15] -
				_e[9] * _e[3] * _e[14] -
				_e[13] * _e[2] * _e[11] +
				_e[13] * _e[3] * _e[10];

			inverseMatrix._e[5] = _e[0] * _e[10] * _e[15] -
				_e[0] * _e[11] * _e[14] -
				_e[8] * _e[2] * _e[15] +
				_e[8] * _e[3] * _e[14] +
				_e[12] * _e[2] * _e[11] -
				_e[12] * _e[3] * _e[10];

			inverseMatrix._e[9] = -_e[0] * _e[9] * _e[15] +
				_e[0] * _e[11] * _e[13] +
				_e[8] * _e[1] * _e[15] -
				_e[8] * _e[3] * _e[13] -
				_e[12] * _e[1] * _e[11] +
				_e[12] * _e[3] * _e[9];

			inverseMatrix._e[13] = _e[0] * _e[9] * _e[14] -
				_e[0] * _e[10] * _e[13] -
				_e[8] * _e[1] * _e[14] +
				_e[8] * _e[2] * _e[13] +
				_e[12] * _e[1] * _e[10] -
				_e[12] * _e[2] * _e[9];

			inverseMatrix._e[2] = _e[1] * _e[6] * _e[15] -
				_e[1] * _e[7] * _e[14] -
				_e[5] * _e[2] * _e[15] +
				_e[5] * _e[3] * _e[14] +
				_e[13] * _e[2] * _e[7] -
				_e[13] * _e[3] * _e[6];

			inverseMatrix._e[6] = -_e[0] * _e[6] * _e[15] +
				_e[0] * _e[7] * _e[14] +
				_e[4] * _e[2] * _e[15] -
				_e[4] * _e[3] * _e[14] -
				_e[12] * _e[2] * _e[7] +
				_e[12] * _e[3] * _e[6];

			inverseMatrix._e[10] = _e[0] * _e[5] * _e[15] -
				_e[0] * _e[7] * _e[13] -
				_e[4] * _e[1] * _e[15] +
				_e[4] * _e[3] * _e[13] +
				_e[12] * _e[1] * _e[7] -
				_e[12] * _e[3] * _e[5];

			inverseMatrix._e[14] = -_e[0] * _e[5] * _e[14] +
				_e[0] * _e[6] * _e[13] +
				_e[4] * _e[1] * _e[14] -
				_e[4] * _e[2] * _e[13] -
				_e[12] * _e[1] * _e[6] +
				_e[12] * _e[2] * _e[5];

			inverseMatrix._e[3] = -_e[1] * _e[6] * _e[11] +
				_e[1] * _e[7] * _e[10] +
				_e[5] * _e[2] * _e[11] -
				_e[5] * _e[3] * _e[10] -
				_e[9] * _e[2] * _e[7] +
				_e[9] * _e[3] * _e[6];

			inverseMatrix._e[7] = _e[0] * _e[6] * _e[11] -
				_e[0] * _e[7] * _e[10] -
				_e[4] * _e[2] * _e[11] +
				_e[4] * _e[3] * _e[10] +
				_e[8] * _e[2] * _e[7] -
				_e[8] * _e[3] * _e[6];

			inverseMatrix._e[11] = -_e[0] * _e[5] * _e[11] +
				_e[0] * _e[7] * _e[9] +
				_e[4] * _e[1] * _e[11] -
				_e[4] * _e[3] * _e[9] -
				_e[8] * _e[1] * _e[7] +
				_e[8] * _e[3] * _e[5];

			inverseMatrix._e[15] = _e[0] * _e[5] * _e[10] -
				_e[0] * _e[6] * _e[9] -
				_e[4] * _e[1] * _e[10] +
				_e[4] * _e[2] * _e[9] +
				_e[8] * _e[1] * _e[6] -
				_e[8] * _e[2] * _e[5];


			float determinant = _e[0] * inverseMatrix._e[0] + _e[1] * inverseMatrix._e[4] + _e[2] * inverseMatrix._e[8] + _e[3] * inverseMatrix._e[12];

			*this = inverseMatrix; // THIS MAY NOT WORK! <- wtf is going on here!?!? don't remember at all..

			if (determinant == 0)
				return;

			for (int i = 0; i < 16; ++i)
				_e[i] *= (1.0f / determinant);
		}

		float operator[](int i) const { return _e[i]; }
		float& operator[](int i) { return _e[i]; }
		const float* getRawArray() const { return _e; }
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
	/*
	inline mat4 create_perspective_projection_matrix(
		float fov,
		float aspectRatio,
		float zNear,
		float zFar
	)
	{
		mat4 result;
		result.setIdentity();
		result[0 + 0 * 4] = 1.0f / (aspectRatio * std::tan(fov / 2.0f));
		result[1 + 1 * 4] = 1.0f / (std::tan(fov / 2.0f));
		result[2 + 2 * 4] = -((zFar + zNear) / (zFar - zNear));
		result[2 + 3 * 4] = -((2.0f * zFar * zNear) / (zFar - zNear));
		result[3 + 2 * 4] = -1.0f;
		result[3 + 3 * 4] = 0.0f;
		return result;
	}*/

	inline mat4 create_perspective_projection_matrix(
		float aspectRatio,
		float fov,
		float zNear, float zFar
	)
	{
		mat4 matrix;
		matrix.setIdentity();
		matrix[0] =			1.0f / (aspectRatio * tan(fov / 2.0f));
		matrix[1 + 1 * 4] = 1.0f / (tan(fov / 2.0f));
		matrix[2 + 2 * 4] = -((zFar + zNear) / (zFar - zNear));
		matrix[3 + 2 * 4] = -1.0f;
		matrix[2 + 3 * 4] = -((2.0f * zFar * zNear) / (zFar - zNear));
		matrix[3 + 3 * 4] = 0.0f;
		return matrix;
	}


	// MISC-----

	// Returns next closest power of 2 value from v
	/*inline unsigned int get_next_pow2(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}*/
}