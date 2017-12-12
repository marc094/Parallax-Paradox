#ifndef __P2DEFS_H__
#define __P2DEFS_H__

#include <stdio.h>
#include <functional>

//  NULL just in case ----------------------

#ifdef NULL
#undef NULL
#endif
#define NULL  0

// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
    }

#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define TO_BOOL( a )  ( (a != 0) ? true : false )
#define CLAMP( value, min, max ) ( MAX( MIN( value, max ), min ) )
#define INTERPOLATE_TO( value, dest, step ) ( (value > dest) ? (value - dest < step) ? dest : value - step : (value < dest) ? (dest - value < step) ? dest : value + step : dest ) //Don't even ask about this
#define MAX_DISTANCE_ASTAR 20*20
#define MAX_DISTANCE_COLLIDER_CULLING 64*64

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned char uchar;

//typedef void(*Callback_v)(void);
typedef std::function<void(void)> Callback_v;
typedef void(*Callback_c)(const char*);

template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}

// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192


// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

// Interpolates between two values at a cerain rate (step)
template<class TYPE>
TYPE Interpolate(TYPE val, TYPE target, TYPE step)
{
	if (val > target)
		if (val - target < step)
			val = target;
		else val = val - step;
	else if (val < target)
		if (target - val < step)
			val = target;
		else val = val + step;
	else val = target;

	return val;
}
#endif