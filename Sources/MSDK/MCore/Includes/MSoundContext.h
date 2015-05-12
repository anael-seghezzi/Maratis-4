/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MSoundContext.h
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


#ifndef _M_SOUND_CONTEXT_H
#define _M_SOUND_CONTEXT_H

enum M_SOUND_FORMAT
{
	M_SOUND_FORMAT_MONO8 = 0,
	M_SOUND_FORMAT_MONO16,
	M_SOUND_FORMAT_STEREO8,
	M_SOUND_FORMAT_STEREO16
};


class M_CORE_EXPORT MSoundContext
{
public:

	// destructor
	virtual ~MSoundContext(void){}

	// init context
	virtual void initContext(void) = 0;

	// buffer
	virtual void createBuffer(unsigned int * bufferId) = 0;
	virtual void deleteBuffer(unsigned int * bufferId) = 0;
	virtual void sendBufferSound(unsigned int bufferId, MSound * sound) = 0;
	virtual void sendBufferData(unsigned int bufferId, M_SOUND_FORMAT format, void * data, unsigned int size, unsigned int freq) = 0;

	virtual float getBufferDuration(unsigned int bufferId) = 0;

	// source
	virtual void createSource(unsigned int * sourceId, unsigned int bufferId) = 0;
	virtual void deleteSource(unsigned int * sourceId) = 0;

	virtual void enableSourceLoop(unsigned int sourceId) = 0;
	virtual void disableSourceLoop(unsigned int sourceId) = 0;

	virtual void setSourceRelative(unsigned int sourceId, bool relative) = 0;

	virtual void setSourceBufferId(unsigned int sourceId, unsigned int bufferId) = 0;
	virtual void setSourcePosition(unsigned int sourceId, const MVector3 & position) = 0;
	virtual void setSourceRadius(unsigned int sourceId, float radius) = 0;
	virtual void setSourcePitch(unsigned int sourceId, float pitch) = 0;
	virtual void setSourceGain(unsigned int sourceId, float gain) = 0;
	virtual void setSourceRolloff(unsigned int sourceId, float rolloff) = 0;
	virtual void setSourceOffset(unsigned int sourceId, float offset) = 0;

	virtual float getSourceOffset(unsigned int sourceId) = 0;

	virtual void playSource(unsigned int sourceId) = 0;
	virtual void pauseSource(unsigned int sourceId) = 0;
	virtual void stopSource(unsigned int sourceId) = 0;
	virtual bool isSourcePaused(unsigned int sourceId) = 0;
	virtual bool isSourcePlaying(unsigned int sourceId) = 0;
	virtual float getSourceTimePos(unsigned int sourceId) = 0;

	virtual void queueSourceBuffer(unsigned int sourceId, unsigned int * buffers, unsigned int bufferSize) = 0;
	virtual void unqueueSourceBuffer(unsigned int sourceId, unsigned int * buffers, unsigned int bufferSize) = 0;

	virtual unsigned int getSourceBuffersQueued(unsigned int sourceId) = 0;
	virtual unsigned int getSourceBuffersProcessed(unsigned int sourceId) = 0;

	// listener
	virtual void updateListenerPosition(const MVector3 & position, const MVector3 & direction, const MVector3 & up) = 0;
};

#endif