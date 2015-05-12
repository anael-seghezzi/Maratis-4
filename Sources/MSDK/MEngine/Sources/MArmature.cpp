/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MArmature.cpp
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


// constructors
MArmature::MArmature(void):
m_bonesNumber(0),
m_bones(NULL)
{}

// destructor
MArmature::~MArmature(void)
{
	clearBones();
}

void MArmature::clearBones(void)
{
	unsigned int i;
	for(i=0; i<m_bonesNumber; i++)
		SAFE_DELETE(m_bones[i]);

	SAFE_FREE(m_bones);
}

void MArmature::allocBones(unsigned int size)
{
	clearBones();
	if(size == 0)
		return;

	m_bones = (MOBone **) malloc(sizeof(void *)*size);
}

MOBone * MArmature::addNewBone(void)
{
	MOBone * bone = new MOBone();
	m_bones[m_bonesNumber] = bone;
	m_bonesNumber++;
	return bone;
}

MOBone * MArmature::getBoneByName(const char * name)
{
	unsigned int i;
	for(i=0; i<m_bonesNumber; i++) // scan bones
	{
		MOBone * bone = getBone(i);
		if(strcmp(name, bone->getName()) == 0)
			return bone;
	}

	return NULL;
}

bool MArmature::getBoneId(const char * boneName, unsigned int * bId)
{
	unsigned int i;

	// return if there is no bones
	if(m_bonesNumber < 1)
		return false;

	for(i=0; i<m_bonesNumber; i++)
	{
		MOBone * iBone = m_bones[i];

		if(strcmp(boneName, iBone->getName()) == 0)
		{
			*bId = i;
			return true;
		}
	}

	return false;
}

void MArmature::constructBonesInversePoseMatrix(void)
{
	unsigned int i;
	MOBone * bone = NULL;

	processBonesLinking();

	// construct inverse pose matrix
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		(*bone->getInversePoseMatrix()) = bone->getMatrix()->getInverse();
	}
}

void MArmature::updateBonesSkinMatrix(void)
{
	unsigned int i;
	MOBone * bone = NULL;
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		(*bone->getSkinMatrix()) = (*bone->getMatrix()) * (*bone->getInversePoseMatrix());
	}
}

void MArmature::processBonesLinking(void)
{
	unsigned int i;
	MOBone * bone = NULL;

	// for all root bones
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		if(! bone->hasParent())
		{
			bone->computeLocalMatrix();
			bone->processChildsLinking();
		}
	}
}