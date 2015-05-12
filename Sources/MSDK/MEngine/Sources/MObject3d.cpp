/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MObject3d.cpp
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


// constructor
MObject3d::MObject3d(void):
	m_scale(1, 1, 1),
	m_isActive(true),
	m_isVisible(true),
	m_needToUpdate(true),
	m_parent(NULL)
{}

// destructor
MObject3d::~MObject3d(void)
{
	clearObject3d();
}

void MObject3d::clearObject3d(void)
{
	unsigned int i;
	unsigned int bSize = m_behaviors.size();
	for(i=0; i<bSize; i++)
		m_behaviors[i]->destroy();
	
	m_behaviors.clear();
	m_childs.clear();
}

// copy constructor
MObject3d::MObject3d(const MObject3d & object):
	m_position(object.m_position),
	m_scale(object.m_scale),
	m_rotation(object.m_rotation),
	m_matrix(object.m_matrix),
	m_isActive(object.m_isActive),
	m_isVisible(object.m_isVisible),
	m_needToUpdate(object.m_needToUpdate),
	m_parent(NULL)
{
	if(object.m_parent)
		linkTo(object.m_parent);

	unsigned int i;
	unsigned int bSize = object.m_behaviors.size();
	for(i=0; i<bSize; i++)
		addBehavior(object.m_behaviors[i]->getCopy(this));
}

void MObject3d::setName(const char * name)
{
	m_name.set(name);
}

void MObject3d::updateBehaviors(void)
{
	unsigned int i;
	unsigned int bSize = m_behaviors.size();
	for(i=0; i<bSize; i++)
		m_behaviors[i]->update();
}

void MObject3d::drawBehaviors(void)
{
	unsigned int i;
	unsigned int bSize = m_behaviors.size();
	for(i=0; i<bSize; i++)
		m_behaviors[i]->draw();
}

void MObject3d::deleteBehavior(unsigned int id)
{
	SAFE_DELETE(m_behaviors[id]);
	m_behaviors.erase(m_behaviors.begin() + id);
}

void MObject3d::invertBehavior(unsigned int idA, unsigned int idB)
{
	MBehavior * behavior = m_behaviors[idA];
	m_behaviors[idA] = m_behaviors[idB];
	m_behaviors[idB] = behavior;
}

void MObject3d::changeBehavior(unsigned int id, MBehavior * behavior)
{
	if(! behavior)
		return;

	SAFE_DELETE(m_behaviors[id]);
	m_behaviors[id] = behavior;
}

void MObject3d::removeChild(MObject3d * child)
{
	unsigned int i;
	unsigned int cSize = m_childs.size();
	for(i=0; i<cSize; i++)
	{
		if(m_childs[i] == child)
		{
			m_childs.erase(m_childs.begin() + i);
			return;
		}
	}
}

void MObject3d::unlinkChilds(void)
{
	unsigned int i;
	unsigned int cSize = m_childs.size();
	for(i=0; i<cSize; i++)
		m_childs[i]->setParent(NULL);
	m_childs.clear();
}

void MObject3d::unLink(void)
{
	if(! getParent())
		return;
	
	// remove child from actual parent
	getParent()->removeChild(this);
	m_parent = NULL;
}

void MObject3d::linkTo(MObject3d * parent)
{
	if(! parent)
		return;

	if(parent == this)
		return;

	if(! getParent()) // if not already linked
	{
		m_parent = parent;
		parent->addChild(this);
		return;
	}

	// if already linked
	if(getParent() == parent)
		return;

	// remove child from actual parent
	getParent()->removeChild(this);
	m_parent = parent;
	parent->addChild(this);
}

void MObject3d::update(void)
{}

void MObject3d::updateMatrix(void)
{
	computeLocalMatrix();

	if(hasParent()){
		m_matrix = m_parent->m_matrix * m_matrix;
	}
}

void MObject3d::computeLocalMatrix(void)
{
	m_matrix.setRotationAxis(m_rotation.getAngle(), m_rotation.getAxis());
	m_matrix.setTranslationPart(m_position);
	m_matrix.scale(m_scale);
}

void MObject3d::computeChildsMatrices(void)
{
	unsigned int i;
	unsigned int childSize = m_childs.size();
	MObject3d * childObject = NULL;

	for(i=0; i<childSize; i++) // for all childs
	{
		childObject = m_childs[i];

		// compute parenting (parent matrix * child local matrix)
		if(m_needToUpdate || childObject->needToUpdate())
		{
			childObject->computeLocalMatrix();
			childObject->m_matrix = m_matrix * childObject->m_matrix;
			childObject->m_needToUpdate = true;
		}
		childObject->computeChildsMatrices();
	}

	m_needToUpdate = false;
}

MVector3 MObject3d::getUniformRotatedVector(const MVector3 & vector)
{
	float L = vector.getLength();
	return getRotatedVector(vector).getNormalized() * L;
}

void MObject3d::setPosition(const MVector3 & position)
{
	if(position != m_position)
	{
		m_position = position;
		m_needToUpdate = true;
	}
}

void MObject3d::setEulerRotation(const MVector3 & euler)
{
	MQuaternion rotation = MQuaternion(euler.x, euler.y, euler.z);
	if(rotation != m_rotation)
	{
		m_rotation = rotation;
		m_needToUpdate = true;
	}
}

void MObject3d::setAxisAngleRotation(const MVector3 & axis, const float angle)
{
	MQuaternion rotation = MQuaternion(angle, axis);
	if(rotation != m_rotation)
	{
		m_rotation = rotation;
		m_needToUpdate = true;
	}
}

void MObject3d::addAxisAngleRotation(const MVector3 & axis, const float angle)
{
	m_rotation *= MQuaternion(angle, axis);
	m_rotation.normalize();
	m_needToUpdate = true;
}

void MObject3d::setRotation(const MQuaternion & rotation)
{
	if(rotation != m_rotation)
	{
		m_rotation = rotation;
		m_needToUpdate = true;
	}
}

void MObject3d::setScale(const MVector3 & scale)
{
	if(scale != m_scale)
	{
		m_scale = scale;
		m_needToUpdate = true;
	}
}