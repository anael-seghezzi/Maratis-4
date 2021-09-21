/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiText.h
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


#ifndef _M_GUI_TEXT_H
#define _M_GUI_TEXT_H


class M_GUI_EXPORT MGuiText : public MGui2d
{
public:

	// constructor
	MGuiText(void);

	// text
	inline void setText(const char * text){ m_textObject.setText(text); if(m_autoScaleFromText) autoScaleFromText(); }

	// virtual
	int getType(void){ return M_GUI_TEXT; }
	void updateFromVariable(void);
	void draw(void);
};

#endif
