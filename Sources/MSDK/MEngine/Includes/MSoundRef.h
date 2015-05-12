/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MSoundRef.h
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


#ifndef _M_SOUND_REF_H
#define _M_SOUND_REF_H


// Sound ref
class M_ENGINE_EXPORT MSoundRef : public MDataRef
{
private:

	// data
	unsigned int m_bufferId;

public:

	// constructor / destructor
	MSoundRef(unsigned int bufferId, const char * filename);
	~MSoundRef(void);

	// clear / destroy
	void clear(void);
	void destroy(void);

	// get new
	static MSoundRef * getNew(unsigned int bufferId, const char * filename);

public:

	// update
	void update(void * arg=NULL);

	// type
	int getType(void){ return M_REF_SOUND; }

	// data
	inline void setBufferId(unsigned int bufferId){ m_bufferId = bufferId; }
	inline unsigned int getBufferId(void){ return m_bufferId; }
};

#endif