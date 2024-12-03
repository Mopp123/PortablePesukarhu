#pragma once

#include <climits>
// TODO: After "primitive datatype overhaul" completed, this is no more needed here!!
// -> delete it
// #include <GL/glew.h>

// NOTE: These should be defined by CMakeLists
//#define PK_BUILD_WINDOWS
//#define PK_BUILD_LINUX
//#define PK_BUILD_WEB

#define PK_PLATFORM_ID_NONE 0
#define PK_PLATFORM_ID_LINUX 1
#define PK_PLATFORM_ID_WINDOWS 2
#define PK_PLATFORM_ID_WEB 3

// Datatypes ---
#define PK_byte	char
#define PK_ubyte	unsigned char

// Limits ---
// Maximum total mem our application can use (in bytes)
#define PK_LIMITS_MAX_TOTAL_MEM 16777216
#define PK_LIMITS_DRAWCALL_MAX_VERTEX_DATA_LEN (USHRT_MAX)




// Some funny ass macros --------------------------------------------
// (to track all sorts of illegal and naughty behaviours xDdd)

// <#M_DANGER>

// This is ment ONLY when committing suicide... xDd (reason for this macro -> easily track fuckups caused by obj suicides..)
#define PK_COMMIT_SUICIDE(obj) delete obj
