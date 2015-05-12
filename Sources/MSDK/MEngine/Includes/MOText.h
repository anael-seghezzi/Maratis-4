/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOText.h
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


#ifndef _M_OTEXT_H
#define _M_OTEXT_H


// align modes
enum M_ALIGN_MODES
{
	M_ALIGN_LEFT = 0,
	M_ALIGN_RIGHT,
	M_ALIGN_CENTER
};

// MOText
class M_ENGINE_EXPORT MOText : public MObject3d
{
public:

	// constructor / destructor
	MOText(MFontRef * fontRef);
	~MOText(void);

	// copy constructor
	MOText(const MOText & text);

private:

	// font ref
	MFontRef * m_fontRef;

	// text
	MString m_text;

	// align
	M_ALIGN_MODES m_align;
	vector <float> m_linesOffset;

	// size
	float m_size;

	// color
	MVector4 m_color;

	// bounding box
	MBox3d m_boundingBox;

private:

	void updateLinesOffset(void);
	void prepare(void);

public:

	// type
	int getType(void){ return M_OBJECT3D_TEXT; }

	// font
	void setFontRef(MFontRef * fontRef);
	MFont * getFont(void);
	inline MFontRef * getFontRef(void){ return m_fontRef; }

	// text
	void setText(const char * text);
	inline const char * getText(void){ return m_text.getSafeString(); }

	// lines offset
	inline vector <float> * getLinesOffset(void){ return &m_linesOffset; }
	
	// align
	void setAlign(M_ALIGN_MODES align);
	inline M_ALIGN_MODES getAlign(void){ return m_align; }

	// size
	void setSize(float size);
	inline float getSize(void){ return m_size; }

	// color
	inline void setColor(const MVector4 & color){ m_color = color; }
	inline MVector4 getColor(void) const { return m_color; }

	// bounding box
	inline MBox3d * getBoundingBox(void){ return &m_boundingBox; }

	// visibility
	void updateVisibility(MOCamera * camera);
};

#endif