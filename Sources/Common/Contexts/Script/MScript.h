/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MScript.h
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


#ifndef _M_SCRIPT_H
#define _M_SCRIPT_H

#include <MEngine.h>
#include <lua.hpp>


class MScript : public MScriptContext
{
public :

	MScript(void);
	~MScript(void);

private:

	void init();
	void clear();

	static int function(lua_State * L);

private :

	bool m_isRunning;
	lua_State * m_state;

	map<string, int (*)(void)> m_functions;

public:

	// run script
	void runScript(const char * filename);

	// call function
	bool startCallFunction(const char* name);
	bool endCallFunction(int numArgs = 0);
	void callFunction(const char * name);

	// add function
	void addFunction(const char * name, int (*function)(void));

	// variables
	unsigned int getArgsNumber(void);

	void getIntArray(unsigned int arg, int * values, unsigned int valuesNumber);
	void getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber);
	const char * getString(unsigned int arg);
	int getInteger(unsigned int arg);
	float getFloat(unsigned int arg);
	void* getPointer(unsigned int arg);

	void pushIntArray(const int * values, unsigned int valuesNumber);
	void pushFloatArray(const float * values, unsigned int valuesNumber);
	void pushString(const char * string);
	void pushBoolean(bool value);
	void pushInteger(int value);
	void pushFloat(float value);
	void pushPointer(void* value);
};

#endif
