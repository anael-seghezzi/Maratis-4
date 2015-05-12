/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWindow.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2013 Anael Seghezzi <www.maratis3d.com>
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


#ifndef _M_WINDOW_H
#define _M_WINDOW_H

#define MWIN_MAX_KEYS 256
#define MWIN_MAX_MOUSE_BUTTONS 32


class M_GUI_EXPORT MWindow
{
public:

	MWindow(int x, int y, unsigned int width, unsigned int height);
	virtual ~MWindow(void);

protected:

	// pos / scale
	int m_pos[2];
	unsigned int m_width;
	unsigned int m_height;

	// keys
	bool m_keys[MWIN_MAX_KEYS];
	unsigned int m_currentKey;

	// mouse
	bool m_mouseButtons[MWIN_MAX_MOUSE_BUTTONS];
	unsigned int m_currentMouseButton;
	MVector2 m_mousePos;
	MVector2 m_mouseDir;
	MVector2 m_mouseScroll;
	
	// local windows
	vector <MGuiWindow *> m_windows;

	// pointer event
	void (* m_eventCallback)(MWindow * rootWindow, MWIN_EVENT_TYPE event);

	// draw callback
	void (* m_drawCallback)(MWindow * rootWindow);

	// events
	void onEvent(MWIN_EVENT_TYPE event);

	// user pointer
	void * m_userPointer;
	
public:

	// clear
	void clear(void);
	
	// pos / scale
	inline int getXPosition(void){ return m_pos[0]; }
	inline int getYPosition(void){ return m_pos[1]; }
	inline unsigned int getWidth(void){ return m_width; }
	inline unsigned int getHeight(void){ return m_height; }
	
	// keys
	inline unsigned int getKey(void){ return m_currentKey; }
	inline bool isKeyPressed(unsigned int id){ return m_keys[id]; }

	// mouse
	inline unsigned int getMouseButton(void){ return m_currentMouseButton; }
	inline bool isMouseButtonPressed(unsigned int id){ return m_mouseButtons[id]; }
	inline MVector2 getMousePosition(void) const { return m_mousePos; }
	inline MVector2 getMouseScroll(void) const { return m_mouseScroll; }
	inline MVector2 getMouseDir(void) const { return m_mouseDir; }
	
	// windows
	void unHighLightAllWindows(void);
	bool isSomethingEditing(void);
	
	MGuiWindow * addNewWindow(void);
	inline unsigned int getWindowsNumber(void){ return m_windows.size(); }
	inline MGuiWindow * getWindow(unsigned int id){ return m_windows[id]; }

	// events
	inline void onChar(unsigned int character){ m_currentKey = character; onEvent(MWIN_EVENT_CHAR); }
	inline void onKeyDown(unsigned int key){ m_keys[key] = true;  m_currentKey = key; onEvent(MWIN_EVENT_KEY_DOWN); }
	inline void onKeyUp(unsigned int key)  { m_keys[key] = false; m_currentKey = key; onEvent(MWIN_EVENT_KEY_UP); }
	
	inline void onCreate(void){ onEvent(MWIN_EVENT_CREATE); }
	inline void onMove(int x, int y){ m_pos[0] = x; m_pos[1] = y; onEvent(MWIN_EVENT_MOVE); }
	inline void onResize(unsigned int width, unsigned int height){ m_width = width; m_height = height; onEvent(MWIN_EVENT_RESIZE); }
	inline void onClose(void){ onEvent(MWIN_EVENT_CLOSE); }
	
	inline void onMouseButtonDown(unsigned int button){ m_mouseButtons[button] = true;  m_currentMouseButton = button; onEvent(MWIN_EVENT_MOUSE_BUTTON_DOWN); }
	inline void onMouseButtonUp(unsigned int button)  { m_mouseButtons[button] = false; m_currentMouseButton = button; onEvent(MWIN_EVENT_MOUSE_BUTTON_UP); }
	inline void onMouseMove(const MVector2 & pos){ m_mouseDir = pos - m_mousePos; m_mousePos = pos; onEvent(MWIN_EVENT_MOUSE_MOVE); }
	inline void onMouseScroll(const MVector2 & scroll){ m_mouseScroll = scroll; onEvent(MWIN_EVENT_MOUSE_SCROLL); }

	// pointer event
	inline void setEventCallback(void (* eventCallback)(MWindow * rootWindow, MWIN_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// draw callback
	inline void setDrawCallback(void (* drawCallback)(MWindow * rootWindow)){ m_drawCallback = drawCallback; }

	// user pointer
	inline void * getUserPointer(void){ return m_userPointer; }
	inline void setUserPointer(void * userPointer){ m_userPointer = userPointer; }
	
	// draw
	void draw(void);
	
	// update
	void update(void);
};

#endif
