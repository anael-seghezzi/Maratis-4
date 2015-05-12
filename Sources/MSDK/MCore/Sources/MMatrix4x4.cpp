/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMatrix4x4.cpp
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


#include <memory.h>
#include "../Includes/MCore.h"


MMatrix4x4::MMatrix4x4(
	float e0,	float e1,	float e2,	float e3,
	float e4,	float e5,	float e6,	float e7,
	float e8,	float e9,	float e10,	float e11,
	float e12,	float e13,	float e14,	float e15)
{
	entries[0] = e0;
	entries[1] = e1;
	entries[2] = e2;
	entries[3] = e3;
	entries[4] = e4;
	entries[5] = e5;
	entries[6] = e6;
	entries[7] = e7;
	entries[8] = e8;
	entries[9] = e9;
	entries[10] = e10;
	entries[11] = e11;
	entries[12] = e12;
	entries[13] = e13;
	entries[14] = e14;
	entries[15] = e15;
}

MMatrix4x4::MMatrix4x4(const MMatrix4x4 & mat)
{
	memcpy(entries, mat.entries, 16 * sizeof(float));
}

MMatrix4x4::MMatrix4x4(const float * value)
{
	memcpy(entries, value, 16*sizeof(float));
}

void MMatrix4x4::setEntry(int position, float value)
{
	if(position >= 0 && position <= 15)
		entries[position] = value;
}
	
float MMatrix4x4::getEntry(int position) const
{
	if(position >= 0 && position <= 15)
		return entries[position];

	return 0.0f;
}

MVector4 MMatrix4x4::getRow(int position) const
{
	if(position == 0)
		return MVector4(entries[0], entries[4], entries[8], entries[12]);
	
	if(position == 1)
		return MVector4(entries[1], entries[5], entries[9], entries[13]);
	
	if(position == 2)
		return MVector4(entries[2], entries[6], entries[10], entries[14]);
	
	if(position == 3)
		return MVector4(entries[3], entries[7], entries[11], entries[15]);

	return MVector4(0.0f, 0.0f, 0.0f, 0.0f);
}

MVector4 MMatrix4x4::getColumn(int position) const
{
	if(position == 0)
		return MVector4(entries[0], entries[1], entries[2], entries[3]);
	
	if(position == 1)
		return MVector4(entries[4], entries[5], entries[6], entries[7]);
	
	if(position == 2)
		return MVector4(entries[8], entries[9], entries[10], entries[11]);
	
	if(position == 3)
		return MVector4(entries[12], entries[13], entries[14], entries[15]);

	return MVector4(0.0f, 0.0f, 0.0f, 0.0f);
}

MVector3 MMatrix4x4::getEulerAngles(void) const
{
	// get euler angles from matrix
	float cy = sqrtf(entries[0] * entries[0] + entries[1] * entries[1]);
	if(cy > (16.0 * 1.192092896e-07F))
	{
		MVector3 euler;
		MVector3 euler1;
		MVector3 euler2;

		euler1.x = atan2f(entries[6], entries[10]);
		euler1.y = atan2f(- entries[2], cy);
		euler1.z = atan2f(entries[1], entries[0]);
		
		euler2.x = atan2f(- entries[6], - entries[10]);
		euler2.y = atan2f(- entries[2], - cy);
		euler2.z = atan2f(- entries[1], - entries[0]);
		
		if( (fabs(euler1.x) + fabs(euler1.y) + fabs(euler1.z)) > 
			(fabs(euler2.x) + fabs(euler2.y) + fabs(euler2.z)))
		{
			euler = euler2;
		}
		else
		{
			euler = euler1;
		}

		return euler * (float)RAD_TO_DEG;
	}
	else
	{
		MVector3 euler;

		euler.x = atan2f(- entries[9], entries[5]);
		euler.y = atan2f(- entries[2], cy);
		euler.z = 0.0f;

		return euler * (float)RAD_TO_DEG;
	}

	return MVector3(0, 0, 0);
}

