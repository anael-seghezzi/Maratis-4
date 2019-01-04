/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiColorPicker.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012-2013 Anael Seghezzi <www.maratis3d.com>
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

#include <MEngine.h>
#include "../Includes/MGui.h"


void MGuiColorPicker::onValueEvents(MGuiEditText * editText, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
		default:
			break;
			
		case MGUI_EVENT_SEND_VARIABLE:
		{
			if(editText->getUserPointer())
			{
				MGuiColorPicker * colorPicker = (MGuiColorPicker*)editText->getUserPointer();
				colorPicker->updateHSVColor();
			}
			break;
		}
	}
}

void MGuiColorPicker::onAlphaEvents(MGuiEditText * editText, MGUI_EVENT_TYPE event)
{
	switch(event)
	{
		default:
			break;
			
		case MGUI_EVENT_SEND_VARIABLE:
		{
			if(editText->getUserPointer())
			{
				MGuiColorPicker * colorPicker = (MGuiColorPicker*)editText->getUserPointer();
				colorPicker->updateRGBColor();
			}
			break;
		}
	}
}

void MGuiColorPicker::winColorEvents(MGuiWindow * window, MGUI_EVENT_TYPE event)
{
	MGuiColorPicker *colorPicker = ((MGuiColorPicker *)window->getUserPointer());

	switch(event)
	{
		default:
			break;
		
		case MGUI_EVENT_MOUSE_BUTTON_DOWN:
		{
			if(! window->isMouseInside() && colorPicker->m_parentButton && !colorPicker->m_parentButton->isMouseInside() && colorPicker->isPopup())
				colorPicker->close();
			break;
		}
	}
}

void MGuiColorPicker::winColorDraw(MGuiWindow * window)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	if(window->getUserPointer())
	{
		MGuiColorPicker * colorPicker = (MGuiColorPicker*)window->getUserPointer();
		
		render->pushMatrix();
		render->translate(window->getPosition());
		colorPicker->draw(window);
		render->popMatrix();
	}
}


MGuiColorPicker::MGuiColorPicker(MWindow * rootWindow, MFontRef * font, bool isPopup):
m_parentButton(NULL),
m_colorSel(NULL),
m_tintSel(NULL),
m_colorTarget(NULL),
m_tintTarget(NULL),
m_font(font),
m_R(NULL),
m_G(NULL),
m_B(NULL),
m_A(NULL),
m_userPointer(NULL),
m_eventCallback(NULL),
m_isPopup(isPopup)
{
	m_window = rootWindow->addNewWindow();

	m_window->setShadow(true);
	m_window->setUserPointer(this);
	m_window->setColor(MVector4(0.25f, 0.25f, 0.25f, 0.75f));
	if (isPopup) m_window->setVisible(false);
}

MGuiColorPicker::~MGuiColorPicker(void)
{
	m_window->deleteMe();
}

void MGuiColorPicker::open(MGuiButton * parentButton, float * R, float * G, float * B, float * A, void (* eventCallback)(MGuiColorPicker * colorPicker, MGUI_COLOR_PICKER_EVENT_TYPE event))
{
	m_eventCallback = eventCallback;
	m_parentButton = parentButton;
	MWindow * rootWindow = m_window->getRootWindow();
	
	m_R = R;
	m_G = G;
	m_B = B;
	m_A = A;

	m_window->clear();
	m_window->setVisible(true);

	// color selection
	m_colorSel = m_window->addNewButton();
	m_tintSel = m_window->addNewButton();
		
	m_colorSel->setPosition(MVector2(10, 10));
	m_colorSel->setScale(MVector2(150, 150));
	m_colorSel->setColor(MVector4(0, 0, 0, 0));
		
	m_tintSel->setPosition(MVector2(170, 10));
	m_tintSel->setScale(MVector2(20, 150));
	m_tintSel->setColor(MVector4(0, 0, 0, 0));
	
	// targets
	m_colorTarget = m_window->addNewImage();
	m_tintTarget = m_window->addNewImage();
		
	m_colorTarget->setScale(MVector2(0, 0));
	m_colorTarget->setColor(MVector4(1, 1, 1, 0));
	m_tintTarget->setScale(MVector2(0, 0));
	m_tintTarget->setColor(MVector4(0, 0, 0, 0));

	// RGB and HSV color
	MVector3 RGBColor = MVector3(*R, *G, *B).getSRBG();
	m_RGB_to_HSV(m_HSVColor, RGBColor);

	
	// window
	if(A == NULL)
		m_window->setScale(MVector2(200, m_colorSel->getScale().y + 70));
	else
		m_window->setScale(MVector2(200, m_colorSel->getScale().y + 86));
	
	if(parentButton)
	{
		MVector2 pos = parentButton->getPosition();
		if(parentButton->getParentWindow())
		{
			pos += parentButton->getParentWindow()->getPosition() + parentButton->getParentWindow()->getScroll();
		}
			
		if(pos.y > rootWindow->getHeight()/2)
		{
			pos.y = pos.y - m_window->getScale().y;
		}
		else
		{
			pos.y = pos.y + parentButton->getScale().y;
		}
			
		if(pos.x > rootWindow->getWidth()/2)
		{
			pos.x = pos.x - m_window->getScale().x + parentButton->getScale().x;
		}
			
		m_window->setPosition(pos);
	}
	
	// update color
	updateHSVColor();
	
	MVector2 position(0, m_colorSel->getScale().y + 16);
	addValue(m_window, &position, "r", m_font, M_VARIABLE_FLOAT, R, onValueEvents);
	addValue(m_window, &position, "g", m_font, M_VARIABLE_FLOAT, G, onValueEvents);
	addValue(m_window, &position, "b", m_font, M_VARIABLE_FLOAT, B, onValueEvents);
	if(A) addValue(m_window, &position, "a", m_font, M_VARIABLE_FLOAT, A, onAlphaEvents);
	
	// callbacks
	m_window->setEventCallback(winColorEvents);
	m_window->setDrawCallback(winColorDraw);
	m_window->setUserPointer(this);
	
	if(m_eventCallback)
		m_eventCallback(this, MGUI_COLOR_PICKER_EVENT_OPEN);
}

