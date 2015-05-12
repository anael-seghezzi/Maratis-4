/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiEditText.h
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


#ifndef _M_GUI_EDIT_TEXT_H
#define _M_GUI_EDIT_TEXT_H


class M_GUI_EXPORT MGuiEditText : public MGui2d
{
private:

	bool m_isSingleLine;

	// text
	unsigned int m_charId; // edited character position

	// limit length
	bool m_limitLength;
	unsigned int m_maxLength;

	// selection
	unsigned int m_startSelectionId;
	unsigned int m_endSelectionId;

	// events
	void (* m_eventCallback)(MGuiEditText * editText, MGUI_EVENT_TYPE event);

public:

	// constructor
	MGuiEditText(void);

private:

	void decodeUTF8(vector <unsigned int> * text32);
	void encodeUTF8(const vector <unsigned int> & text32);
	
	bool canAddCharacter(void);
	bool getSelectionIds(unsigned int * start, unsigned int * end);
	void editText(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	MVector2 getCharacterPosition(unsigned int characterId);
	unsigned int findPointedCharacter(MVector2 point);
	void drawSelection(void);

public:

	// limit
	inline void limitLength(unsigned int maxLength){ m_limitLength = true; m_maxLength = maxLength; }
	inline void setMaxLength(unsigned int maxLength){ m_maxLength = maxLength; }
	inline void setLimitedLength(bool limit){ m_limitLength = limit; }

	// linked variable
	void sendVariable(void);

	// selection
	inline void setSelection(unsigned int start, unsigned int end){ m_startSelectionId = start; m_endSelectionId = end; }

	// on change
	void onChange(void);

	// editText properties
	inline unsigned int getCharId(void){ return m_charId; }

	void upCharId(int direction);
	void setCharId(unsigned int id);
	void addCharId(void);
	void subCharId(void);
	void setText(const char * text);

	inline void setSingleLine(bool single){ m_isSingleLine = single; }
	inline bool isSingleLine(void){ return m_isSingleLine; }

	// parent window auto scrolling
	void autoScrolling(void);

	// pointer events
	inline void setEventCallback(void (* eventCallback)(MGuiEditText * editText, MGUI_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// virtual
	int getType(void){ return M_GUI_EDIT_TEXT; }
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void draw(void);
	void autoScaleFromText(void);
	void setPressed(bool pressed);
	void updateFromVariable(void);
};

#endif
