/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MKey.h
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


#ifndef _M_KEY
#define _M_KEY


class M_ENGINE_EXPORT MKey
{
public:

	MKey(void);
	~MKey(void);

private:

	int m_t;
	void * m_data;

public:

	inline void setT(int t){ m_t = t; }
	inline int getT(void){ return m_t; }

	float * createFloatData(void);
	MVector2 * createVector2Data(void);
	MVector3 * createVector3Data(void);
	MVector4 * createVector4Data(void);
	MQuaternion * createQuaternionData(void);

	inline void * getData(void){ return m_data; }
};

#endif