MVector3 MMatrix4x4::getScale(void) const
{
	float xSize = getRotatedVector3(MVector3(1, 0, 0)).getLength();
	float ySize = getRotatedVector3(MVector3(0, 1, 0)).getLength();
	float zSize = getRotatedVector3(MVector3(0, 0, 1)).getLength();
	
	return MVector3(xSize, ySize, zSize);
}

void MMatrix4x4::loadIdentity(void)
{
	memset(entries, 0, 16*sizeof(float));
	entries[0] = 1.0f;
	entries[5] = 1.0f;
	entries[10] = 1.0f;
	entries[15] = 1.0f;
}

void MMatrix4x4::loadZero(void)
{
	memset(entries, 0, 16*sizeof(float));
}

MMatrix4x4 MMatrix4x4::operator + (const MMatrix4x4 & mat) const
{
	return MMatrix4x4(	
		entries[0] + mat.entries[0],
		entries[1] + mat.entries[1],
		entries[2] + mat.entries[2],
		entries[3] + mat.entries[3],
		entries[4] + mat.entries[4],
		entries[5] + mat.entries[5],
		entries[6] + mat.entries[6],
		entries[7] + mat.entries[7],
		entries[8] + mat.entries[8],
		entries[9] + mat.entries[9],
		entries[10] + mat.entries[10],
		entries[11] + mat.entries[11],
		entries[12] + mat.entries[12],
		entries[13] + mat.entries[13],
		entries[14] + mat.entries[14],
		entries[15] + mat.entries[15]
	);
}

MMatrix4x4 MMatrix4x4::operator - (const MMatrix4x4 & mat) const
{
	return MMatrix4x4(	
		entries[0] - mat.entries[0],
		entries[1] - mat.entries[1],
		entries[2] - mat.entries[2],
		entries[3] - mat.entries[3],
		entries[4] - mat.entries[4],
		entries[5] - mat.entries[5],
		entries[6] - mat.entries[6],
		entries[7] - mat.entries[7],
		entries[8] - mat.entries[8],
		entries[9] - mat.entries[9],
		entries[10] - mat.entries[10],
		entries[11] - mat.entries[11],
		entries[12] - mat.entries[12],
		entries[13] - mat.entries[13],
		entries[14] - mat.entries[14],
		entries[15] - mat.entries[15]
	);
}

