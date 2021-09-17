/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MVector4.h
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


#ifndef _M_VECTOR4_H
#define _M_VECTOR4_H


class M_CORE_EXPORT MVector4
{
public:

	float x;
	float y;
	float z;
	float w;

public:

	MVector4(void):
		x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{}

	MVector4(float newX, float newY, float newZ, float newW):
		x(newX),
		y(newY),
		z(newZ),
		w(newW)
	{}

	MVector4(const float value):
		x(value),
		y(value),
		z(value),
		w(value)
	{}
	
	MVector4(const float * values):
		x(*(values)),
		y(*(values + 1)),
		z(*(values + 2)),
		w(*(values + 3))
	{}

	MVector4(const MVector4 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(vec.w)
	{}

	MVector4(const MVector3 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(1.0f)
	{}

	MVector4(const MColor & color);

public:

	inline MVector4 operator + (const MVector4 & vec) const
	{
		return MVector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	inline MVector4 operator - (const MVector4 & vec) const
	{
		return MVector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	inline MVector4 operator * (const MVector4 & vec) const
	{
		return MVector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
	}
	
	inline MVector4 operator * (const float value) const
	{
		return MVector4(x * value, y * value, z * value, w * value);
	}

	inline MVector4 operator / (const float value) const
	{
		if(value == 0.0f) return MVector4(0.0f, 0.0f, 0.0f, 0.0f);
		return MVector4(x / value, y / value, z / value, w / value);
	}

	inline friend MVector4 operator * (float factor, const MVector4 & vec)
	{
		return vec * factor;
	}

	inline bool operator == (const MVector4 & vec) const
	{
		if(x == vec.x && y == vec.y && z == vec.z && w == vec.w){
			return true;
		}

		return false;
	}

	inline bool operator != (const MVector4 & vec) const
	{
		return !((*this) == vec);
	}

	inline void operator += (const MVector4 & vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
	}

	inline void operator -= (const MVector4 & vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
	}

	inline void operator *= (const MVector4 & vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;
	}
	
	inline void operator *= (const float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
	}

	inline void operator /= (const float value)
	{
		if(value == 0.0f){
			return;
		}
		else
		{
			float div = 1.0f/value;
			x *= div;
			y *= div;
			z *= div;
			w *= div;
		}
	}

	inline MVector4 operator - (void) const
	{
		return MVector4(-x, -y, -z, -w);
	}

	inline MVector4 operator + (void) const
	{
		return (*this);
	}

	inline operator float* () const
	{
		return (float*) this;
	}

	inline operator const float* () const
	{
		return (const float*) this;
	}

	operator MVector3();

public:

	inline void toSRBG(void)
	{
		m_linear_to_sRGB(&x, &x, 3);
	}

	inline void toLinear(void)
	{
		m_sRGB_to_linear(&x, &x, 3);
	}

	inline MVector4 getSRBG(void)
	{
		MVector4 copy(*this);
		copy.toSRBG();
		return copy;
	}

	inline MVector4 getLinear(void)
	{
		MVector4 copy(*this);
		copy.toLinear();
		return copy;
	}

	inline void set(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

	inline void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	inline float dotProduct(const MVector4 & vec)
	{
		return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
	}

	inline MVector4 lerp(const MVector4 & vec, float factor) const
	{
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}

	inline float getSquaredLength() const
	{	
		return (x*x) + (y*y) + (z*z) + (w*w);
	}

	inline void clamp(float min, float max)
	{	
		x = M_CLAMP(x, min, max);
		y = M_CLAMP(y, min, max);
		z = M_CLAMP(z, min, max);
		w = M_CLAMP(w, min, max);
	}
};

#endif
