/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiMenu.cpp
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


#include <MEngine.h>
#include "../Includes/MGui.h"


// constructor
MGuiMenu::MGuiMenu(MWindow * rootWindow):
m_menuId(0),
m_eventCallback(NULL),
m_window(rootWindow)
{
	setTextColor(MVector4(0, 0, 0, 0.5f));
	m_window.setMargin(MVector2(0, 0));
}

MGuiMenu::~MGuiMenu(void)
{
	clearWindowMenu();
}

void MGuiMenu::setMenuId(unsigned int id)
{
	m_menuId = id;
	updateText();
	autoScaleFromText();
}

void MGuiMenu::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
			case M_VARIABLE_INT:
			{
				int * value = (int *)getVariablePointer();
				*value = (int)getMenuId();
			}
			break;
			case M_VARIABLE_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();
				*value = getMenuId();
			}
			break;
			default:
				break;
		}
	}
	
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_SEND_VARIABLE);
}

void MGuiMenu::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;
	
	switch(getVariableType())
	{
		case M_VARIABLE_INT:
		{
			int * value = (int *)getVariablePointer();
			setMenuId(*value);
			updateText();
			autoScaleFromText();
			break;
		}
		
		case M_VARIABLE_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();
			setMenuId(*value);
			updateText();
			autoScaleFromText();
			break;
		}
		
		default:
			break;
	}
}

void MGuiMenu::updateSelection(void)
{
	unsigned int oldId = getMenuId();
	if(findMenuId())
	{
		sendVariable();
		
		if(getMenuId() != oldId)
			onChange();
	}
}

void MGuiMenu::setPressed(bool pressed)
{
	MGui2d::setPressed(pressed);
}

bool MGuiMenu::findMenuId(void)
{
	// find menu_id
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	for(i=0; i<bSize; i++)
	{
		if(m_window.getButton(i)->isMouseInside())
		{
			m_menuId = i;
			return true;
		}
	}
	
	return false;
}

void MGuiMenu::onChange(void)
{
	// send on change gui event
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_ON_CHANGE);
}

void MGuiMenu::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MGuiWindow * parent = getParentWindow();
	
	//event
	switch(event)
	{
		case MWIN_EVENT_MOUSE_SCROLL:
		case MWIN_EVENT_MOUSE_MOVE:
			if((! isPressed()) && parent->isHighLight() && isMouseInside() &&
			   (! rootWindow->isMouseButtonPressed(0)) &&
			   (! rootWindow->isMouseButtonPressed(1)) &&
			   (! rootWindow->isMouseButtonPressed(2)))
			{
				unsigned int m, mSize = parent->getMenusNumber();
				for(m=0; m<mSize; m++)
					parent->getMenu(m)->setHighLight(false);

				setHighLight(true);
				
				if(m_eventCallback)
					m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
			}
			else
			{
				setHighLight(false);
			}
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_DOWN:
			
			if(parent->isHighLight() && isHighLight())
			{
				if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
				{
					if(isPressed())
					{
						setPressed(false);
						updateText();
						autoScaleFromText();
					}
					else {
						setPressed(true);
						setHighLight(false);
						m_window.setHighLight(false);
						rescaleWindowMenu();
					}
				}
				
				if(m_eventCallback)
					m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
			}
			else if(! m_window.isMouseInside())
			{
				if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT) // left mouse button
				{
					setPressed(false);
					updateText();
					autoScaleFromText();
				}
			}
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_UP:
		
			if(parent->isHighLight() && isMouseInside())
			{
				if(m_eventCallback)
					m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_UP);
			}
			else if(isPressed() && (!isMouseInside()))
			{
				if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
				{
					setPressed(false);
					if(m_window.isMouseInside())
						updateSelection();
						
					updateText();
					autoScaleFromText();
				}
			}
			break;
			
		default:
			break;
	}
}

void MGuiMenu::updateText(void)
{
	unsigned int bSize = m_window.getButtonsNumber();
	if(m_menuId < bSize)
		m_textObject.setText(m_window.getButton(m_menuId)->getText());
}

void MGuiMenu::rescaleWindowMenu(void)
{
	MVector2 tmp;
	MVector2 newScale(0, 0);
	
	MGuiWindow * parent = getParentWindow();
	MWindow * rootWindow = parent->getRootWindow();
	
	unsigned int width = rootWindow->getWidth();
	unsigned int height = rootWindow->getHeight();
	
	// position
	MVector2 newPos = parent->getPosition() +
	parent->getScroll() +
	getPosition();
	
	// window scale
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	for(i=0; i<bSize; i++)
	{
		MGuiButton * button = m_window.getButton(i);
		tmp = button->getPosition()
		+ button->getScale();
		
		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}
	
	unsigned int tSize = m_window.getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MGuiText * text = m_window.getText(i);
		tmp = text->getPosition()
		+ text->getScale();
		
		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}
	
	// y
	if(newPos.y < height/2)
	{
		newPos.y += getScale().y;
		
		if((newPos.y + newScale.y) > height)
			newScale.y = height - newPos.y;
	}
	else
	{
		newPos.y -= newScale.y;
		
		if(newPos.y < 0)
		{
			newScale.y += newPos.y;
			newPos.y = 0;
		}
	}
	
	// x
	if((newPos.x + newScale.x) > width){
		newPos.x -= ((newPos.x + newScale.x) - width);
	}
	
	if(newPos.x < 0) newPos.x = 0;
	
	if((newPos.x + newScale.x) > width){
		newScale.x = width - newPos.x;
	}
	
	m_window.setPosition(newPos);
	m_window.setScale(newScale);
	
	// buttons size
	for(i=0; i<bSize; i++)
	{
		MGuiButton * button = m_window.getButton(i);
		tmp = button->getScale();
		
		if(newScale.x > tmp.x)
			tmp.x = newScale.x;
		
		button->setScale(tmp);
	}
	
	m_window.resizeScroll();
}

void MGuiMenu::drawWindowMenu(void)
{
	m_window.draw();
}

void MGuiMenu::addSimpleButton(const char * text, void (* buttoneventCallback)(MGuiButton * button, MGUI_EVENT_TYPE event))
{
	MVector2 pos(0, 0);
	
	// position
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	
	for(i=0; i<bSize; i++)
		pos.y += m_window.getButton(i)->getScale().y;
	
	// new button
	MGuiButton * btn = m_window.addNewButton();
	
	btn->setPosition(pos);
	btn->setTextMargin(2);
	btn->setFont(getFont());
	btn->setTextSize(getTextSize());
	btn->setTextColor(getTextColor());
	btn->setText(text);
	btn->setColor(MVector4(1, 1, 1, 0));
	btn->setPressedColor(getHighLightColor());
	btn->setHighLightColor(getHighLightColor());
	btn->setEventCallback(buttoneventCallback);
	
	updateText();
	autoScaleFromText();
}

void MGuiMenu::clearWindowMenu(void)
{
	m_window.clear();
}

void MGuiMenu::draw(void)
{
	if(! isVisible())
		return;
	
	if(! isPressed())
		updateFromVariable();
	
	MGui2d::draw();
}