MMatrix4x4 MMatrix4x4::operator * (const MMatrix4x4 & mat) const
{
	// if bottom row is (0, 0, 0, 1) in both matrices
	if(	entries[3]==0.0f && entries[7]==0.0f && 
		entries[11]==0.0f && entries[15]==1.0f	&&
		mat.entries[3]==0.0f && mat.entries[7]==0.0f &&
		mat.entries[11]==0.0f && mat.entries[15]==1.0f)
	{
		return MMatrix4x4(	
			entries[0]*mat.entries[0]+entries[4]*mat.entries[1]+entries[8]*mat.entries[2],
			entries[1]*mat.entries[0]+entries[5]*mat.entries[1]+entries[9]*mat.entries[2],
			entries[2]*mat.entries[0]+entries[6]*mat.entries[1]+entries[10]*mat.entries[2],
			0.0f,
			entries[0]*mat.entries[4]+entries[4]*mat.entries[5]+entries[8]*mat.entries[6],
			entries[1]*mat.entries[4]+entries[5]*mat.entries[5]+entries[9]*mat.entries[6],
			entries[2]*mat.entries[4]+entries[6]*mat.entries[5]+entries[10]*mat.entries[6],
			0.0f,
			entries[0]*mat.entries[8]+entries[4]*mat.entries[9]+entries[8]*mat.entries[10],
			entries[1]*mat.entries[8]+entries[5]*mat.entries[9]+entries[9]*mat.entries[10],
			entries[2]*mat.entries[8]+entries[6]*mat.entries[9]+entries[10]*mat.entries[10],
			0.0f,
			entries[0]*mat.entries[12]+entries[4]*mat.entries[13]+entries[8]*mat.entries[14]+entries[12],
			entries[1]*mat.entries[12]+entries[5]*mat.entries[13]+entries[9]*mat.entries[14]+entries[13],
			entries[2]*mat.entries[12]+entries[6]*mat.entries[13]+entries[10]*mat.entries[14]+entries[14],
			1.0f
		);
	}

	// if bottom row of 1st matrix is (0, 0, 0, 1)
	if(	entries[3]==0.0f && entries[7]==0.0f && 
		entries[11]==0.0f && entries[15]==1.0f)
	{
		return MMatrix4x4(	
			entries[0]*mat.entries[0]+entries[4]*mat.entries[1]+entries[8]*mat.entries[2]+entries[12]*mat.entries[3],
			entries[1]*mat.entries[0]+entries[5]*mat.entries[1]+entries[9]*mat.entries[2]+entries[13]*mat.entries[3],
			entries[2]*mat.entries[0]+entries[6]*mat.entries[1]+entries[10]*mat.entries[2]+entries[14]*mat.entries[3],
			mat.entries[3],
			entries[0]*mat.entries[4]+entries[4]*mat.entries[5]+entries[8]*mat.entries[6]+entries[12]*mat.entries[7],
			entries[1]*mat.entries[4]+entries[5]*mat.entries[5]+entries[9]*mat.entries[6]+entries[13]*mat.entries[7],
			entries[2]*mat.entries[4]+entries[6]*mat.entries[5]+entries[10]*mat.entries[6]+entries[14]*mat.entries[7],
			mat.entries[7],
			entries[0]*mat.entries[8]+entries[4]*mat.entries[9]+entries[8]*mat.entries[10]+entries[12]*mat.entries[11],
			entries[1]*mat.entries[8]+entries[5]*mat.entries[9]+entries[9]*mat.entries[10]+entries[13]*mat.entries[11],
			entries[2]*mat.entries[8]+entries[6]*mat.entries[9]+entries[10]*mat.entries[10]+entries[14]*mat.entries[11],
			mat.entries[11],
			entries[0]*mat.entries[12]+entries[4]*mat.entries[13]+entries[8]*mat.entries[14]+entries[12]*mat.entries[15],
			entries[1]*mat.entries[12]+entries[5]*mat.entries[13]+entries[9]*mat.entries[14]+entries[13]*mat.entries[15],
			entries[2]*mat.entries[12]+entries[6]*mat.entries[13]+entries[10]*mat.entries[14]+entries[14]*mat.entries[15],
			mat.entries[15]
		);
	}

	// if bottom row of 2nd matrix is (0, 0, 0, 1)
	if(	mat.entries[3]==0.0f && mat.entries[7]==0.0f &&
		mat.entries[11]==0.0f && mat.entries[15]==1.0f)
	{
		return MMatrix4x4(	
			entries[0]*mat.entries[0]+entries[4]*mat.entries[1]+entries[8]*mat.entries[2],
			entries[1]*mat.entries[0]+entries[5]*mat.entries[1]+entries[9]*mat.entries[2],
			entries[2]*mat.entries[0]+entries[6]*mat.entries[1]+entries[10]*mat.entries[2],
			entries[3]*mat.entries[0]+entries[7]*mat.entries[1]+entries[11]*mat.entries[2],
			entries[0]*mat.entries[4]+entries[4]*mat.entries[5]+entries[8]*mat.entries[6],
			entries[1]*mat.entries[4]+entries[5]*mat.entries[5]+entries[9]*mat.entries[6],
			entries[2]*mat.entries[4]+entries[6]*mat.entries[5]+entries[10]*mat.entries[6],
			entries[3]*mat.entries[4]+entries[7]*mat.entries[5]+entries[11]*mat.entries[6],
			entries[0]*mat.entries[8]+entries[4]*mat.entries[9]+entries[8]*mat.entries[10],
			entries[1]*mat.entries[8]+entries[5]*mat.entries[9]+entries[9]*mat.entries[10],
			entries[2]*mat.entries[8]+entries[6]*mat.entries[9]+entries[10]*mat.entries[10],
			entries[3]*mat.entries[8]+entries[7]*mat.entries[9]+entries[11]*mat.entries[10],
			entries[0]*mat.entries[12]+entries[4]*mat.entries[13]+entries[8]*mat.entries[14]+entries[12],
			entries[1]*mat.entries[12]+entries[5]*mat.entries[13]+entries[9]*mat.entries[14]+entries[13],
			entries[2]*mat.entries[12]+entries[6]*mat.entries[13]+entries[10]*mat.entries[14]+entries[14],
			entries[3]*mat.entries[12]+entries[7]*mat.entries[13]+entries[11]*mat.entries[14]+entries[15]
		);
	}
	
	return MMatrix4x4(	
		entries[0]*mat.entries[0]+entries[4]*mat.entries[1]+entries[8]*mat.entries[2]+entries[12]*mat.entries[3],
		entries[1]*mat.entries[0]+entries[5]*mat.entries[1]+entries[9]*mat.entries[2]+entries[13]*mat.entries[3],
		entries[2]*mat.entries[0]+entries[6]*mat.entries[1]+entries[10]*mat.entries[2]+entries[14]*mat.entries[3],
		entries[3]*mat.entries[0]+entries[7]*mat.entries[1]+entries[11]*mat.entries[2]+entries[15]*mat.entries[3],
		entries[0]*mat.entries[4]+entries[4]*mat.entries[5]+entries[8]*mat.entries[6]+entries[12]*mat.entries[7],
		entries[1]*mat.entries[4]+entries[5]*mat.entries[5]+entries[9]*mat.entries[6]+entries[13]*mat.entries[7],
		entries[2]*mat.entries[4]+entries[6]*mat.entries[5]+entries[10]*mat.entries[6]+entries[14]*mat.entries[7],
		entries[3]*mat.entries[4]+entries[7]*mat.entries[5]+entries[11]*mat.entries[6]+entries[15]*mat.entries[7],
		entries[0]*mat.entries[8]+entries[4]*mat.entries[9]+entries[8]*mat.entries[10]+entries[12]*mat.entries[11],
		entries[1]*mat.entries[8]+entries[5]*mat.entries[9]+entries[9]*mat.entries[10]+entries[13]*mat.entries[11],
		entries[2]*mat.entries[8]+entries[6]*mat.entries[9]+entries[10]*mat.entries[10]+entries[14]*mat.entries[11],
		entries[3]*mat.entries[8]+entries[7]*mat.entries[9]+entries[11]*mat.entries[10]+entries[15]*mat.entries[11],
		entries[0]*mat.entries[12]+entries[4]*mat.entries[13]+entries[8]*mat.entries[14]+entries[12]*mat.entries[15],
		entries[1]*mat.entries[12]+entries[5]*mat.entries[13]+entries[9]*mat.entries[14]+entries[13]*mat.entries[15],
		entries[2]*mat.entries[12]+entries[6]*mat.entries[13]+entries[10]*mat.entries[14]+entries[14]*mat.entries[15],
		entries[3]*mat.entries[12]+entries[7]*mat.entries[13]+entries[11]*mat.entries[14]+entries[15]*mat.entries[15]
	);
}

