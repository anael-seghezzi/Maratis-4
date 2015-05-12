/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MSelectionManager.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#include "MEditor.h"


void MSelectionManager::selectAll(void)
{
	//autoSave();

	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	bool allSelect = (getSelectionSize() == scene->getObjectsNumber());
	clearSelection();

	if(! allSelect)
	{
		unsigned int i;
		unsigned int size = scene->getObjectsNumber();
		for(i=0; i<size; i++)
			select(scene->getObjectByIndex(i));
	}
	else
	{
		//UI->editObject(NULL);
	}
}

void MSelectionManager::select(MObject3d * object)
{
	unsigned int i, size = getSelectionSize();
	for(i=0; i<size; i++)
	{
		if(m_selection[i] == object) // already selected
		{
			// deselect
			m_selection.erase(m_selection.begin() + i);
			return;
		}
	}

	// select
	m_selection.push_back(object);
}

void MSelectionManager::deselect(MObject3d * object)
{
	unsigned int i, size = getSelectionSize();
	for(i=0; i<size; i++)
	{
		if(m_selection[i] == object) // selected
		{
			// deselect
			m_selection.erase(m_selection.begin() + i);
			return;
		}
	}
}

void MSelectionManager::selectSameMesh(void)
{
	//autoSave();

	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	vector <MMesh *> meshs;

	unsigned int i, selSize = getSelectionSize();
	for(i=0; i<selSize; i++)
	{
		MObject3d * object = getSelectedObject(i);
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity *)object;
			MMesh * mesh = entity->getMesh();
			if(mesh)
				meshs.push_back(mesh);
		}
	}

	unsigned int meshsSize = meshs.size();
	if(meshsSize == 0)
		return;

	clearSelection();

	unsigned int j, size = scene->getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();
		for(j=0; j<meshsSize; j++){
			if(meshs[j] == mesh){
				select(entity);
				break;
			}
		}
	}
}

void MSelectionManager::clearSelection(void)
{
	m_selection.clear();
}

void MSelectionManager::activateSelection(void)
{
	//autoSave();

	unsigned int i;
	unsigned int selSize = getSelectionSize();
	for(i=0; i<selSize; i++)
	{
		MObject3d * object = m_selection[i];
		object->setActive(! object->isActive());
	}
}

void MSelectionManager::deleteSelection(void)
{
	//autoSave();

	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	unsigned int i;
	unsigned int oSize = getSelectionSize();
	for(i=0; i<oSize; i++)
		scene->deleteObject(m_selection[i]);

	m_selection.clear();
}

void MSelectionManager::duplicateSelection(void)
{
	//autoSave();

	//MaratisUI * UI = MaratisUI::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	// get current objects number
	unsigned int objStart = scene->getObjectsNumber();
	unsigned int i;
	unsigned int oSize = getSelectionSize();
	if(oSize == 0)
		return;

	// duplicate
	MOLight * light;
	MOEntity * entity;
	MOCamera * camera;
	MOSound * sound;
	MOText * text;
	MObject3d * object;
	for(i=0; i<oSize; i++)
	{
		object = m_selection[i];
		switch(object->getType())
		{
            case M_OBJECT3D_ENTITY:
			{
				entity = scene->addNewEntity(*(MOEntity *)object);
				entity->setName(scene->getNewObjectName("Entity").getSafeString());
				break;
			}
            case M_OBJECT3D_LIGHT:
			{
				light = scene->addNewLight(*(MOLight *)object);
				light->setName(scene->getNewObjectName("Light").getSafeString());
				break;
			}
            case M_OBJECT3D_CAMERA:
			{
				camera = scene->addNewCamera(*(MOCamera *)object);
				camera->setName(scene->getNewObjectName("Camera").getSafeString());
				break;
			}
            case M_OBJECT3D_SOUND:
			{
				sound = scene->addNewSound(*(MOSound *)object);
				sound->setName(scene->getNewObjectName("Sound").getSafeString());
				break;
			}
            case M_OBJECT3D_TEXT:
			{
				text = scene->addNewText(*(MOText *)object);
				text->setName(scene->getNewObjectName("Text").getSafeString());
				 break;
			}
		}
	}

	// select duplicated objects
	{
		clearSelection();
	
		oSize = scene->getObjectsNumber();
		for(i=objStart; i<oSize; i++)
		{
			object = scene->getObjectByIndex(i);
			select(object);
		}
	}
	
	//UI->editObject(object);
	//UI->updateViewMenu();
}

void MSelectionManager::updateSelectionCenter(void)
{	
	MVector3 position(0, 0, 0);

	unsigned int i, size = getSelectionSize();
	for(i=0; i<size; i++)
	{
		MObject3d * object = m_selection[i];
		position += object->getTransformedPosition();
	}

	if(size > 0)
		m_selectionCenter = position / (float)size;
}

bool MSelectionManager::isObjectSelected(MObject3d * object)
{
	unsigned int i;
	unsigned int oSize = getSelectionSize();
	for(i=0; i<oSize; i++)
	{
		if(m_selection[i] == object)
			return true;
	}

	return false;
}

unsigned int MSelectionManager::getSelectionSize(void)
{
	return m_selection.size();
}

MObject3d * MSelectionManager::getSelectedObject(unsigned int id)
{
	return m_selection[id];
}