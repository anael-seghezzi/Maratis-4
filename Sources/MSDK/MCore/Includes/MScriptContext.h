/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MScriptContext.h
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


#ifndef _M_SCRIPT_CONTEXT_H
#define _M_SCRIPT_CONTEXT_H


class M_CORE_EXPORT MScriptContext
{
public :

	// destructor
	virtual ~MScriptContext(void){}

	// run script
	virtual void runScript(const char * filename) = 0;

	// call function
	virtual bool startCallFunction(const char * name) = 0;
	virtual bool endCallFunction(int numArgs = 0) = 0;
	virtual void callFunction(const char * name) = 0;

	// add function
	virtual void addFunction(const char * name, int (*function)(void)) = 0;

	// handle variables
	virtual unsigned int getArgsNumber(void) = 0;

	virtual void getIntArray(unsigned int arg, int * values, unsigned int valuesNumber) = 0;
	virtual void getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber) = 0;
	virtual void * getPointer(unsigned int arg) = 0;
	virtual int getInteger(unsigned int arg) = 0;
	virtual float getFloat(unsigned int arg) = 0;
	virtual const char * getString(unsigned int arg) = 0;

	virtual void pushIntArray(const int * values, unsigned int valuesNumber) = 0;
	virtual void pushFloatArray(const float * values, unsigned int valuesNumber) = 0;
	virtual void pushString(const char * string) = 0;
	virtual void pushBoolean(bool value) = 0;
	virtual void pushInteger(int value) = 0;
	virtual void pushFloat(float value) = 0;
	virtual void pushPointer(void * value) = 0;
};

#endif
