#pragma once

#include "gl_conf.hpp"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <type_traits>
// using namespace std;

typedef int8_t   gl_int8;
typedef int16_t  gl_int16;
typedef int32_t  gl_int32;
typedef uint8_t  gl_uint8;
typedef uint16_t gl_uint16;
typedef uint32_t gl_uint32;
typedef float    gl_float;
typedef double   gl_double;

typedef gl_uint8  gl_mask8;   // 0xff
typedef gl_uint16 gl_mask12;  // 0xfff
typedef gl_uint16 gl_mask16;  // 0xffff
typedef gl_uint32 gl_mask24;  // 0xffffff
typedef gl_uint32 gl_mask32;  // 0xffffffff

#define GL_MASK8  (gl_uint8)0XFF
#define GL_MASK12 (gl_uint16)0X7FFF
#define GL_MASK16 (gl_uint16)0XFFFF
#define GL_MASK24 (gl_uint32)0X00FFFFFF
#define GL_MASK32 (gl_uint32)0XFFFFFFFF

typedef gl_int16  gl_int;
typedef gl_uint16 gl_uint;
typedef gl_int32  gl_long;
typedef gl_uint32 gl_ulong;

#ifndef PI
#define PI 3.1415926f
#endif

#if CONFIG_USE_DOUBLE_PRECISION_FLOATS

typedef gl_double gl_real;
#define gl_round round
#define gl_cos   cos
#define gl_sin   sin
#define gl_atan2 atan2
#define gl_sqrt  sqrt

#else

typedef gl_float gl_real;
#define gl_round roundf
#define gl_cos   cosf
#define gl_sin   sinf
#define gl_atan2 atan2f
#define gl_sqrt  sqrtf

#endif

#define ARRSZIE(arr) (sizeof(arr) / sizeof(arr[0]))

//
template <typename T>
inline T gl_min(T a, T b) { return a < b ? a : b; }
template <typename T>
inline T gl_max(T a, T b) { return a > b ? a : b; }
template <typename T>
inline T gl_abs(T n) { return n > 0 ? n : -n; }
template <typename T>
inline void gl_swap(T* a, T* b)
{
    T t = *a;
    *a  = *b;
    *b  = t;
}

//
enum gl_align {
    ALIGN_NONE = 0,
    // horizontal
    ALIGN_INNER_LEFT  = 1,
    ALIGN_INNER_RIGHT = 2,
    ALIGN_OUTER_LEFT  = 3,
    ALIGN_OUTER_RIGHT = 4,
    ALIGN_HCENTER     = 5,
    // vertical
    ALIGN_INNER_TOP    = 1,
    ALIGN_INNER_BOTTOM = 2,
    ALIGN_OUTER_TOP    = 3,
    ALIGN_OUTER_BOTTOM = 4,
    ALIGN_VCENTER      = 5,
};

//
struct gl_image {
    gl_uint     width;
    gl_uint     height;
    const void* buffer;
};

enum gl_direction {
    Direction_Clockwise        = 0,  // 瞬时针
    Direction_CounterClockwise = 1,  // 逆时针
};

enum gl_selection {
    Selection_None        = 0,
    Selection_LeftTop     = 1 << 1,
    Selection_RightTop    = 1 << 2,
    Selection_RightBottom = 1 << 3,
    Selection_LeftBottom  = 1 << 4,
    Selection_Top         = Selection_LeftTop | Selection_RightTop,
    Selection_Bottom      = Selection_LeftBottom | Selection_RightBottom,
    Selection_Left        = Selection_LeftTop | Selection_LeftBottom,
    Selection_Right       = Selection_RightTop | Selection_RightBottom,
    Selection_ALL         = Selection_Left | Selection_Right,
};