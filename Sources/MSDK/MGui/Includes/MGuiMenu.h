/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiMenu.h
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


#ifndef _M_GUI_MENU_H
#define _M_GUI_MENU_H


class M_GUI_EXPORT MGuiMenu : public MGui2d
{
private:

	// menu
	unsigned int m_menuId;

	// pointer event
	void (* m_eventCallback)(MGuiMenu * menu, MGUI_EVENT_TYPE event);

public:

	// constructor / destructor
	MGuiMenu(MWindow * rootWindow);
	~MGuiMenu();

private:

	// window
	MGuiWindow m_window;

	// menu
	bool findMenuId(void);
	void updateSelection(void);
	void updateText(void);

public:

	// linked variable
	void sendVariable(void);

	// on change
	void onChange(void);

	// menu id
	inline unsigned int getMenuId(void){ return m_menuId; }
	void setMenuId(unsigned int id);

	// window
	void addSimpleButton(const char * text, void (* buttoneventCallback)(MGuiButton * button, MGUI_EVENT_TYPE event));
	void clearWindowMenu(void);
	void rescaleWindowMenu(void);
	void drawWindowMenu(void);
	inline MGuiWindow * getWindowMenu(void){ return &m_window; }

	inline void setWindowShadow(bool shadow){ m_window.setShadow(shadow); }
	inline void setWindowMenuColor(const MVector4 & color){ m_window.setNormalColor(color); }
	inline void onWindowMenuEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event){ if(m_window.isVisible()) m_window.onEvent(rootWindow, event); }
	inline bool isWindowMenuHighLight(void){ return m_window.isHighLight(); }
	inline bool isWindowMenuScrollBarPressed(void){ return m_window.isScrollBarPressed(); }

	// pointer event
	inline void setEventCallback(void (* eventCallback)(MGuiMenu * menu, MGUI_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// virtual
	int getType(void){ return M_GUI_MENU; }
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void setPressed(bool pressed);
	void updateFromVariable(void);
	void draw(void);
};

#endif
