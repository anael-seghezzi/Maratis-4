/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MVector3.h
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


#ifndef _M_VECTOR3_H
#define _M_VECTOR3_H


class M_CORE_EXPORT MVector3
{
public:

	float x;
	float y;
	float z;

public:

	MVector3(void):	
		x(0.0f), 
		y(0.0f), 
		z(0.0f)
	{}

	MVector3(float newX, float newY, float newZ):	
		x(newX), 
		y(newY), 
		z(newZ)
	{}

	MVector3(const float value):	
		x(value), 
		y(value), 
		z(value)
	{}

	MVector3(const float * values):	
		x(*(values)), 
		y(*(values+1)), 
		z(*(values+2))
	{}

	MVector3(const MVector3 & vec):	
		x(vec.x), 
		y(vec.y), 
		z(vec.z)
	{}

	MVector3(const MVector4 & vec);
	
	MVector3(const MVector2 & vec):	
		x(vec.x), 
		y(vec.y), 
		z(0)
	{}

public:

	inline MVector3 operator + (const MVector3 & vec) const
	{	
		return MVector3(x + vec.x, y + vec.y, z + vec.z);
	}
	
	inline MVector3 operator - (const MVector3 & vec) const
	{	
		return MVector3(x - vec.x, y - vec.y, z - vec.z);
	}

	inline MVector3 operator + (const float & value) const
	{	
		return MVector3(x + value, y + value, z + value);
	}

	inline MVector3 operator - (const float & value) const
	{	
		return MVector3(x - value, y - value, z - value);
	}

	inline MVector3 operator * (const float value) const
	{	
		return MVector3(x * value, y * value, z * value);
	}

	inline MVector3 operator * (const MVector3 & vec) const
	{	
		return MVector3(x * vec.x, y * vec.y, z * vec.z);
	}

	inline MVector3 operator / (const MVector3 & vec) const
	{	
		return MVector3(x / vec.x, y / vec.y, z / vec.z);
	}
	
	inline MVector3 operator / (const float value) const
	{	
		if(value == 0) return MVector3(0.0f, 0.0f, 0.0f);
		float i = 1.0f / value;
		return MVector3(x*i, y*i, z*i);
	}

	friend inline MVector3 operator * (float factor, const MVector3 & vec)
	{
		return vec * factor;
	}
	
	inline bool operator == (const MVector3 & vec) const
	{
		if(x == vec.x && y == vec.y && z == vec.z)
			return true;

		return false;
	}

	inline bool operator != (const MVector3 & vec) const
	{	
		return !((*this) == vec);
	}

	inline void operator += (const MVector3 & vec)
	{	
		x += vec.x;
		y += vec.y;	
		z += vec.z;
	}

	inline void operator -= (const MVector3 & vec)
	{	
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
	}

	inline void operator *= (const float value)
	{	
		x *= value;	
		y *= value;	
		z *= value;	
	}

	inline void operator = (const float value)
	{	
		x = value;	
		y = value;	
		z = value;	
	}
	
	inline void operator /= (const float value)
	{	
		if(value == 0.0f)
			return;

		float i = 1.0f / value;
		x *= i; 
		y *= i; 
		z *= i;	
	}

	inline MVector3 operator - (void) const
	{
		return MVector3(-x, -y, -z);
	}

	inline MVector3 operator + (void) const 
	{
		return *this;
	}

	inline operator float* () const 
	{
		return (float*) this;
	}

	inline operator const float* () const 
	{
		return (const float*) this;
	}

public:

	inline void set(const float newX, const float newY, const float newZ)
	{	
		x = newX;	
		y = newY;	
		z = newZ;
	}

	inline void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	
	inline MVector3 crossProduct(const MVector3 & vec) const
	{	
		return MVector3((y*vec.z) - (z*vec.y), (z*vec.x) - (x*vec.z), (x*vec.y) - (y*vec.x));
	}

	inline float dotProduct(const MVector3 & vec) const
	{	
		return (x*vec.x) + (y*vec.y) + (z*vec.z);
	}
	
	void normalize();

	MVector3 getNormalized() const;

	inline float getLength() const
	{	
		return sqrtf((x*x) + (y*y) + (z*z));
	}
	
	inline float getSquaredLength() const
	{	
		return (x*x) + (y*y) + (z*z);
	}

	void rotateX(double angle);
	void rotateY(double angle);
	void rotateZ(double angle);
	void rotateAxis(double angle, const MVector3 & axis);

	MVector3 getRotatedX(double angle) const;
	MVector3 getRotatedY(double angle) const;
	MVector3 getRotatedZ(double angle) const;
	MVector3 getRotatedAxis(double angle, const MVector3 & axis) const;

	inline MVector3 lerp(const MVector3 & vec, float factor) const
	{	
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};

#endif