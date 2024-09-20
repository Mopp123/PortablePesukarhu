#pragma once

#include <cstring>
#include <string>


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
        std::string toString() const;

        float length() const;
        float dotp(const vec2& other) const;
        vec2 normalize() const;

        friend vec2 operator+(const vec2& left, const vec2& right);
        friend vec2 operator-(const vec2& left, const vec2& right);
        friend vec2 operator*(const vec2& left, const vec2& right);
        friend vec2 operator*(const vec2& left, float right);
        friend vec2 operator/(const vec2& left, float right);
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
        std::string toString() const;

        float length() const;
        float dotp(const vec3& other) const;
        vec3 normalize() const;
        vec3 lerp(const vec3& other, float amount) const;

        friend vec3 operator+(const vec3& left, const vec3& right);
        friend vec3 operator-(const vec3& left, const vec3& right);
        friend vec3 operator*(const vec3& left, const vec3& right);
        friend vec3 operator*(const vec3& left, float right);
        friend vec3 operator/(const vec3& left, float right);
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
        vec4(vec3 v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
        vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        std::string toString() const;

        float length() const;
        float dotp(const vec4& other) const;
        vec4 normalize() const;

        inline vec3 toVec3() const { return { x, y, z }; }

        friend vec4 operator+(const vec4& left, const vec4& right);
        friend vec4 operator-(const vec4& left, const vec4& right);
        friend vec4 operator*(const vec4& left, const vec4& right);
        friend vec4 operator*(const vec4& left, float right);
        friend vec4 operator/(const vec4& left, float right);
    };


    class mat4
    {
    private:
        float _e[16];

    public:
        mat4(float diag = 0.0f);
        mat4(const mat4& other) { memcpy(_e, other._e, sizeof(float) * 16); }
        mat4(mat4&& other) { memcpy(_e, other._e, sizeof(float) * 16); }

        std::string toString() const;

        void operator=(const mat4& other);
        bool operator==(const mat4& other) const;
        bool operator!=(const mat4& other) const;

        void setIdentity();

        mat4 transpose() const;

        //	*Found from: https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
        //		comment on the site about this :
        //			"This was lifted from MESA implementation of the GLU library."
        void inverse();

        friend mat4 operator*(const mat4& left, const mat4& right);
        friend vec4 operator*(const mat4& left, const vec4& right);

        inline float operator[](int i) const { return _e[i]; }
        inline float& operator[](int i) { return _e[i]; }
        inline const float* getRawArray() const { return _e; }
    };


    class quat : public vec4
	  {
    public:
	      quat(const vec3& axis, float angle);

	      quat() :
	          vec4(0,0,0,1)
	      {}

	      quat(float x, float y, float z, float w) :
	          vec4(x, y, z, w)
	      {}

	      quat(const vec4& other) :
	      	vec4(other.x, other.y, other.z, other.w)
	      {}

        std::string toString() const;

        // NOTE: why no firends? :'D
	      quat operator*(const quat& other) const;
	      quat operator*(const vec3& other) const;
	      bool operator==(const quat& other) const;

        quat conjugate() const;
        mat4 toRotationMatrix() const;

        quat slerp(const quat& other, float amount) const;
	  };


    mat4 create_proj_mat_ortho(
        float left,
        float right,
        float top,
        float bottom,
        float zNear,
        float zFar
    );

    mat4 create_perspective_projection_matrix(
        float aspectRatio,
        float fov,
        float zNear, float zFar
    );

    mat4 create_rotation_matrix(float pitch, float yaw, float roll);

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
