/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMorphingData.h
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


#ifndef _M_MORPHING_DATA_H
#define _M_MORPHING_DATA_H


// Buffer
class M_ENGINE_EXPORT MMorphingPoint
{
public:

	MMorphingPoint(void);
	~MMorphingPoint(void);

private:

	// vertex id
	unsigned int m_vertexId;

	// dif vector
	MVector3 m_difVector;

public:

	// get vertex id
	inline unsigned int getVertexId(void){ return m_vertexId; }
};


// Pose
class M_ENGINE_EXPORT MMorphingPose
{
public:

	MMorphingPose(void);
	~MMorphingPose(void);

private:

	// points
	unsigned int m_pointsNumber;
	MMorphingPoint * m_points;

public:
	
	// points
	void clearPoints(void);
	MMorphingPoint * allocPoints(unsigned int size);
	inline unsigned int getPointsNumber(void){ return m_pointsNumber; }
	inline MMorphingPoint * getPoint(unsigned int id){ return &m_points[id]; }
};


// Morphing Data
class M_ENGINE_EXPORT MMorphingData
{
public:

	MMorphingData(void);
	~MMorphingData(void);

private:

	unsigned int m_posesNumber;
	MMorphingPose ** m_poses;

public:
	
	void allocPoses(unsigned int size);
	void clearPoses(void);
	void addPose(MMorphingPose * pose);
	inline unsigned int getPosesNumber(void){ return m_posesNumber; }
	inline MMorphingPose * getPose(unsigned int id){ return m_poses[id]; }
};

#endif