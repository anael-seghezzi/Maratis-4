/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MSystemContext.h
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


#ifndef _M_SYSTEM_CONTEXT_H
#define _M_SYSTEM_CONTEXT_H


class M_CORE_EXPORT MSystemContext
{
public:

	// destructor
	virtual ~MSystemContext(void){}

	// screen
	virtual void getScreenSize(unsigned int * width, unsigned int * height) = 0;

	// cursor
	virtual void setCursorPosition(int x, int y){};
	virtual void hideCursor(void){};
	virtual void showCursor(void){};
	
	// working directory
	virtual const char * getWorkingDirectory(void) = 0;

	// system tick
	virtual unsigned long getSystemTick(void) = 0;
};

#endif