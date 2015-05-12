/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MSkinData.h
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


#ifndef _M_SKIN_DATA_H
#define _M_SKIN_DATA_H


// Skin Point
class M_ENGINE_EXPORT MSkinPoint
{
public:

	MSkinPoint(void);
	~MSkinPoint(void);

private:

	// bones
	unsigned int m_bonesNumber;
	unsigned short * m_bonesIds;
	float * m_bonesWeights;

	// vertex id
	unsigned int m_vertexId;

public:

	// bones
	void clearBonesLinks(void);
	bool allocateBonesLinks(unsigned int size);

	inline unsigned int getBonesNumber(void){ return m_bonesNumber; }
	inline unsigned short * getBonesIds(void){ return m_bonesIds; }
	inline float * getBonesWeights(void){ return m_bonesWeights; }

	// vertex id
	inline void setVertexId(unsigned int vertexId){ m_vertexId = vertexId; }
	inline unsigned int getVertexId(void){ return m_vertexId; }
};


// Skin Data
class M_ENGINE_EXPORT MSkinData
{
public:

	MSkinData(void);
	~MSkinData(void);

private:

	// points
	unsigned int m_pointsNumber;
	MSkinPoint * m_points;

public:

	// points
	void clearPoints(void);
	MSkinPoint * allocPoints(unsigned int size);
	inline unsigned int getPointsNumber(void){ return m_pointsNumber; }
	inline MSkinPoint * getPoint(unsigned int id){ return &m_points[id]; }
};

#endif