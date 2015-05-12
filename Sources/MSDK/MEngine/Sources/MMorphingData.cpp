/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMorphingData.cpp
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


MMorphingPoint::MMorphingPoint(void):
m_vertexId(0)
{}

MMorphingPoint::~MMorphingPoint(void)
{}


MMorphingPose::MMorphingPose(void):
m_pointsNumber(0),
m_points(NULL)
{}

MMorphingPose::~MMorphingPose(void)
{
	clearPoints();
}

MMorphingPoint * MMorphingPose::allocPoints(unsigned int size)
{
	clearPoints();
	if(size == 0)
		return NULL;
	m_pointsNumber = size;
	m_points = new MMorphingPoint[size];
	return m_points;
}

void MMorphingPose::clearPoints(void)
{
	m_pointsNumber = 0;
	SAFE_DELETE_ARRAY(m_points);
}

MMorphingData::MMorphingData(void):
m_posesNumber(0),
m_poses(NULL)
{}

MMorphingData::~MMorphingData(void)
{
	clearPoses();
}

void MMorphingData::allocPoses(unsigned int size)
{
	clearPoses();
	if(size == 0)
		return;
	m_poses = (MMorphingPose **) new int[size];
}

void MMorphingData::clearPoses(void)
{
	unsigned int i;
	for(i=0; i<m_posesNumber; i++)
		SAFE_DELETE(m_poses[i]);

	m_posesNumber = 0;
	SAFE_DELETE_ARRAY(m_poses);
}

void MMorphingData::addPose(MMorphingPose * pose)
{
	m_poses[m_posesNumber] = pose;
	m_posesNumber++;
}