/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOSound.cpp
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
MOSound::MOSound(MSoundRef * soundRef):
	MObject3d(),
	m_soundRef(soundRef),
	m_sourceId(0)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	if(soundContext)
		soundContext->createSource(&m_sourceId, bufferId);

	setLooping(false);
	setRelative(false);
	setPitch(1.0f);
	setGain(50.0f);
	setRadius(10.0f);
	setRolloff(1.0f);
}

// copy constructor
MOSound::MOSound(const MOSound & sound):
	MObject3d(sound),
	m_soundRef(sound.m_soundRef),
	m_sourceId(0)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	if(soundContext)
		soundContext->createSource(&m_sourceId, bufferId);

	setLooping(sound.m_isLooping);
	setRelative(sound.m_isRelative);
	setPitch(sound.m_pitch);
	setGain(sound.m_gain);
	setRadius(sound.m_radius);
	setRolloff(sound.m_rolloff);
}

// destructor
MOSound::~MOSound(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	MObject3d::clearObject3d();
	if(soundContext)
		soundContext->deleteSource(&m_sourceId);
}

void MOSound::setSoundRef(MSoundRef * soundRef)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	m_soundRef = soundRef;

	if(m_soundRef && soundContext)
		soundContext->setSourceBufferId(m_sourceId, m_soundRef->getBufferId());
}

bool MOSound::isPlaying(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	if(soundContext)
		return soundContext->isSourcePlaying(m_sourceId);
	else
		return false;
}

float MOSound::getTimePos(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	if(soundContext)
		return soundContext->getSourceTimePos(m_sourceId);
	else
		return 0;
}

float MOSound::getSoundDuration(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	
	if(m_soundRef && soundContext)
		return soundContext->getBufferDuration(m_soundRef->getBufferId());
	else
		return 0.0f;
}

void MOSound::play(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();

	if(m_soundRef && soundContext)
	{
		soundContext->setSourceBufferId(m_sourceId, m_soundRef->getBufferId());
		soundContext->playSource(m_sourceId);
	}
}

void MOSound::pause(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->pauseSource(m_sourceId);
}

void MOSound::stop(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->stopSource(m_sourceId);
}

void MOSound::setLooping(bool loop)
{ 
	MSoundContext * soundContext = MEngine::getInstance()->getSoundContext();

	if(loop != m_isLooping)
	{
		if(soundContext)
		{
			if(loop)
				soundContext->enableSourceLoop(m_sourceId);
			else
				soundContext->disableSourceLoop(m_sourceId);
		}
		
		m_isLooping = loop;
	}
}

void MOSound::setRelative(bool relative)
{
	m_isRelative = relative;
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourceRelative(m_sourceId, m_isRelative);
}

void MOSound::setPitch(float pitch)
{
	m_pitch = pitch;
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourcePitch(m_sourceId, m_pitch);
}

void MOSound::setGain(float gain)
{
	m_gain = gain;
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourceGain(m_sourceId, m_gain);
}

void MOSound::setRadius(float radius)
{
	m_radius = radius;
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourceRadius(m_sourceId, m_radius);
}

void MOSound::setRolloff(float rolloff)
{
	m_rolloff = rolloff;
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourceRolloff(m_sourceId, m_rolloff);
}

void MOSound::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();
	if(soundContext)
		soundContext->setSourcePosition(m_sourceId, getTransformedPosition());
}