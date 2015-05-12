/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MLevel.cpp
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


#include <stdio.h>
#include "../Includes/MEngine.h"


// level
MLevel::MLevel(void):
m_currentSceneId(0),
m_requestedSceneId(0xFFFFFFFF)
{}

MLevel::~MLevel(void)
{
	clear();
}

void MLevel::incrDataRefScore(MDataRef * ref)
{
	if(! ref)
		return;

	if(ref->getScore() == 0)
		sendToUpdateQueue(ref);

	ref->incrScore();
}

void MLevel::decrDataRefScore(MDataRef * ref)
{
	if(! ref)
		return;

	if(ref->getScore() == 1)
		sendToClearQueue(ref);

	ref->decrScore();
}

MFontRef * MLevel::loadFont(const char * filename, const bool preload)
{
	MFontRef * ref = (MFontRef *)m_fontManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MFontRef::getNew(NULL, filename);
		m_fontManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}
	
	return ref;
}

MMeshRef * MLevel::loadMesh(const char * filename, const bool preload)
{
	MMeshRef * ref = (MMeshRef *)m_meshManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MMeshRef::getNew(NULL, filename);
		m_meshManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update(this);
		ref->incrScore();
	}

	return ref;
}

MArmatureAnimRef * MLevel::loadArmatureAnim(const char * filename, const bool preload)
{
	MArmatureAnimRef * ref = (MArmatureAnimRef *)m_armatureAnimManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MArmatureAnimRef::getNew(NULL, filename);
		m_armatureAnimManager.addRef(ref);
	}

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MTexturesAnimRef * MLevel::loadTexturesAnim(const char * filename, const bool preload)
{
	MTexturesAnimRef * ref = (MTexturesAnimRef *)m_texturesAnimManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MTexturesAnimRef::getNew(NULL, filename);
		m_texturesAnimManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MMaterialsAnimRef * MLevel::loadMaterialsAnim(const char * filename, const bool preload)
{
	MMaterialsAnimRef * ref = (MMaterialsAnimRef *)m_materialsAnimManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MMaterialsAnimRef::getNew(NULL, filename);
		m_materialsAnimManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MSoundRef * MLevel::loadSound(const char * filename, const bool preload)
{
	MSoundRef * ref = (MSoundRef *)m_soundManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MSoundRef::getNew(0, filename);
		m_soundManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MTextureRef * MLevel::loadTexture(const char * filename, const bool mipmap, const bool preload)
{
	MTextureRef * ref = (MTextureRef *)m_textureManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MTextureRef::getNew(0, filename, mipmap);
		m_textureManager.addRef(ref);
	}

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MShaderRef * MLevel::loadShader(const char * filename, M_SHADER_TYPES type, const bool preload)
{
	MShaderRef * ref = (MShaderRef *)m_shaderManager.getRefFromFilename(filename);
	if(! ref)
	{
		ref = MShaderRef::getNew(0, type, filename);
		m_shaderManager.addRef(ref);
	}
	
	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update(this);
		ref->incrScore();
	}
	
	return ref;
}

MFXRef * MLevel::createFX(MShaderRef * vertexShaderRef, MShaderRef * pixelShaderRef)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	unsigned int i;
	unsigned int size = m_FXManager.getFXRefsNumber();
	for(i=0; i<size; i++)
	{
		MFXRef * FXRef = (MFXRef *)m_FXManager.getFXRef(i);
		if((FXRef->getVertexShaderRef() == vertexShaderRef) && (FXRef->getPixelShaderRef() == pixelShaderRef))
			return FXRef;
	}

	unsigned int FXId;
	render->createFX(&FXId, vertexShaderRef->getShaderId(), pixelShaderRef->getShaderId());
	return m_FXManager.addFXRef(FXId, vertexShaderRef, pixelShaderRef);
}

void MLevel::sendToUpdateQueue(MDataRef * ref)
{
	m_updateQueue.push_back(ref);
}

void MLevel::sendToClearQueue(MDataRef * ref)
{
	m_clearQueue.push_back(ref);
}

void MLevel::updateQueueDatas(void)
{
	unsigned int i, size = m_updateQueue.size();
	for(i=0; i<size; i++)
	{
		MDataRef * ref = m_updateQueue[i];
		ref->update();
	}

	m_updateQueue.clear();
}

void MLevel::clearQueueDatas(void)
{
	unsigned int i, size = m_clearQueue.size();
	for(i=0; i<size; i++)
	{
		MDataRef * ref = m_clearQueue[i];
		ref->clear();
	}

	m_clearQueue.clear();
}

MScene * MLevel::addNewScene(void)
{
	MScene * scene = new MScene();
	m_scenes.push_back(scene);
	return scene;
}

void MLevel::clear(void)
{
	clearScenes();

	m_updateQueue.clear();
	m_clearQueue.clear();

	m_FXManager.clear();
	m_armatureAnimManager.clear();
	m_textureManager.clear();
	m_shaderManager.clear();
	m_soundManager.clear();
	m_meshManager.clear();
	m_fontManager.clear();
	m_armatureAnimManager.clear();
	m_texturesAnimManager.clear();
	m_materialsAnimManager.clear();
}

void MLevel::clearScenes(void)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
		SAFE_DELETE(m_scenes[i]);

	m_currentSceneId = 0;
	m_scenes.clear();
}

