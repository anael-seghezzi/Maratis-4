/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MEngine.cpp
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


MEngine::MEngine(void):
m_isActive(true),
m_soundContext(NULL),
m_renderingContext(NULL),
m_physicsContext(NULL),
m_scriptContext(NULL),
m_inputContext(NULL),
m_systemContext(NULL),
m_packageManager(NULL),
m_level(NULL),
m_game(NULL),
m_renderer(NULL),
m_requestedLevelToLoad(NULL)
{}

MEngine::~MEngine(void)
{
	SAFE_FREE(m_requestedLevelToLoad);
}

MEngine * MEngine::getInstance(void)
{
	static MEngine m_instance;
	return &m_instance;
}

void MEngine::setRenderingContext(MRenderingContext * renderingContext)
{
	m_renderingContext = renderingContext;
}

void MEngine::setSoundContext(MSoundContext * soundContext)
{
	m_soundContext = soundContext;
}

void MEngine::setPhysicsContext(MPhysicsContext * physicsContext)
{
	m_physicsContext = physicsContext;
}

void MEngine::setScriptContext(MScriptContext * scriptContext)
{
	m_scriptContext = scriptContext;
}

void MEngine::setInputContext(MInputContext * inputContext)
{
	m_inputContext = inputContext;
}

void MEngine::setSystemContext(MSystemContext * systemContext)
{
	m_systemContext = systemContext;
}

void MEngine::setPackageManager(MPackageManager * packageManager)
{
	m_packageManager = packageManager;
}

void MEngine::setLevel(MLevel * level)
{
	m_level = level;
}

void MEngine::updateRequests(void)
{
	loadLevelIfRequested();
	if(m_level)
		m_level->changeCurrentSceneIfRequested();
}

bool MEngine::loadLevel(const char * filename)
{
	if(! filename)
		return false;

	if(! m_level)
		return false;

	char globalFilename[256];
	getGlobalFilename(globalFilename, m_systemContext->getWorkingDirectory(), filename);

	// Check if there is a level under the given name
	if(! isFileExist(globalFilename))
		return false;

	if(m_game)
	{
		if(m_game->isRunning())
		{
			m_game->onEndScene();
			m_game->onEndLevel();
		}
	}

	if(m_levelLoader.loadData(globalFilename, m_level))
	{
		if(m_game){
			if(m_game->isRunning())
			{
				m_game->onBeginLevel();
				m_game->onBeginScene();
			}
		}
		return true;
	}

	return false;
}

void MEngine::requestLoadLevel(const char * filename)
{
	SAFE_FREE(m_requestedLevelToLoad);

	unsigned int len = strlen(filename);
	m_requestedLevelToLoad = (char*)malloc(len + 1);
	strcpy(m_requestedLevelToLoad, filename);
}

bool MEngine::doesLevelExist(const char * filename)
{
	if(! filename)
		return false;

	char globalFilename[256];
	getGlobalFilename(globalFilename, m_systemContext->getWorkingDirectory(), filename);
	return isFileExist(globalFilename);
}

void MEngine::setGame(MGame * game)
{
	m_game = game;
}

void MEngine::setRenderer(MRenderer * renderer)
{
	m_renderer = renderer;
}

void MEngine::loadLevelIfRequested()
{
	if(m_requestedLevelToLoad == NULL)
		return;

	loadLevel(m_requestedLevelToLoad);
	SAFE_FREE(m_requestedLevelToLoad);
}
