/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MBehavior.h
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


#ifndef _M_BEHAVIOR_H
#define _M_BEHAVIOR_H


// class for creating behaviors
class M_ENGINE_EXPORT MBehavior
{
public:

	MBehavior(MObject3d * parentObject);
	virtual ~MBehavior(void);

private:

	MObject3d * m_parentObject;

public:

	// destroy
	virtual void destroy(void) = 0;

	// parent
	inline MObject3d * getParentObject(void){ return m_parentObject; }

	// name
	virtual const char * getName(void) = 0;

	// get copy
	virtual MBehavior * getCopy(MObject3d * parentObject) = 0;

	// events
	virtual void update(void) = 0;
	virtual void draw(void){}
	virtual void runEvent(int param){}

	// variables
	virtual unsigned int getVariablesNumber(void) = 0;
	virtual MVariable getVariable(unsigned int id) = 0;
};


#endif