/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MVariable.h
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


#ifndef _M_VARIABLE_H
#define _M_VARIABLE_H


enum M_VARIABLE_TYPE
{
	M_VARIABLE_NULL = 0,
	M_VARIABLE_BOOL,
	M_VARIABLE_INT,
	M_VARIABLE_UINT,
	M_VARIABLE_FLOAT,
	M_VARIABLE_STRING,
	M_VARIABLE_VEC2,
	M_VARIABLE_VEC3,
	M_VARIABLE_VEC4,
	M_VARIABLE_QUAT,
	M_VARIABLE_DATA_REF
};

class M_ENGINE_EXPORT MVariable
{
private:

	M_VARIABLE_TYPE m_type;

	const char * m_name;
	void * m_pointer;

public:
	
	MVariable(const char * name, void * pointer, M_VARIABLE_TYPE type):
	m_type(type),
	m_name(name),
	m_pointer(pointer)
	{}

public:

	inline M_VARIABLE_TYPE getType(void){ return m_type; }
	inline const char * getName(void){ return m_name; }
	inline void * getPointer(void){ return m_pointer; }
};


#endif