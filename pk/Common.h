#pragma once

#include <climits>
#include <GL/glew.h>

//#define PK_BUILD_WINDOWS
//#define PK_BUILD_LINUX
#define PK_BUILD_WEB


// Datatypes ---

#ifdef PK_BUILD_WEB

#define PK_byte		char
#define PK_ubyte	unsigned char
#define PK_float	GLfloat
#define PK_int		GLint
#define PK_uint		GLuint
#define PK_short	GLshort
#define PK_ushort	GLushort

// Limits ---

// Maximum total mem our application can use (in bytes)
#define PK_LIMITS_MAX_TOTAL_MEM 16777216
#define PK_LIMITS_DRAWCALL_MAX_VERTEX_DATA_LEN (USHRT_MAX)



enum PK_ShaderDatatype
{
	PK_FLOAT = GL_FLOAT
};

#endif




// Some funny ass macros --------------------------------------------
// (to track all sorts of illegal and naughty behaviours xDdd)

// <#M_DANGER>

// This is ment ONLY when committing suicide... xDd (reason for this macro -> easily track fuckups caused by obj suicides..)
#define PK_COMMIT_SUICIDE(obj) delete obj