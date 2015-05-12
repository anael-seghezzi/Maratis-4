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

float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin, bool end)
{
	float m0,m1,mu2,mu3;
	float a0,a1,a2,a3;
	
	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1-y0)*(1+bias)*(1-tension)*0.5f;
	m1 = (y3-y2)*(1-bias)*(1-tension)*0.5f;
	
	if(!begin)
		m0 += (y2-y1)*(1-bias)*(1-tension)*0.5f;
		
	if(!end)
		m1 += (y2-y1)*(1+bias)*(1-tension)*0.5f;
	
	a0 =  2*mu3 - 3*mu2 + 1;
	a1 =    mu3 - 2*mu2 + mu;
	a2 =    mu3 -   mu2;
	a3 = -2*mu3 + 3*mu2;
	
	return(a0*y1+a1*m0+a2*m1+a3*y2);
}

// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// Modified to use MVector3
//
//  This is the Douglas-Peucker recursive simplification routine
//  It just marks vertices that are part of the simplified polyline
//  for approximating the polyline subchain v[j] to v[k].
//    Input:  tol = approximation tolerance
//            v[] = polyline array of vertex points
//            j,k = indices for the subchain v[j] to v[k]
//    Output: mk[] = array of markers matching vertex array v[]

void simplifyDP(float tol, MVector3 * v, int j, int k, int * mk)
{
    if(k <= j+1) // there is nothing to simplify
        return;
	
    // check for adequate approximation by segment S from v[j] to v[k]
    int			maxi = j;					// index of vertex farthest from S
    float		maxd2 = 0;					// distance squared of farthest vertex
    float		tol2 = tol * tol;			// tolerance squared
    MVector3	S[2] = {v[j], v[k]};		// segment from v[j] to v[k]
    MVector3	u = S[1] - S[0];			// segment direction vector
    float		cu = u.getSquaredLength();	// segment length squared
	
    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_Point_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    MVector3 w;
    MVector3 Pb;               // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared
	
    for(int i=j+1; i<k; i++)
    {
        // compute distance squared
        w = v[i] - S[0];
        cw = w.dotProduct(u);
        if(cw <= 0)
            dv2 = (v[i] - S[0]).getSquaredLength();
        else if ( cu <= cw )
            dv2 = (v[i] - S[1]).getSquaredLength();
        else {
            b = cw / cu;
            Pb = S[0] + b * u;
            dv2 = (v[i] - Pb).getSquaredLength();
        }
		
        // test with current max distance squared
        if (dv2 <= maxd2)
            continue;
		
        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }
	
    if(maxd2 > tol2) // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
		
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
	
    // else the approximation is OK, so ignore intermediate vertices
    return;
}
