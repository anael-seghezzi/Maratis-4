/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaths.cpp
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


void sortFloatIndexList(int indexList[], float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[(start + end)/2];
	
	while(i <= j)
	{
		while(floatList[i] > middle) i++;
		while(floatList[j] < middle) j--;
		
		if(i <= j)
		{
			float temp = floatList[i];
			floatList[i] = floatList[j];
			floatList[j] = temp;
			
			int itemp = indexList[i];
			indexList[i] = indexList[j];
			indexList[j] = itemp;
			
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatIndexList(indexList, floatList, i, end);
	if(start < j)
		sortFloatIndexList(indexList, floatList, start, j);
}

void sortFloatList(float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[(start + end)/2];
	
	while(i <= j)
	{
		while(floatList[i] > middle) i++;
		while(floatList[j] < middle) j--;
		
		if(i <= j)
		{
			float temp = floatList[i];
			floatList[i] = floatList[j];
			floatList[j] = temp;
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatList(floatList, i, end);
	if(start < j)
		sortFloatList(floatList, start, j);
}

float loopFloat(float val, float min, float max)
{
	if(val >= min && val < max)
		return val;
	
	float d = max - min;
	if(d <= 0)
		return min;
	
	float v = (val - min) / d;
	v = v - (float)((int)v);
	
	if(v < 0)
		return max + (d*v);
	else
		return min + (d*v);
}

bool lineToLineIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I)
{
	MVector2 DP, QA, QB;
	float d, la, lb;

	DP.x = C.x - A.x ; DP.y = C.y - A.y;
	QA.x = B.x - A.x ; QA.y = B.y - A.y;
	QB.x = D.x - C.x ; QB.y = D.y - C.y;

	d = QA.y * QB.x - QB.y * QA.x;
	if(d == 0)
		return false;
		
	la = (QB.x * DP.y - QB.y * DP.x) / d;

	if(la < 0 || la > 1)
		return false;

	lb = (QA.x * DP.y - QA.y * DP.x) / d;

	if(lb < 0 || lb > 1)
		return false;

	if(I)
	{
		I->x = A.x + la * QA.x;
		I->y = A.y + la * QA.y;
	}
	return true;
}

bool boxToBoxCollision2d(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB)
{
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;
	return true;
}

bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max)
{
	if(point.x > max.x) return false;
	if(point.x < min.x) return false;
	if(point.y > max.y) return false;
	if(point.y < min.y) return false;
	return true;
}

bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max)
{
	if(point.x > max.x) return false;
	if(point.x < min.x) return false;
	if(point.y > max.y) return false;
	if(point.y < min.y) return false;
	if(point.z > max.z) return false;
	if(point.z < min.z) return false;
	return true;
}

MVector3 getTriangleNormal(const MVector3 & A, const MVector3 & B, const MVector3 & C)
{
	MVector3 vec1 = A - B;
	MVector3 vec2 = A - C;
	return vec1.crossProduct(vec2).getNormalized();
}
