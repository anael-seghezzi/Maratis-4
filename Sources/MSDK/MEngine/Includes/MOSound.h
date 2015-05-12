/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOSound.h
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


#ifndef _M_OSOUND_SOURCE_H
#define _M_OSOUND_SOURCE_H


class M_ENGINE_EXPORT MOSound : public MObject3d
{
public:

	// constructor / destructor
	MOSound(MSoundRef * soundRef);
	virtual ~MOSound(void);

	// copy constructor
	MOSound(const MOSound & sound);

private:

	// sound ref
	MSoundRef * m_soundRef;

	// source
	unsigned int m_sourceId;

	// properties
	bool m_isLooping;
	bool m_isRelative;

	float m_pitch;
	float m_gain;
	float m_radius;
	float m_rolloff;

public:

	// type
	int getType(void){ return M_OBJECT3D_SOUND; }

	// sound ref
	void setSoundRef(MSoundRef * soundRef);
	inline MSoundRef * getSoundRef(void){ return m_soundRef; }

	// source
	inline unsigned int getSourceId(void){ return m_sourceId; }

	// properties
	void setLooping(bool loop);
	void setRelative(bool relative);
	void setPitch(float pitch);
	void setGain(float gain);
	void setRadius(float radius);
	void setRolloff(float rolloff);

	bool isPlaying(void);
	inline bool isLooping(void){ return m_isLooping; }
	inline bool isRelative(void){ return m_isRelative; }

	inline float getPitch(void){ return m_pitch; }
	inline float getGain(void){ return m_gain; }
	inline float getRadius(void){ return m_radius; }
	inline float getRolloff(void){ return m_rolloff; }

	float getTimePos(void);
	float getSoundDuration(void);

	// control
	void play(void);
	void pause(void);
	void stop(void);

	// update
	void update(void);
};

#endif