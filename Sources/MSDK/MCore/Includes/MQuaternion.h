/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MQuaternion.h
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


#ifndef _M_QUATERNION_H
#define _M_QUATERNION_H


class M_CORE_EXPORT MQuaternion
{
public:

	float values[4];

public:

	MQuaternion(void)
	{
		loadIdentity();
	}

	MQuaternion(float val0, float val1, float val2, float val3)
	{
		values[0] = val0;
		values[1] = val1;
		values[2] = val2;
		values[3] = val3;
	}

	MQuaternion(float angle, const MVector3 & axis)
	{
		setFromAngleAxis(angle, axis);
	}

	MQuaternion(float xAngle, float yAngle, float zAngle)
	{
		setFromAngles(xAngle, yAngle, zAngle);
	}

	MQuaternion(const MQuaternion & q1, MQuaternion q2, float interpolation)
	{
		slerp(q1, q2, interpolation);
	}

public:

	MQuaternion operator * (const MQuaternion & quat) const;

	void operator *= (const MQuaternion & quat);

	MQuaternion operator - (void) const
	{
		return MQuaternion(-values[0], -values[1], -values[2], -values[3]);
	}

	inline bool operator == (const MQuaternion & quat) const
	{
		if(values[0] == quat.values[0] && 
		   values[1] == quat.values[1] &&
		   values[2] == quat.values[2] &&
		   values[3] == quat.values[3])
			return true;

		return false;
	}

	inline bool operator != (const MQuaternion & quat) const
	{	
		return !((*this) == quat);
	}

public:

	void normalize(void);
	void loadIdentity(void);
	void setFromAngles(float xAngle, float yAngle, float zAngle);
	void slerp(const MQuaternion & q1, const MQuaternion & q2, float interpolation);
	void invert(void);
	void setFromAngleAxis(float angle, const MVector3 & axis);
	void setFromVectors(const MVector3 & source, const MVector3 & destination);

	MVector3 getEulerAngles(void) const;

	float getAngle(void) const;
	MVector3 getAxis(void) const;
};

#endif