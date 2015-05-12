/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MDataManager.h
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


#ifndef _M_DATA_MANAGER_H
#define _M_DATA_MANAGER_H


// Data ref
class M_CORE_EXPORT MDataRef
{
protected:

	long m_time;
	MString m_filename;
	unsigned int m_score;

public:

	// constructor / destructor
	MDataRef(void);
	virtual ~MDataRef(void){}

	// clear / destroy
	virtual void clear(void) = 0;
	virtual void destroy(void) = 0;

public:

	// score
	void incrScore(void);
	void decrScore(void);
	inline unsigned int getScore(void){ return m_score; }

	// update
	virtual void update(void * arg=NULL) = 0;

	// type
	virtual int getType(void) = 0;

	// time
	inline void setTime(long time){ m_time = time; }
	inline long getTime(void){ return m_time; }

	// filename
	inline void setFilename(const char * filename){ m_filename = filename; }
	inline const char * getFilename(void){ return m_filename.getData(); }
};


// Data manager
class M_CORE_EXPORT MDataManager
{
private:

	// refs
	vector <MDataRef *> m_refs;

public:

	// destructor
	virtual ~MDataManager(void);

public:

	// clear
	virtual void clear(void);

	// refs
	void addRef(MDataRef * ref);
	MDataRef * getRefFromFilename(const char * filename);
	inline MDataRef * getRef(unsigned int id){ return m_refs[id]; }
	inline unsigned int getRefsNumber(void){ return m_refs.size(); }
};

#endif
