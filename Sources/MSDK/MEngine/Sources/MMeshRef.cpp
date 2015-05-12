/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMeshRef.cpp
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


MMeshRef::MMeshRef(MMesh * mesh, const char * filename):m_mesh(mesh)
{
	m_filename.set(filename);
}

MMeshRef::~MMeshRef(void)
{
	clear();
}

MMeshRef * MMeshRef::getNew(MMesh * mesh, const char * filename)
{
	return new MMeshRef(mesh, filename);
}

void MMeshRef::clear(void)
{
	if(m_mesh)
	{
		m_mesh->destroy();
		m_mesh = NULL;
	}
}

void MMeshRef::destroy(void)
{
	delete this;
}

void MMeshRef::update(void * arg)
{
	MEngine * engine = MEngine::getInstance();

	if(! m_mesh)
		m_mesh = MMesh::getNew();

	if(! engine->getMeshLoader()->loadData(getFilename(), m_mesh, arg))
		MLOG_WARNING("Cannot load data " << getFilename());
}