MMatrix4x4 MMatrix4x4::operator * (const float value) const
{
	return MMatrix4x4(	
		entries[0] * value,
		entries[1] * value,
		entries[2] * value,
		entries[3] * value,
		entries[4] * value,
		entries[5] * value,
		entries[6] * value,
		entries[7] * value,
		entries[8] * value,
		entries[9] * value,
		entries[10] * value,
		entries[11] * value,
		entries[12] * value,
		entries[13] * value,
		entries[14] * value,
		entries[15] * value
	);
}

MMatrix4x4 MMatrix4x4::operator / (const float value) const
{
	if(value == 0.0f || value == 1.0f)
		return (*this);
		
	float temp = 1/value;

	return (*this) * temp;
}

MMatrix4x4 operator * (float factor, const MMatrix4x4 & mat)
{
	return mat * factor;
}

bool MMatrix4x4::operator == (const MMatrix4x4 & mat) const
{
	for(int i=0; i<16; i++)
	{
		if(entries[i] != mat.entries[i])
			return false;
	}
	return true;
}

bool MMatrix4x4::operator != (const MMatrix4x4 & mat) const
{
	return !((*this) == mat);
}

void MMatrix4x4::operator += (const MMatrix4x4 & mat)
{
	(*this) = (*this) + mat;
}

