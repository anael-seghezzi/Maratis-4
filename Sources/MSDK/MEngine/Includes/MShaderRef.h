/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MShaderRef.h
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


#ifndef _M_SHADER_REF_H
#define _M_SHADER_REF_H


// shader types
enum M_SHADER_TYPES
{
	M_SHADER_VERTEX = 0,
	M_SHADER_PIXEL
};


// Shader ref
class M_ENGINE_EXPORT MShaderRef : public MDataRef
{
private:

	// data
	M_SHADER_TYPES m_type;
	unsigned int m_shaderId;

public:

	// constructor / destructor
	MShaderRef(unsigned int shaderId, M_SHADER_TYPES type, const char * filename);
	~MShaderRef(void);

	// clear / destroy
	void clear(void);
	void destroy(void);

	// get new
	static MShaderRef * getNew(unsigned int shaderId, M_SHADER_TYPES type, const char * filename);

public:

	// update
	void update(void * arg);

	// type
	int getType(void){ return M_REF_SHADER; }

	// data
	inline void setShaderId(unsigned int shaderId){ m_shaderId = shaderId; }
	inline unsigned int getShaderId(void){ return m_shaderId; }
	inline M_SHADER_TYPES getShaderType(void){ return m_type; }
};

#endif