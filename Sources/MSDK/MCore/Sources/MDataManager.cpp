/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MDataManager.cpp
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


#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../Includes/MCore.h"


// MDataRef
MDataRef::MDataRef(void):
m_time(0),
m_score(0)
{}

void MDataRef::incrScore(void)
{
	m_score++;
}

void MDataRef::decrScore(void)
{
	if(m_score > 0)
		m_score--;
}


// MDataManager
MDataManager::~MDataManager(void)
{
	clear();
}

void MDataManager::clear(void)
{
	unsigned int i;
	unsigned int size = m_refs.size();
	for(i=0; i<size; i++)
		m_refs[i]->destroy();

	m_refs.clear();
}

void MDataManager::addRef(MDataRef * ref)
{
	const char * filename = ref->getFilename();
	if(filename)
	{
		struct stat timeBuf;
		if(stat(filename, &timeBuf) == 0)
			ref->setTime((long)timeBuf.st_mtime);
	}

	m_refs.push_back(ref);
}

MDataRef * MDataManager::getRefFromFilename(const char * filename)
{
	unsigned int i, size = getRefsNumber();
	for(i=0; i<size; i++)
	{
		MDataRef * ref = getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
			return ref;
	}
	
	return NULL;
}
