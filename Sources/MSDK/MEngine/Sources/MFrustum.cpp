/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MFrustum.cpp
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


#include "../Includes/MEngine.h"


static const int pointsId[4][2] = {
	{1, 0}, {2, 1}, {3, 2}, {0, 3}
};

void MFrustum::makeVolume(MOCamera * camera)
{
	// get viewport
	int * viewport = camera->getCurrentViewport();

	// make window points
	int wPoints2d[4][2];

	wPoints2d[0][0] = viewport[0];
	wPoints2d[0][1] = viewport[1];
	wPoints2d[1][0] = viewport[0] + viewport[2];
	wPoints2d[1][1] = viewport[1];
	wPoints2d[2][0] = viewport[0] + viewport[2];
	wPoints2d[2][1] = viewport[1] + viewport[3];
	wPoints2d[3][0] = viewport[0];
	wPoints2d[3][1] = viewport[1] + viewport[3];

	// front normal
	m_direction = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();

 	// compute volume points
	MVector3 position = camera->getTransformedPosition();

	int i;
	MVector3 p3d;
	for(i=0; i<4; i++)
	{
		p3d.x = (float)wPoints2d[i][0];
		p3d.y = (float)wPoints2d[i][1];
		
		p3d.z = 0.0f;
		m_points[i] = camera->getUnProjectedPoint(p3d);
		p3d.z = 1.0f;
		m_points[i+4] = camera->getUnProjectedPoint(p3d);
	}
	
	// compute normals (perspective)
	if(! camera->isOrtho())
	{
		for(i=0; i<4; i++)
		{
			m_normals[i] = getTriangleNormal(
				position, 
				m_points[pointsId[i][0]],
				m_points[pointsId[i][1]]
			);
		}
	
		return;
	}
	else
	{
		// compute normals (ortho)
		m_normals[0] = camera->getRotatedVector(MVector3(1, 0, 0)).getNormalized();
		m_normals[2] = - m_normals[0];
		m_normals[1] = camera->getRotatedVector(MVector3(0, 1, 0)).getNormalized();
		m_normals[3] = - m_normals[1];
	}
}

bool MFrustum::isPointCloudVisible(MVector3 * points, unsigned int pointsNumber)
{
	bool out = true;
	unsigned int p;

	// out of far plane
	for(p=0; p<pointsNumber; p++)
	{
		if((points[p] - m_points[4]).dotProduct(m_direction) <= 0)
		{
			out = false;
			break;
		}
	}

	if(out) return false;

	// out of near plane
	out = true;
	for(p=0; p<pointsNumber; p++)
	{
		// near
		if((points[p] - m_points[0]).dotProduct(m_direction) > 0)
		{
			out = false;
			break;
		}
	}

	if(out) return false;

	// out of camera planes
	for(int i=0; i<4; i++)
	{
		MVector3 * origin = &m_points[pointsId[i][1]];

		out = true;
		for(p=0; p<pointsNumber; p++)
		{
			float dot = (points[p] - (*origin)).dotProduct(m_normals[i]);
			if(dot > 0)
			{
				out = false;
				break;
			}
		}

		if(out) return false;
	}

	return true;
}