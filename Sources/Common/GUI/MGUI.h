/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MGUI.h
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


#ifndef _GUI_H
#define _GUI_H


#define MGUI_EVENT_CALLBACK void (* eventCallback)(MWindow * rootWindow, MWIN_EVENT_TYPE event)

bool				MGUI_init(void);
MWindow *			MGUI_createWindow(const char * title, int x, int y, unsigned int width, unsigned int height, MGUI_EVENT_CALLBACK = NULL);
MWindow *			MGUI_getWindow(unsigned int id);
unsigned int		MGUI_getWindowsNumber(void);
void				MGUI_closeWindow(MWindow * window);
void				MGUI_pauseWindow(MWindow * window);
void				MGUI_unpauseWindow(MWindow * window);
void				MGUI_closeAllWindows(void);
void				MGUI_pauseAllWindows(void);
void				MGUI_unpauseAllWindows(void);
bool				MGUI_update(void);
bool				MGUI_isFocused(void);
void				MGUI_close(void);
double				MGUI_getTime(void);
void				MGUI_setCurrentDirectory(const char * path);


// context
class MGUIContext : public MSystemContext
{
public:

	~MGUIContext(void){}
	void getScreenSize(unsigned int * width, unsigned int * height){}
	void setCursorPosition(int x, int y){}
	void hideCursor(void){}
	void showCursor(void){}
	const char * getWorkingDirectory(void){ return ""; }
	unsigned long getSystemTick(void){ return (unsigned long)(MGUI_getTime()*1000); }
};

#endif