void MMatrix4x4::operator -= (const MMatrix4x4 & mat)
{
	(*this) = (*this) - mat;
}

void MMatrix4x4::operator *= (const MMatrix4x4 & mat)
{
	(*this) = (*this) * mat;
}

void MMatrix4x4::operator *= (const float value)
{
	(*this) = (*this) * value;
}

void MMatrix4x4::operator /= (const float value)
{
	(*this) = (*this) / value;
}

MMatrix4x4 MMatrix4x4::operator - (void) const
{
	MMatrix4x4 result(*this);

	for(int i=0; i<16; i++)
		result.entries[i] = -result.entries[i];

	return result;
}

MVector4 MMatrix4x4::operator * (const MVector4 mat) const
{
	// if bottom row is (0, 0, 0, 1)
	if(entries[3] == 0.0f && entries[7] == 0.0f && entries[11] == 0.0f && entries[15] == 1.0f)
	{
		return MVector4(
			  entries[0]*mat.x
			+ entries[4]*mat.y
			+ entries[8]*mat.z
			+ entries[12]*mat.w,

			  entries[1]*mat.x
			+ entries[5]*mat.y
			+ entries[9]*mat.z
			+ entries[13]*mat.w,

			  entries[2]*mat.x
			+ entries[6]*mat.y
			+ entries[10]*mat.z
			+ entries[14]*mat.w,

			mat.w
		);
	}
	
	return MVector4(	
		  entries[0]*mat.x
		+ entries[4]*mat.y
		+ entries[8]*mat.z
		+ entries[12]*mat.w,

		  entries[1]*mat.x
		+ entries[5]*mat.y
		+ entries[9]*mat.z
		+ entries[13]*mat.w,

		  entries[2]*mat.x
		+ entries[6]*mat.y
		+ entries[10]*mat.z
		+ entries[14]*mat.w,

		  entries[3]*mat.x
		+ entries[7]*mat.y
		+ entries[11]*mat.z
		+ entries[15]*mat.w
	);
}

MVector3 MMatrix4x4::getInverseRotatedVector3(const MVector3 & vec) const
{
	return MVector3(
		entries[0]*vec.x + entries[1]*vec.y + entries[2]*vec.z,
		entries[4]*vec.x + entries[5]*vec.y + entries[6]*vec.z,
		entries[8]*vec.x + entries[9]*vec.y + entries[10]*vec.z
	);
}

MVector3 MMatrix4x4::getTranslatedVector3(const MVector3 & vec) const
{
	return MVector3(vec.x + entries[12], vec.y + entries[13], vec.z + entries[14]);
}

MVector3 MMatrix4x4::getInversetranslatedVector3(const MVector3 & vec) const
{
	return MVector3(vec.x - entries[12], vec.y - entries[13], vec.z - entries[14]);
}

void MMatrix4x4::invert(void)
{
	*this = getInverse();
}

MMatrix4x4 MMatrix4x4::getInverse(void) const
{
	MMatrix4x4 result = getInversetranspose();
	result.transpose();

	return result;
}

void MMatrix4x4::transpose(void)
{
	*this = getTranspose();
}

MMatrix4x4 MMatrix4x4::getTranspose(void) const
{
	return MMatrix4x4(	
		entries[ 0], entries[ 4], entries[ 8], entries[12],
		entries[ 1], entries[ 5], entries[ 9], entries[13],
		entries[ 2], entries[ 6], entries[10], entries[14],
		entries[ 3], entries[ 7], entries[11], entries[15]
	);
}

void MMatrix4x4::invertTranspose(void)
{
	*this = getInversetranspose();
}

