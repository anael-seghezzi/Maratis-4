/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui2d.h
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


#ifndef _M_GUI2D_H
#define _M_GUI2D_H


class M_GUI_EXPORT MGui2d
{
public:

	MGui2d(void);
	virtual ~MGui2d(void);

protected:

	// linked variable
	M_VARIABLE_TYPE m_variableType;
	void * m_variablePointer;

	// is affected by scrolling
	bool m_isScrollAffect;

	// properties
	bool m_isVisible;
	bool m_isHighLight;
	bool m_isPressed;
	bool m_isDeleted;

	// text object
	float m_textMargin;
	MOText m_textObject;

	// parent
	MGuiWindow * m_parentWindow;

	// position / scale
	MVector2 m_position;
	MVector2 m_scale;
	
	// textures
	MTextureRef * m_normalTexture;
	MTextureRef * m_pressedTexture;
	MTextureRef * m_highLightTexture;

	// style
	bool m_isDrawingText;
	bool m_autoScaleFromText;

	// colors
	MVector4 m_normalColor;
	MVector4 m_pressedColor;
	MVector4 m_highLightColor;

	// shadow textures
	bool m_hasShadow;
	float m_shadowOpacity;
	MVector2 m_shadowDir;

	// user pointer
	void * m_userPointer;

protected:

	MVector2 getAlignedTextPosition(void);

public:

	inline void deleteMe(void){ m_isDeleted = true; }
	inline bool isDeleted(void){ return m_isDeleted; }

	// position / scale
	inline void setPosition(const MVector2 & position){ m_position = position; }
	inline void setXPosition(float x){ m_position.x = x; }
	inline void setYPosition(float y){ m_position.y = y; }
	inline void setScale(const MVector2 & scale){ m_scale = scale; }
	inline void setXScale(float x){ m_scale.x = x; }
	inline void setYScale(float y){ m_scale.y = y; }

	// drawing
	void drawTexturedQuad(unsigned int textureId);
	void drawShadow(void);
	void drawQuad(void);
	void drawText(void);

	// scrolling affect
	inline bool isScrollAffected(void){ return m_isScrollAffect; }
	inline void setScrollAffect(bool affect){ m_isScrollAffect = affect; }

	// linked variable
	inline M_VARIABLE_TYPE getVariableType(void){ return m_variableType; }
	inline void * getVariablePointer(void){ return m_variablePointer; }
	inline void enableVariable(void * variable, M_VARIABLE_TYPE type){ m_variablePointer = variable; m_variableType = type; updateFromVariable(); }
	inline void disableVariable(void){ m_variablePointer = NULL; }

	// shadows
	inline void setShadow(bool shadow){ m_hasShadow = shadow; }
	inline bool hasShadow(void){ return m_hasShadow; }
	inline void setShadowOpacity(float opacity){ m_shadowOpacity = opacity; }
	inline float getShadowOpacity(void){ return m_shadowOpacity; }
	inline void setShadowDir(MVector2 dir){ m_shadowDir = dir; }
	inline MVector2 getShadowDir(void){ return m_shadowDir; }

	// text
	inline void setDrawingText(bool draw){ m_isDrawingText = draw; }
	inline void setTextSize(float size){ m_textObject.setSize(size); if(m_autoScaleFromText) autoScaleFromText(); }
	inline bool isDrawingText(void){ return m_isDrawingText; }
	inline float getTextSize(void){ return m_textObject.getSize(); }
	inline void setTextAlign(M_ALIGN_MODES align){ m_textObject.setAlign(align); }
	inline void setAutoScaleFromText(bool autoScale){ m_autoScaleFromText = autoScale; if(autoScale) autoScaleFromText(); }
	inline bool isAutoScaleFromTextEnabled(void){ return m_autoScaleFromText; }
	inline const char * getText(void){ return m_textObject.getText(); }
	inline M_ALIGN_MODES getTextAlign(void){ return m_textObject.getAlign(); }
	void setTextMargin(float margin){ m_textMargin = margin; }
	float getTextMargin(void){ return m_textMargin; }

	// root window
	MWindow * getRootWindow(void);
	
	// parent
	inline void setParentWindow(MGuiWindow * window){ m_parentWindow = window; }
	inline MGuiWindow * getParentWindow(void){ if(m_parentWindow) return m_parentWindow; return NULL; }

	// position / scale
	inline MVector2 getPosition(void){ return m_position; }
	inline MVector2 getScale(void){ return m_scale; }

	// colors
	inline void setColor(const MVector4 & color){ setNormalColor(color); setPressedColor(color); setHighLightColor(color); }
	inline void setNormalColor(const MVector4 & color){ m_normalColor = color; }
	inline void setPressedColor(const MVector4 & color){ m_pressedColor = color; }
	inline void setHighLightColor(const MVector4 & color){ m_highLightColor = color; }
	inline void setTextColor(const MVector4 & color){ m_textObject.setColor(color); }
	inline MVector4 getNormalColor(void) const { return m_normalColor; }
	inline MVector4 getPressedColor(void) const { return m_pressedColor; }
	inline MVector4 getHighLightColor(void) const { return m_highLightColor; }
	inline MVector4 getTextColor(void) const { return m_textObject.getColor(); }

	// textures
	void setNormalTexture(MTextureRef * texture);
	void setPressedTexture(MTextureRef * texture);
	void setHighLightTexture(MTextureRef * texture);
	inline void disableNormalTexture(void){ m_normalTexture = 0; }
	inline void disablePressedTexture(void){ m_pressedTexture = 0; }
	inline void disableHighLightTexture(void){ m_highLightTexture = 0; }
	inline bool hasNormalTexture(void){ return (m_normalTexture != NULL); }
	inline bool hasPressedTexture(void){ return (m_pressedTexture != NULL); }
	inline bool hasHighLightTexture(void){ return (m_highLightTexture != NULL); }
	inline MTextureRef * getNormalTexture(void){ return m_normalTexture; }
	inline MTextureRef * getPressedTexture(void){ return m_pressedTexture; }
	inline MTextureRef * getHighLightTexture(void){ return m_highLightTexture; }

	// font
	inline void setFont(MFontRef * font){ m_textObject.setFontRef(font); }
	inline MFontRef * getFont(void){ return m_textObject.getFontRef(); }

	// mouse helper
	bool isMouseInside(const MVector2 & margin = MVector2(0, 0));
	
	// point detection
	bool isPointInside(const MVector2 & point, const MVector2 & margin = MVector2(0, 0));
	MVector2 getPointLocalPosition(const MVector2 & point);

	// properties
	inline bool isVisible(void){ return (m_isVisible && !m_isDeleted); }
	inline bool isHighLight(void){ return m_isHighLight; }
	inline bool isPressed(void){ return m_isPressed; }

	// user pointer
	inline void * getUserPointer(void){ return m_userPointer; }
	inline void setUserPointer(void * userPointer){ m_userPointer = userPointer; }

	// virtual
	virtual int getType(void) = 0;
	virtual void setVisible(bool visible){ m_isVisible = visible; }
	virtual void setPressed(bool pressed){ m_isPressed = pressed; }
	virtual void setHighLight(bool highLight){ m_isHighLight = highLight; }
	virtual void autoScaleFromText(void);
	virtual void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event){}
	virtual void updateFromVariable(void){}
	virtual void draw(void);
};

#endif
