/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaths.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef _M_MATHS_H
#define _M_MATHS_H

#include <math.h>

class MVector2;
class MVector3;
class MVector4;
class MColor;
class MMatrix4x4;
class MQuaternion;


#ifndef MIN
#define MIN(a, b) M_MIN(a, b)
#endif
#ifndef MAX
#define MAX(a, b) M_MAX(a, b)
#endif
#ifndef CLAMP
#define CLAMP(x, low, high) M_CLAMP(x, low, high)
#endif
#ifndef ABS
#define ABS(a) M_ABS(a)
#endif

// degree/radian conversions
#define DEG_TO_RAD M_DEG_TO_RAD
#define RAD_TO_DEG M_RAD_TO_DEG

// power of two
#define isPowerOfTwo(x)	M_ISPOWEROFTWO(x)
inline unsigned int getNextPowerOfTwo(unsigned int x) {return m_next_power_of_two(x); }

// loop float value
M_CORE_EXPORT float loopFloat(float val, float min, float max);

// range
struct M_CORE_EXPORT MRange
{
	int start;
	int end;
};

// interpolations
inline float cubicInterpolation(float y0, float y1, float y2, float y3, float mu) { m_interpolation_cubic(y0, y1, y2, y3, mu); }
inline float CatmullRomInterpolation(float y0, float y1, float y2, float y3, float mu) { m_interpolation_catmullrom(y0, y1, y2, y3, mu); }
M_CORE_EXPORT float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin = false, bool end = false);

// sort
M_CORE_EXPORT void sortFloatList(float floatList[], int start, int end);
M_CORE_EXPORT void sortFloatIndexList(int indexList[], float floatList[], int start, int end);

// 3d utils
M_CORE_EXPORT void simplifyDP(float tol, MVector3 * v, int j, int k, int * mk);
M_CORE_EXPORT MVector3 getTriangleNormal(const MVector3 & A, const MVector3 & B, const MVector3 & C);

// 3d collision
M_CORE_EXPORT bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max);

inline bool boxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB)
{
	return m_3d_box_to_box_collision((float3 *)&minA, (float3 *)&maxA, (float3 *)&minB, (float3 *)&maxB) == 1;
}

inline bool isTriangleInBox(const MVector3 & A, const MVector3 & B, const MVector3 & C, const MVector3 & boxCenter, const MVector3 & boxHalfSize)
{
	return m_3d_tri_box_overlap((float3 *)&boxCenter, (float3 *)&boxHalfSize, (float3 *)&A, (float3 *)&B, (float3 *)&C) == 1;
}

// 3d raytracing
inline float rayBoxIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & min, const MVector3 & max)
{
	return m_3d_ray_box_intersection((float3 *)&origin, (float3 *)&direction, (float3 *)&min, (float3 *)&max);
}

inline float rayTriangleIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & A, const MVector3 & B, const MVector3 & C, float & u, float & v)
{
	return m_3d_ray_triangle_intersection((float3 *)&origin, (float3 *)&direction, (float3 *)&A, (float3 *)&B, (float3 *)&C, &u, &v);
}

inline float rayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal)
{
	return m_3d_ray_plane_intersection((float3 *)&origin, (float3 *)&direction, (float3 *)&planePoint, (float3 *)&planeNormal);
}

// classes
#include "MVector2.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MColor.h"
#include "MMatrix4x4.h"
#include "MQuaternion.h"

// color convertions
inline MVector3 RGBToHSV(const MVector3 &rgbColor) { MVector3 res; m_color_RGB_to_HSV(rgbColor, res); return res; }
inline MVector3 HSVToRGB(const MVector3 &HSVColor) { MVector3 res; m_color_HSV_to_RGB(HSVColor, res); return res; }
inline MVector3 RGBToHSL(const MVector3 &rgbColor) { MVector3 res; m_color_RGB_to_HSL(rgbColor, res); return res; }
inline MVector3 HSLToRGB(const MVector3 &hslColor) { MVector3 res; m_color_HSL_to_RGB(hslColor, res); return res; }

#endif