void MGuiColorPicker::close(void)
{
	m_window->setVisible(false);
	if(m_eventCallback)
		m_eventCallback(this, MGUI_COLOR_PICKER_EVENT_CLOSE);
}

void MGuiColorPicker::draw(MGuiWindow * window)
{
	MWindow * rootWindow = window->getRootWindow();
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
			
	MVector2 mousePos = m_tintSel->getPointLocalPosition(rootWindow->getMousePosition());

	
	MVector2 vertices[14];
	MVector3 colors[14];
				
	render->disableTexture();
	render->disableCullFace();
		
	render->disableNormalArray();
	render->disableTexCoordArray();
	render->enableVertexArray();
	render->enableColorArray();
				
	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->setColorPointer(M_FLOAT, 3, colors);

	// color
	{
		MVector2 pos = m_colorSel->getPosition();
		MVector2 scale = m_colorSel->getScale();
		MVector3 color;
		
		m_HSV_to_RGB(color, MVector3(m_HSVColor.x, 1, 1));

		for(int i=0; i<(int)scale.y; i++)
		{
			float factor = 1 - (i/scale.y);
			
			colors[0] = colors[1] = MVector3(factor);//.getSRBG();
			colors[2] = colors[3] = (color*factor);//.getSRBG();
						
			vertices[0] = pos + MVector2(0, i);
			vertices[1] = pos + MVector2(0, i+1);
						
			vertices[2] = pos + MVector2(scale.x, i);
			vertices[3] = pos + MVector2(scale.x, i+1);
						
			render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
		}
	}
				
	// tint
	{
		colors[0] = colors[1]	= MVector3(1, 0, 0);
		colors[2] = colors[3]	= MVector3(1, 0, 1);
		colors[4] = colors[5]	= MVector3(0, 0, 1);
		colors[6] = colors[7]	= MVector3(0, 1, 1);
		colors[8] = colors[9]	= MVector3(0, 1, 0);
		colors[10] = colors[11] = MVector3(1, 1, 0);
		colors[12] = colors[13] = MVector3(1, 0, 0);
					
		MVector2 pos = m_tintSel->getPosition();
		MVector2 scale = m_tintSel->getScale();
					
		float py = pos.y;
		float step = scale.y/6.0f;
					
		vertices[0] = pos;
		vertices[1] = pos + MVector2(scale.x, 0);
		py += step;
					
		for(int i=1; i<7; i++)
		{
			vertices[i*2] = MVector2(pos.x, py);
			vertices[i*2+1] = MVector2(pos.x + scale.x, py);
			py += step;
		}
					
		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 14);
	}

	// targets
	{
		render->enableLineAntialiasing();
		render->disableColorArray();
		
		MVector2 center = m_colorTarget->getPosition();
		for(int i=0; i<14; i++)
		{
			float angle = ((float)i/14.0f)*360.0f;
			vertices[i] = center + MVector2(5, 0).getRotated(angle);
		}
		
		render->setColor3(MVector3(m_colorTarget->getNormalColor()));
		render->drawArray(M_PRIMITIVE_LINE_LOOP, 0, 14);
		
		center = m_tintTarget->getPosition();
		for(int i=0; i<14; i++)
		{
			float angle = ((float)i/14.0f)*360.0f;
			vertices[i] = center + MVector2(5, 0).getRotated(angle);
		}
		
		render->setColor3(MVector3(m_tintTarget->getNormalColor()));
		render->drawArray(M_PRIMITIVE_LINE_LOOP, 0, 14);
		
		render->disableLineAntialiasing();
	}

	// update selection
	{
		if(m_tintSel->isActive())
		{
			m_HSVColor.x = (1 - ((mousePos.y - m_tintSel->getPosition().y) / m_tintSel->getScale().y)) * 360;
			m_HSVColor.x = CLAMP(m_HSVColor.x, 0, 360);
					
			updateRGBColor();
		}
		else if(m_colorSel->isActive())
		{
			m_HSVColor.y = ((mousePos.x - m_colorSel->getPosition().x) / m_colorSel->getScale().x);
			m_HSVColor.z = 1 - ((mousePos.y - m_colorSel->getPosition().y) / m_colorSel->getScale().y);
					
			m_HSVColor.y = CLAMP(m_HSVColor.y, 0, 1);
			m_HSVColor.z = CLAMP(m_HSVColor.z, 0, 1);
					
			updateRGBColor();
		}
	}
}