void MLevel::setCurrentSceneId(unsigned int id)
{
	if(id == m_currentSceneId)
	{
		updateQueueDatas();
		return;
	}

	// previous scene
	MScene * scene = getCurrentScene();
	if(scene->getDataMode() == M_DATA_DYNAMIC || scene->getDataMode() == M_DATA_STREAM)
	{
		// meshs
		unsigned int i;

		unsigned int eSize = scene->getEntitiesNumber();
		for(i=0; i<eSize; i++)
		{
			MOEntity * entity = scene->getEntityByIndex(i);
			MMesh * mesh = entity->getMesh();
			if(mesh)
			{
				// textures
				unsigned int t;
				unsigned int tSize = mesh->getTexturesNumber();
				for(t=0; t<tSize; t++)
				{
					MTextureRef * ref = mesh->getTexture(t)->getTextureRef();
					decrDataRefScore(ref);
				}

				// armature anim
				{
					MArmatureAnimRef * ref = mesh->getArmatureAnimRef();
					decrDataRefScore(ref);
				}

				// textures anim
				{
					MTexturesAnimRef * ref = mesh->getTexturesAnimRef();
					decrDataRefScore(ref);
				}

				// materials anim
				{
					MMaterialsAnimRef * ref = mesh->getMaterialsAnimRef();
					decrDataRefScore(ref);
				}
			}

			// mesh
			MMeshRef * ref = entity->getMeshRef();
			decrDataRefScore(ref);

		}

		// sounds
		unsigned int sSize = scene->getSoundsNumber();
		for(i=0; i<sSize; i++)
		{
			MOSound * sound = scene->getSoundByIndex(i);
			MSoundRef * ref = sound->getSoundRef();
			decrDataRefScore(ref);
		}

		// clear queue data
		clearQueueDatas();
	}

	// change current id
	m_currentSceneId = id;

	// next scene
	scene = getCurrentScene();
	if(scene->getDataMode() == M_DATA_DYNAMIC || scene->getDataMode() == M_DATA_STREAM)
	{
		// meshs
		unsigned int i;

		unsigned int eSize = scene->getEntitiesNumber();
		for(i=0; i<eSize; i++)
		{
			MOEntity * entity = scene->getEntityByIndex(i);
			MMesh * mesh = entity->getMesh();
			if(mesh)
			{
				// textures
				unsigned int t;
				unsigned int tSize = mesh->getTexturesNumber();
				for(t=0; t<tSize; t++)
				{
					MTextureRef * ref = mesh->getTexture(t)->getTextureRef();
					incrDataRefScore(ref);
				}

				// armature anim
				{
					MArmatureAnimRef * ref = mesh->getArmatureAnimRef();
					incrDataRefScore(ref);
				}

				// textures anim
				{
					MTexturesAnimRef * ref = mesh->getTexturesAnimRef();
					incrDataRefScore(ref);
				}

				// materials anim
				{
					MMaterialsAnimRef * ref = mesh->getMaterialsAnimRef();
					incrDataRefScore(ref);
				}
			}

			// mesh
			MMeshRef * ref = entity->getMeshRef();
			incrDataRefScore(ref);
		}

		// sounds
		unsigned int sSize = scene->getSoundsNumber();
		for(i=0; i<sSize; i++)
		{
			MOSound * sound = scene->getSoundByIndex(i);
			MSoundRef * ref = sound->getSoundRef();
			incrDataRefScore(ref);
		}
	}

	// updateQueueData
	updateQueueDatas();
}

void MLevel::changeCurrentScene(unsigned int id)
{
	m_requestedSceneId = id;
}

void MLevel::changeCurrentSceneIfRequested()
{
	if(m_requestedSceneId == 0xFFFFFFFF)
		return;

	if(m_requestedSceneId != m_currentSceneId)
	{
		MEngine * engine = MEngine::getInstance();

		if(m_requestedSceneId < getScenesNumber())
		{
			MScene * scene = getCurrentScene();

			// onEndScene
			MGame * game = engine->getGame();
			if(game)
				game->onEndScene();

			// change scene
			setCurrentSceneId(m_requestedSceneId);
			scene = getCurrentScene();

			// onBeginScene
			if(game)
				game->onBeginScene();
		}
	}

	m_requestedSceneId = 0xFFFFFFFF; // reset requested scene
}

void MLevel::deleteScene(unsigned int id)
{
	SAFE_DELETE(m_scenes[id]);
	m_scenes.erase(m_scenes.begin() + id);

	unsigned int sSize = getScenesNumber();
	if(sSize == 0)
	{
		m_currentSceneId = 0;
		return;
	}

	unsigned int maxId = (unsigned int)((int)getScenesNumber() - 1);
	if(m_currentSceneId > maxId)
		m_currentSceneId = maxId;
}

MScene * MLevel::getCurrentScene(void)
{
	unsigned int sSize = getScenesNumber();
	if(m_currentSceneId < sSize)
		return m_scenes[m_currentSceneId];

	return NULL;
}

MScene * MLevel::getSceneByName(const char * name)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
	{
		if(! strcmp(name, m_scenes[i]->getName()))
			return m_scenes[i];
	}

	return NULL;
}

bool MLevel::getSceneIndexByName(const char * name, unsigned int * index)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
	{
		if(! strcmp(name, m_scenes[i]->getName()))
		{
			(*index) = i;
			return true;
		}
	}

	return false;
}
