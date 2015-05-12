/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MQuaternion.cpp
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


#include "../Includes/MCore.h"


void MQuaternion::normalize(void)
{
	float distance = sqrtf(
		values[0] * values[0] + 
		values[1] * values[1] + 
		values[2] * values[2] + 
		values[3] * values[3]
	);

	if(distance > 0.00000001f)
	{
		float invDist = 1.0f / distance;
		values[0] *= invDist;
		values[1] *= invDist;
		values[2] *= invDist;
		values[3] *= invDist;
	}
	else
	{
		loadIdentity();
	}
}

void MQuaternion::loadIdentity(void)
{
	values[0] = 0.0f;
	values[1] = 0.0f;
	values[2] = 0.0f;
	values[3] = 0.9999999f;
}

MQuaternion MQuaternion::operator * (const MQuaternion & quat) const
{
	return MQuaternion(
		(values[3] * quat.values[0]) + (values[0] * quat.values[3]) + (values[1] * quat.values[2]) - (values[2] * quat.values[1]),
		(values[3] * quat.values[1]) + (values[1] * quat.values[3]) + (values[2] * quat.values[0]) - (values[0] * quat.values[2]),
		(values[3] * quat.values[2]) + (values[2] * quat.values[3]) + (values[0] * quat.values[1]) - (values[1] * quat.values[0]),
		(values[3] * quat.values[3]) - (values[0] * quat.values[0]) - (values[1] * quat.values[1]) - (values[2] * quat.values[2])
	);
}

void MQuaternion::operator *= (const MQuaternion & quat)
{
	(*this) = (*this) * quat;
}

void MQuaternion::invert()
{
	values[0] = -values[0];
	values[1] = -values[1];
	values[2] = -values[2];
	values[3] =  values[3];
}

void MQuaternion::setFromAngles(float xAngle, float yAngle, float zAngle)
{
	float ti = (float)(xAngle * DEG_TO_RAD)*0.5f;
	float tj = (float)(yAngle * DEG_TO_RAD)*0.5f;
	float th = (float)(zAngle * DEG_TO_RAD)*0.5f;

    float ci = cosf(ti);
	float cj = cosf(tj);
	float ch = cosf(th);
    float si = sinf(ti);
	float sj = sinf(tj);
	float sh = sinf(th);
    float cc = ci*ch;
	float cs = ci*sh;
	float sc = si*ch;
	float ss = si*sh;
	
	values[0] = cj*sc - sj*cs;
	values[1] = cj*ss + sj*cc;
	values[2] = cj*cs - sj*sc;
	values[3] = cj*cc + sj*ss;
}

void MQuaternion::slerp(const MQuaternion & q1, const MQuaternion & q2, float interpolation)
{
	MQuaternion q3;

	float dot = 
	q1.values[0] * q2.values[0] + 
	q1.values[1] * q2.values[1] + 
	q1.values[2] * q2.values[2] + 
	q1.values[3] * q2.values[3];

	if(dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	}
	else{
		q3 = q2;
	}

	if(dot < 0.95f)
	{
		float angle = acosf(dot);
		float factor1 = sinf(angle*(1-interpolation));
		float factor2 = sinf(angle*interpolation);
		float factor3 = sinf(angle);

		values[0] = (q1.values[0] * factor1 + q3.values[0] * factor2) / factor3;
		values[1] = (q1.values[1] * factor1 + q3.values[1] * factor2) / factor3;
		values[2] = (q1.values[2] * factor1 + q3.values[2] * factor2) / factor3;
		values[3] = (q1.values[3] * factor1 + q3.values[3] * factor2) / factor3;
	}
	else // use linear interpolation if angle is small	
	{
		float inv = 1-interpolation;

		values[0] = (q1.values[0] * inv + q3.values[0] * interpolation);
		values[1] = (q1.values[1] * inv + q3.values[1] * interpolation);
		values[2] = (q1.values[2] * inv + q3.values[2] * interpolation);
		values[3] = (q1.values[3] * inv + q3.values[3] * interpolation);

		normalize();		
	}
}

void MQuaternion::setFromAngleAxis(float angle, const MVector3 & axis)
{
	const MVector3 normAxis = axis.getNormalized();
	
	float sinHalfAngle = (float)sin((angle * DEG_TO_RAD) / 2.0);
	float cosHalfAngle = (float)cos((angle * DEG_TO_RAD) / 2.0);

	values[0] = sinHalfAngle * normAxis.x;
	values[1] = sinHalfAngle * normAxis.y;
	values[2] = sinHalfAngle * normAxis.z;
	values[3] = cosHalfAngle;

	normalize();
}

void MQuaternion::setFromVectors(const MVector3 & source, const MVector3 & destination)
{
	MVector3 axis=source.crossProduct(destination);
	
	float angle = acosf(source.getNormalized().dotProduct(destination.getNormalized()));

	setFromAngleAxis((float)(angle*RAD_TO_DEG), axis);
}

float MQuaternion::getAngle(void) const
{
	float v = values[3];

	if(1.0f < v){ 
		v = 1.0f;
	}
	else if(-1.0f > v){
		v = -1.0f;
	}

	return (float)((2.0f * acosf(v)) * RAD_TO_DEG);
}
	
MVector3 MQuaternion::getAxis(void) const
{
	float v = values[3];

	if(1.0f < v){
		v = 1.0f;
	}
	else if(-1.0f > v){ 
		v = -1.0f;
	}

	float angle = (2.0f * acosf(v));
	float scale = (float)sin(angle / 2);
	MVector3 result;

	if(scale != 0)
	{
		scale = 1.0f / scale;
		result.x = scale * values[0];
		result.y = scale * values[1];
		result.z = scale * values[2];
		return result;
	}

	return MVector3(0.0f, 1.0f, 0.0f);
}

MVector3 MQuaternion::getEulerAngles(void) const
{
	MMatrix4x4 matrix;
	matrix.setRotationAxis(getAngle(), getAxis());
	return matrix.getEulerAngles();
}