MMatrix4x4 MMatrix4x4::getInversetranspose(void) const
{
	MMatrix4x4 result;

	float tmp[12]; // pair storage
	float det; // determinant

	// calculate pairs for first 8 elements (cofactors)
	tmp[0] = entries[10] * entries[15];
	tmp[1] = entries[11] * entries[14];
	tmp[2] = entries[9] * entries[15];
	tmp[3] = entries[11] * entries[13];
	tmp[4] = entries[9] * entries[14];
	tmp[5] = entries[10] * entries[13];
	tmp[6] = entries[8] * entries[15];
	tmp[7] = entries[11] * entries[12];
	tmp[8] = entries[8] * entries[14];
	tmp[9] = entries[10] * entries[12];
	tmp[10] = entries[8] * entries[13];
	tmp[11] = entries[9] * entries[12];

	// calculate first 8 elements (cofactors)
	result.setEntry(0,	tmp[0]*entries[5] + tmp[3]*entries[6] + tmp[4]*entries[7]
					-	tmp[1]*entries[5] - tmp[2]*entries[6] - tmp[5]*entries[7]);

	result.setEntry(1,	tmp[1]*entries[4] + tmp[6]*entries[6] + tmp[9]*entries[7]
					-	tmp[0]*entries[4] - tmp[7]*entries[6] - tmp[8]*entries[7]);

	result.setEntry(2,	tmp[2]*entries[4] + tmp[7]*entries[5] + tmp[10]*entries[7]
					-	tmp[3]*entries[4] - tmp[6]*entries[5] - tmp[11]*entries[7]);

	result.setEntry(3,	tmp[5]*entries[4] + tmp[8]*entries[5] + tmp[11]*entries[6]
					-	tmp[4]*entries[4] - tmp[9]*entries[5] - tmp[10]*entries[6]);

	result.setEntry(4,	tmp[1]*entries[1] + tmp[2]*entries[2] + tmp[5]*entries[3]
					-	tmp[0]*entries[1] - tmp[3]*entries[2] - tmp[4]*entries[3]);

	result.setEntry(5,	tmp[0]*entries[0] + tmp[7]*entries[2] + tmp[8]*entries[3]
					-	tmp[1]*entries[0] - tmp[6]*entries[2] - tmp[9]*entries[3]);

	result.setEntry(6,	tmp[3]*entries[0] + tmp[6]*entries[1] + tmp[11]*entries[3]
					-	tmp[2]*entries[0] - tmp[7]*entries[1] - tmp[10]*entries[3]);

	result.setEntry(7,	tmp[4]*entries[0] + tmp[9]*entries[1] + tmp[10]*entries[2]
					-	tmp[5]*entries[0] - tmp[8]*entries[1] - tmp[11]*entries[2]);

	// calculate pairs for second 8 elements (cofactors)
	tmp[0] = entries[2]*entries[7];
	tmp[1] = entries[3]*entries[6];
	tmp[2] = entries[1]*entries[7];
	tmp[3] = entries[3]*entries[5];
	tmp[4] = entries[1]*entries[6];
	tmp[5] = entries[2]*entries[5];
	tmp[6] = entries[0]*entries[7];
	tmp[7] = entries[3]*entries[4];
	tmp[8] = entries[0]*entries[6];
	tmp[9] = entries[2]*entries[4];
	tmp[10] = entries[0]*entries[5];
	tmp[11] = entries[1]*entries[4];

	// calculate second 8 elements (cofactors)
	result.setEntry(8,	tmp[0]*entries[13] + tmp[3]*entries[14] + tmp[4]*entries[15]
					-	tmp[1]*entries[13] - tmp[2]*entries[14] - tmp[5]*entries[15]);

	result.setEntry(9,	tmp[1]*entries[12] + tmp[6]*entries[14] + tmp[9]*entries[15]
					-	tmp[0]*entries[12] - tmp[7]*entries[14] - tmp[8]*entries[15]);

	result.setEntry(10,	tmp[2]*entries[12] + tmp[7]*entries[13] + tmp[10]*entries[15]
					-	tmp[3]*entries[12] - tmp[6]*entries[13] - tmp[11]*entries[15]);

	result.setEntry(11,	tmp[5]*entries[12] + tmp[8]*entries[13] + tmp[11]*entries[14]
					-	tmp[4]*entries[12] - tmp[9]*entries[13] - tmp[10]*entries[14]);

	result.setEntry(12,	tmp[2]*entries[10] + tmp[5]*entries[11] + tmp[1]*entries[9]
					-	tmp[4]*entries[11] - tmp[0]*entries[9] - tmp[3]*entries[10]);

	result.setEntry(13,	tmp[8]*entries[11] + tmp[0]*entries[8] + tmp[7]*entries[10]
					-	tmp[6]*entries[10] - tmp[9]*entries[11] - tmp[1]*entries[8]);

	result.setEntry(14,	tmp[6]*entries[9] + tmp[11]*entries[11] + tmp[3]*entries[8]
					-	tmp[10]*entries[11] - tmp[2]*entries[8] - tmp[7]*entries[9]);

	result.setEntry(15,	tmp[10]*entries[10] + tmp[4]*entries[8] + tmp[9]*entries[9]
					-	tmp[8]*entries[9] - tmp[11]*entries[10] - tmp[5]*entries[8]);

	// calculate determinant
	det	= entries[0]*result.getEntry(0)
		+ entries[1]*result.getEntry(1)
		+ entries[2]*result.getEntry(2)
		+ entries[3]*result.getEntry(3);

	if(det == 0.0f)
	{
		MMatrix4x4 id;
		return id;
	}
	
	result = result/det;

	return result;
}

