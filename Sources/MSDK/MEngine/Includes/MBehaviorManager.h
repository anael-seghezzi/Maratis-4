/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MBehaviorManager.h
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


#ifndef _M_BEHAVIOR_MANAGER_H
#define _M_BEHAVIOR_MANAGER_H


class M_ENGINE_EXPORT MBehaviorManager
{
public:

	// constructors / destructors
	MBehaviorManager(void){}
	virtual ~MBehaviorManager(void);

private:

	vector <MBehaviorCreator *> m_behaviors;

public:

	void clear(void);

	// behaviors
	void addBehavior(const char * name, int objectFilter, MBehavior * (*getNewBehaviorFunctionPointer)(MObject3d * parentObject));
	MBehaviorCreator * getBehaviorByName(const char * name);

	inline unsigned int getBehaviorsNumber(void){ return m_behaviors.size(); }
	inline MBehaviorCreator * getBehaviorByIndex(const unsigned int id){ return m_behaviors[id]; }
};

#endif