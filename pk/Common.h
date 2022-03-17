#pragma once

#include <climits>
#include <GL/glew.h>

//#define PK_BUILD_WINDOWS
//#define PK_BUILD_LINUX
#define PK_BUILD_WEB


// Datatypes ---

#ifdef PK_BUILD_WEB

#define PK_float	GLfloat
#define PK_int		GLint
#define PK_uint		GLuint
#define PK_short	GLshort
#define PK_ushort	GLushort

// Limits ---

#define PK_LIMITS_DRAWCALL_MAX_VERTEX_COUNT (USHRT_MAX)


enum PK_ShaderDatatype
{
	PK_FLOAT = GL_FLOAT
};

#endif