void MGuiColorPicker::updateTargets(void)
{
	float x, y;
	
	x = m_tintSel->getPosition().x + m_tintSel->getScale().x*0.5f - m_tintTarget->getScale().x*0.5f;
	y = m_tintSel->getPosition().y + (1.0f - (m_HSVColor.x/360.0f))*m_tintSel->getScale().y - m_tintTarget->getScale().y*0.5f;
	m_tintTarget->setPosition(MVector2(x, y));
	
	x = m_colorSel->getPosition().x + m_colorSel->getScale().x*m_HSVColor.y - m_colorTarget->getScale().x*0.5f;
	y = m_colorSel->getPosition().y + m_colorSel->getScale().y*(1.0f - m_HSVColor.z) - m_colorTarget->getScale().y*0.5f;
	m_colorTarget->setPosition(MVector2(x, y));
	
	if(m_HSVColor.z > 0.75f)
		m_colorTarget->setNormalColor(MVector3(0.0f));
	else
		m_colorTarget->setNormalColor(MVector3(1.0f));
}

void MGuiColorPicker::updateRGBColor(void)
{
	MVector3 RGBColor, RGBColor_lin;
	
	m_HSV_to_RGB(RGBColor, m_HSVColor);
	RGBColor_lin = RGBColor.getLinear();

	*m_R = RGBColor_lin.x;
	*m_G = RGBColor_lin.y;
	*m_B = RGBColor_lin.z;
	
	if(m_parentButton)
		m_parentButton->setColor(RGBColor);

	updateTargets();

	if(m_eventCallback)
		m_eventCallback(this, MGUI_COLOR_PICKER_EVENT_ON_CHANGE);
}

void MGuiColorPicker::updateHSVColor(void)
{
	if (m_R && m_G && m_B)
	{
		MVector3 RGBColor = MVector3(CLAMP(*m_R, 0, 1), CLAMP(*m_G, 0, 1), CLAMP(*m_B, 0, 1)).getSRBG();
		m_RGB_to_HSV(m_HSVColor, RGBColor);

		if(m_parentButton)
			m_parentButton->setColor(RGBColor);
	
		updateTargets();

		if(m_eventCallback)
			m_eventCallback(this, MGUI_COLOR_PICKER_EVENT_ON_CHANGE);
	}
}
	
void MGuiColorPicker::addValue(MGuiWindow * window, MVector2 * position, const char * name, MFontRef * font, M_VARIABLE_TYPE varType, void * pointer, void (* eventCallback)(MGuiEditText * editText, MGUI_EVENT_TYPE event))
{
	float size = 16;
	float ySpace = 15;
	MVector4 color(1, 1, 1, 1);
	MVector4 valueColor(1.0f, 0.97f, 0.33f, 1.0f);
	MVector4 variableColor(0.66f, 0.76f, 0.87f, 1.0f);
	
	// name
	if(name)
	{
		MGuiText * text = window->addNewText();
		text->setFont(font);
		text->setTextSize(font->getFont()->getFontSize());
		text->setText(name);
		text->setPosition(*position + MVector2(12, 0));
		text->setScale(size);
		text->setColor(valueColor);
	}
	
	// value
	MGuiEditText * editText = window->addNewEditText();
	editText->setFont(font);
	editText->setTextSize(font->getFont()->getFontSize());
	editText->setColor(MVector4(0, 0, 0, 0));
	editText->setPosition(*position + MVector2(32, 0));
	editText->setScale(size);
	editText->setTextColor(color);
	editText->setSingleLine(true);
	editText->setEventCallback(eventCallback);
	editText->enableVariable(pointer, varType);
	editText->setUserPointer(this);
	
	*position += MVector2(0, ySpace);
}
