/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiButton.cpp
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


//constructor
MGuiButton::MGuiButton(void):
m_mode(MGUI_BUTTON_SIMPLE),
m_groupId(0),
m_isActive(false),
m_eventCallback(NULL)
{
	setText("");
	setColor(MVector4(1, 1, 1, 1));
	setTextColor(MVector4(0, 0, 0, 0.5f));
}

void MGuiButton::sendVariable(void)
{
	if(getVariablePointer())
	switch(getVariableType())
	{
	case M_VARIABLE_BOOL:
		{
			bool * value = (bool *)getVariablePointer();
			*value = isPressed();
		}
		break;
            
	default:
		break;
	}
}

void MGuiButton::updateFromVariable(void)
{
	if(getVariablePointer())
	switch(getVariableType())
	{
	case M_VARIABLE_BOOL:
		{
			bool * value = (bool *)getVariablePointer();
			if(*value)
				m_isPressed = true;
			else
				m_isPressed = false;
		}
		break;
            
	default:
		break;
	}
}

void MGuiButton::onChange(void)
{
	sendVariable();
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_ON_CHANGE);
}

void MGuiButton::press(bool press)
{
	if(press != isPressed())
	{
		MGui2d::setPressed(press);
		onChange();
	}	
}

void MGuiButton::setPressed(bool pressed)
{
	if(pressed != isPressed())
	{
		MGui2d::setPressed(pressed);
		sendVariable();
	}
}

void MGuiButton::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MGuiWindow * parent = getParentWindow();

	// events
	switch(event)
	{
	case MWIN_EVENT_MOUSE_SCROLL:
	case MWIN_EVENT_MOUSE_MOVE:
	
		if(parent->isHighLight() && isMouseInside())
		{
			setHighLight(true);
			if((m_mode == MGUI_BUTTON_SIMPLE) && (!isPressed()) && m_isActive)
				press(true);

			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
		}
		else
		{
		    setHighLight(false);
			if((m_mode == MGUI_BUTTON_SIMPLE) && isPressed())
				press(false);
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(parent->isHighLight() && isMouseInside())
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				if(! isCheckButton())
				{
					if(isGroupButton())
						 unPressGroupButtons();
                    press(true);
				}

				if(! isHighLight())
                    setHighLight(true);
			}

			m_isActive = true;

			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_UP:
		if(parent->isHighLight() && isMouseInside())
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
			    if(m_isActive)
			    {
                    if(isCheckButton())
                        press(! isPressed());
                }

				if(! isHighLight())
                    setHighLight(true);
			}

			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_UP);

			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				if((! isCheckButton()) && (! isGroupButton()))
					press(false);
			}
		}
		m_isActive = false;
		break;
            
	default:
		break;
	}
}

bool MGuiButton::unPressGroupButtons()
{
	MGuiWindow * parent = getParentWindow();

	if(parent)
	{
		unsigned int i;
		unsigned int bSize = parent->getButtonsNumber();
		for(i=0; i<bSize; i++)
		{
			MGuiButton * button = parent->getButton(i);

			if(button->isGroupButton())
			if(button->getGroupId() == getGroupId())
			{
				button->press(false);
			}
		}

		return true;
	}

	return false;
}

void MGuiButton::draw(void)
{
	if(! isVisible())
		return;

	updateFromVariable();
	MGui2d::draw();
}