void MMatrix4x4::affineInvert(void)
{
	(*this) = getAffineInverse();
}

MMatrix4x4 MMatrix4x4::getAffineInverse(void) const
{
	return MMatrix4x4(	
		entries[0],
		entries[4],
		entries[8],
		0.0f,
		entries[1],
		entries[5],
		entries[9],
		0.0f,
		entries[2],
		entries[6],
		entries[10],
		0.0f,
		-(entries[0]*entries[12] + entries[1]*entries[13] + entries[2]*entries[14]),
		-(entries[4]*entries[12] + entries[5]*entries[13] + entries[6]*entries[14]),
		-(entries[8]*entries[12] + entries[9]*entries[13] + entries[10]*entries[14]),
		1.0f
	);
}

void MMatrix4x4::affineInvertTranspose(void)
{
	(*this) = getAffineInverseTranspose();
}

MMatrix4x4 MMatrix4x4::getAffineInverseTranspose(void) const
{
	return MMatrix4x4(	
		entries[0],
		entries[1],
		entries[2],
		-(entries[0]*entries[12] + entries[1]*entries[13] + entries[2]*entries[14]),
		entries[4],
		entries[5],
		entries[6],
		-(entries[4]*entries[12] +entries[5]*entries[13] + entries[6]*entries[14]),
		entries[8],
		entries[9],
		entries[10],
		-(entries[8]*entries[12] +entries[9]*entries[13] + entries[10]*entries[14]),
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

void MMatrix4x4::setTranslation(const MVector3 & translation)
{
	loadIdentity();
	setTranslationPart(translation);
}

void MMatrix4x4::setScale(const MVector3 & scaleFactor)
{
	loadIdentity();

	entries[0] = scaleFactor.x;
	entries[5] = scaleFactor.y;
	entries[10] = scaleFactor.z;
}

void MMatrix4x4::setUniformScale(const float size)
{
	loadIdentity();
	entries[0] = entries[5] = entries[10] = size;
}

void MMatrix4x4::scale(const MVector3 & scaleFactor)
{
	entries[0] *= scaleFactor.x;
	entries[1] *= scaleFactor.x;
	entries[2] *= scaleFactor.x;
	entries[3] *= scaleFactor.x;
	entries[4] *= scaleFactor.y;
	entries[5] *= scaleFactor.y;
	entries[6] *= scaleFactor.y;
	entries[7] *= scaleFactor.y;
	entries[8] *= scaleFactor.z;
	entries[9] *= scaleFactor.z;
	entries[10] *= scaleFactor.z;
	entries[11] *= scaleFactor.z;
}

void MMatrix4x4::rotate(const MVector3 & axis, const float angle)
{
	MMatrix4x4 matrix;
	matrix.setRotationAxis(angle, axis);
	(*this) = (*this) * matrix;
}

void MMatrix4x4::translate(const MVector3 translate)
{
	MMatrix4x4 matrix;
	matrix.setTranslationPart(translate);
	(*this) = (*this) * matrix;
}

void MMatrix4x4::setRotationAxis(const float angle, const MVector3 & axis)
{
	MVector3 u = axis.getNormalized();

	float sinAngle = (float)sin(angle * DEG_TO_RAD);
	float cosAngle = (float)cos(angle * DEG_TO_RAD);
	float oneMinusCosAngle = 1.0f - cosAngle;

	loadIdentity();

	entries[0] = (u.x)*(u.x) + cosAngle * (1-(u.x)*(u.x));
	entries[4] = (u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
	entries[8] = (u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

	entries[1] = (u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
	entries[5] = (u.y)*(u.y) + cosAngle * (1-(u.y)*(u.y));
	entries[9] = (u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
	
	entries[2] = (u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
	entries[6] = (u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
	entries[10] = (u.z)*(u.z) + cosAngle * (1-(u.z)*(u.z));
}

void MMatrix4x4::setRotationX(const float angle)
{
	loadIdentity();

	entries[5] = (float)cos(angle * DEG_TO_RAD);
	entries[6] = (float)sin(angle * DEG_TO_RAD);

	entries[9] = -entries[6];
	entries[10] = entries[5];
}

void MMatrix4x4::setRotationY(const float angle)
{
	loadIdentity();

	entries[0] =  (float)cos(angle * DEG_TO_RAD);
	entries[2] = -(float)sin(angle * DEG_TO_RAD);

	entries[2] = -entries[8];
	entries[10] = entries[0];
}

void MMatrix4x4::setRotationZ(const float angle)
{
	loadIdentity();

	entries[0] = (float)cos(angle * DEG_TO_RAD);
	entries[1] = (float)sin(angle * DEG_TO_RAD);

	entries[4] = -entries[1];
	entries[5] =  entries[0];
}

void MMatrix4x4::setRotationEuler(const float angleX, const float angleY, const float angleZ)
{
	loadIdentity();
	setRotationPartEuler(angleX, angleY, angleZ);
}

void MMatrix4x4::setTranslationPart(const MVector3 & translation)
{
	entries[12] = translation.x;
	entries[13] = translation.y;
	entries[14] = translation.z;
}

void MMatrix4x4::setRotationPartEuler(const float angleX, const float angleY, const float angleZ)
{
	double cr = cos( angleX * DEG_TO_RAD );
	double sr = sin( angleX * DEG_TO_RAD );
	double cp = cos( angleY * DEG_TO_RAD );
	double sp = sin( angleY * DEG_TO_RAD );
	double cy = cos( angleZ * DEG_TO_RAD );
	double sy = sin( angleZ * DEG_TO_RAD );

	entries[0] = ( float )( cp*cy );
	entries[1] = ( float )( cp*sy );
	entries[2] = ( float )( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	entries[4] = ( float )( srsp*cy-cr*sy );
	entries[5] = ( float )( srsp*sy+cr*cy );
	entries[6] = ( float )( sr*cp );

	entries[8] = ( float )( crsp*cy+sr*sy );
	entries[9] = ( float )( crsp*sy-sr*cy );
	entries[10] = ( float )( cr*cp );
}

void MMatrix4x4::lookAt(const MVector3 & pos, const MVector3 & dir, const MVector3 & up)
{
	MVector3 lftN = dir.crossProduct(up).getNormalized();
	MVector3 upN = lftN.crossProduct(dir).getNormalized();
	MVector3 dirN = dir.getNormalized();

	entries[0] = lftN.x;
	entries[1] = upN.x;
	entries[2] = -dirN.x;
	entries[3] = 0.0f;

	entries[4] = lftN.y;
	entries[5] = upN.y;
	entries[6] = -dirN.y;
	entries[7] = 0.0f;

	entries[8] = lftN.z;
	entries[9] = upN.z;
	entries[10] = -dirN.z;
	entries[11] = 0.0f;

	entries[12] = -lftN.dotProduct(pos);
	entries[13] = -upN.dotProduct(pos);
	entries[14] = dirN.dotProduct(pos);
	entries[15] = 1.0f;
}