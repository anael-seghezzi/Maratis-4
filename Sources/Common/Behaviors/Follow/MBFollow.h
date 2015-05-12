/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBFollow.h
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


#ifndef _MB_FOLLOW_H
#define _MB_FOLLOW_H


class MBFollow : public MBehavior
{
public:

	// constructors / destructors
	MBFollow(MObject3d * parentObject);
	MBFollow(MBFollow & behavior, MObject3d * parentObject);
	~MBFollow(void);

private:

	// variables
	bool m_local;
	float m_delay;
	MVector3 m_offset;
	MString m_targetName;
	
public:

	// destroy
	void destroy(void);

	// get new
	static MBehavior * getNew(MObject3d * parentObject);

	// get copy
	MBehavior * getCopy(MObject3d * parentObject);

	// name
	static const char * getStaticName(void){ return "Follow"; }
	const char * getName(void){ return getStaticName(); }

	// events
	void update(void);
	void runEvent(int param){}

	// variables
	unsigned int getVariablesNumber(void);
	MVariable getVariable(unsigned int id);
};

#endif