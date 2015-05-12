/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MDataLoader.cpp
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


// Data load function
MDataLoadFunction::MDataLoadFunction(bool (* functionPtr)(const char * filename, void * data, void * arg)):
m_functionPtr(functionPtr)
{}

MDataLoadFunction::~MDataLoadFunction(void)
{}

// Data loader
MDataLoader::MDataLoader(void)
{}

MDataLoader::~MDataLoader(void)
{
	clear();
}

void MDataLoader::clear(void)
{
	unsigned int i;
	unsigned int lSize = m_loaders.size();
	for(i=0; i<lSize; i++)
		SAFE_DELETE(m_loaders[i]);

	m_loaders.clear();
}

void MDataLoader::addLoader(bool (* functionPtr)(const char * filename, void * data, void * arg))
{
	m_loaders.push_back(new MDataLoadFunction(functionPtr));
}

bool MDataLoader::loadData(const char * filename, void * data, void * arg)
{
	// parse loaders
	unsigned int i;
	unsigned int lSize = m_loaders.size();
	for(i=0; i<lSize; i++)
	{
		MDataLoadFunction * loader = m_loaders[i];
		if(loader->m_functionPtr(filename, data, arg))
			return true;
	}

	return false;
}
