/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MALContext.h
//
// OpenAL Sound Context 
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


#ifndef _M_AL_CONTEXT_H
#define _M_AL_CONTEXT_H

#ifdef __APPLE__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include <MCore.h>


class MALContext : public MSoundContext
{
private:

	// sources
	map <unsigned int, unsigned int> m_sources;

	// context
	ALCdevice * m_device;
	ALCcontext * m_context;

public:

	MALContext(void);
	~MALContext(void);

	// init context
	void initContext(void);

	// buffer
	void createBuffer(unsigned int * bufferId);
	void deleteBuffer(unsigned int * bufferId);
	void sendBufferSound(unsigned int bufferId, MSound * sound);
	void sendBufferData(unsigned int bufferId, M_SOUND_FORMAT format, void * data, unsigned int size, unsigned int freq);

	float getBufferDuration(unsigned int bufferId);

	// source
	void createSource(unsigned int * sourceId, unsigned int bufferId);
	void deleteSource(unsigned int * sourceId);

	void enableSourceLoop(unsigned int sourceId);
	void disableSourceLoop(unsigned int sourceId);

	void setSourceRelative(unsigned int sourceId, bool relative);

	void setSourceBufferId(unsigned int sourceId, unsigned int bufferId);
	void setSourcePosition(unsigned int sourceId, const MVector3 & position);
	void setSourceRadius(unsigned int sourceId, float radius);
	void setSourcePitch(unsigned int sourceId, float pitch);
	void setSourceGain(unsigned int sourceId, float gain);
	void setSourceRolloff(unsigned int sourceId, float rolloff);
	void setSourceOffset(unsigned int sourceId, float offset);

	float getSourceOffset(unsigned int sourceId);

	void playSource(unsigned int sourceId);
	void pauseSource(unsigned int sourceId);
	void stopSource(unsigned int sourceId);
	bool isSourcePaused(unsigned int sourceId);
	bool isSourcePlaying(unsigned int sourceId);
	float getSourceTimePos(unsigned int sourceId);

	void queueSourceBuffer(unsigned int sourceId, unsigned int * buffers, unsigned int bufferSize);
	void unqueueSourceBuffer(unsigned int sourceId, unsigned int * buffers, unsigned int bufferSize);

	unsigned int getSourceBuffersQueued(unsigned int sourceId);
	unsigned int getSourceBuffersProcessed(unsigned int sourceId);

	// listener
	void updateListenerPosition(const MVector3 & position, const MVector3 & direction, const MVector3 & up);
};

#endif