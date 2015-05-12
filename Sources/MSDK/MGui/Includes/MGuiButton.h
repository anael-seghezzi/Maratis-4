/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiButton.h
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


#ifndef _M_GUI_BUTTON_H
#define _M_GUI_BUTTON_H


enum MGUI_BUTTON_MODES
{
	MGUI_BUTTON_SIMPLE,
	MGUI_BUTTON_CHECK,
	MGUI_BUTTON_GROUP,
};

class M_GUI_EXPORT MGuiButton : public MGui2d
{
private:

	// button
	MGUI_BUTTON_MODES m_mode;
	unsigned int m_groupId;
	bool m_isActive;

	// events function pointer
	void (* m_eventCallback)(MGuiButton * button, MGUI_EVENT_TYPE event);

public:

	// constructor
	MGuiButton(void);

private:
	
	void press(bool press);
	
public:

	// active
	inline bool isActive(void){ return m_isActive; }
	
	// linked variable
	void sendVariable(void);

	// on change
	void onChange(void);

	// text
	inline void setText(const char * text){ m_textObject.setText(text); if(m_autoScaleFromText) autoScaleFromText(); }

	// mode
	inline bool isCheckButton(void){ return m_mode == MGUI_BUTTON_CHECK; }
	inline bool isGroupButton(void){ return m_mode == MGUI_BUTTON_GROUP; }
	inline void setMode(MGUI_BUTTON_MODES mode){ m_mode = mode; }
	inline MGUI_BUTTON_MODES getMode(void){ return m_mode; }

	// group
	bool unPressGroupButtons(void);
	inline void setGroupId(unsigned int id){ m_groupId = id; }
	inline unsigned int getGroupId(void){ return m_groupId; }

	// pointer event
	inline void setEventCallback(void (* eventCallback)(MGuiButton * button, MGUI_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// virtual
	int getType(void){ return M_GUI_BUTTON; }
	void setPressed(bool pressed);
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void updateFromVariable(void);
	void draw(void);
};

#endif
