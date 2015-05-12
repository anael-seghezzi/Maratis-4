/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBFollow.cpp
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


#include <MEngine.h>
#include "MBFollow.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MBFollow::MBFollow(MObject3d * parentObject):
MBehavior(parentObject),
m_local(false),
m_delay(10),
m_targetName("none")
{}

MBFollow::MBFollow(MBFollow & behavior, MObject3d * parentObject):
MBehavior(parentObject),
m_local(behavior.m_local),
m_delay(behavior.m_delay),
m_offset(behavior.m_offset),
m_targetName(behavior.m_targetName)
{}

MBFollow::~MBFollow(void)
{}

void MBFollow::destroy(void)
{
	delete this;
}

MBehavior * MBFollow::getNew(MObject3d * parentObject)
{
	return new MBFollow(parentObject);
}

MBehavior * MBFollow::getCopy(MObject3d * parentObject)
{
	return new MBFollow(*this, parentObject);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int MBFollow::getVariablesNumber(void){
	return 4;
}

MVariable MBFollow::getVariable(unsigned int id)
{
	switch(id)
	{
	default:
		return MVariable("NULL", NULL, M_VARIABLE_NULL);
	case 0:
		return MVariable("target", &m_targetName, M_VARIABLE_STRING);
	case 1:
		return MVariable("delay", &m_delay, M_VARIABLE_FLOAT);
	case 2:
		return MVariable("offset", &m_offset, M_VARIABLE_VEC3);
	case 3:
		return MVariable("local", &m_local, M_VARIABLE_BOOL);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void MBFollow::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MGame * game = engine->getGame();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();

	MObject3d * parent = getParentObject();

	const char * targetName = m_targetName.getSafeString();
	if(strcmp(targetName, "none") == 0)
		return;

	// target object
	MObject3d * object = scene->getObjectByName(targetName);
	if(! object)
		return;

	// targetPos
	MVector3 offset = m_offset;
	if(m_local)
		offset = object->getRotatedVector(offset);

	float delay = MAX(1, m_delay);
	MVector3 direction = (object->getTransformedPosition() + offset) - parent->getPosition();

	if(parent->getType() == M_OBJECT3D_ENTITY)
	{
		MOEntity * entity = (MOEntity *)parent;
		MPhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(phyProps)
		{
			if(! phyProps->isGhost())
			{
				if(game->isRunning())
				{
					MPhysicsContext * physics = engine->getPhysicsContext();
					physics->addCentralForce(phyProps->getCollisionObjectId(), (direction/delay)*phyProps->getMass());
				}
				return;
			}
		}
	}

	MVector3 position = parent->getPosition() + (direction / delay);
	parent->setPosition(position);
	parent->updateMatrix();
}