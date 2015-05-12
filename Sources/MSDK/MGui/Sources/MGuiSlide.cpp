/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiSlide.cpp
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
MGuiSlide::MGuiSlide(void):
m_value(0),
m_minValue(0),
m_maxValue(1),
m_eventCallback(NULL)
{}

float MGuiSlide::getNormalizedValue(void)
{
	if(getVariableType())
	{
		if(getVariableType() == M_VARIABLE_INT || getVariableType() == M_VARIABLE_UINT)
		{
			int iValue;
			if(m_value >= 0)
				iValue = (int)(m_value + 0.5f);
			else
				iValue = (int)(m_value - 0.5f);

			return (iValue - getMinValue()) / (getMaxValue() - getMinValue());
		}
	}

	return (getValue() - getMinValue()) / (getMaxValue() - getMinValue());
}

void MGuiSlide::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
		case M_VARIABLE_INT:
			{
				int * value = (int *)getVariablePointer();
				if(m_value >= 0)
					*value = (int)(m_value + 0.5f);
				else
					*value = (int)(m_value - 0.5f);
			}
			break;
		case M_VARIABLE_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();
				if(m_value >= 0)
					*value = (unsigned int)(m_value + 0.5f);
				else
					*value = 0;
			}
			break;
		case M_VARIABLE_FLOAT:
			{
				float * value = (float *)getVariablePointer();
				*value = m_value;
			}
			break;
		default:
			break;
		}
	}

	// send on change gui event
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_SEND_VARIABLE);
}

void MGuiSlide::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;

	switch(getVariableType())
	{
	case M_VARIABLE_INT:
		{
			int * value = (int *)getVariablePointer();
			setValue((float)*value);
		}
		break;
	case M_VARIABLE_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();
			setValue((float)*value);
		}
		break;
	case M_VARIABLE_FLOAT:
		{
			float * value = (float *)getVariablePointer();
			setValue(*value);
		}
		break;
	default:
		break;
	}
}

void MGuiSlide::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MGuiWindow * parent = getParentWindow();

	switch(event)
	{
	case MWIN_EVENT_MOUSE_SCROLL:
	case MWIN_EVENT_MOUSE_MOVE:
		{
			MVector2 localMousePos = getPointLocalPosition(rootWindow->getMousePosition());
	
			if(parent->isHighLight() && m_button.isPointInside(localMousePos) &&
			   (! rootWindow->isMouseButtonPressed(0)) &&
			   (! rootWindow->isMouseButtonPressed(1)) &&
			   (! rootWindow->isMouseButtonPressed(2)))
			{
				setHighLight(true);

				if(m_eventCallback)
					m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
			}
			else{
				setHighLight(false);
			}

			if(isPressed())
			{
				float value = getValueFromPoint(localMousePos);
				setValue(value);
			}
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(isHighLight())
		{
			MVector2 localMousePos = getPointLocalPosition(rootWindow->getMousePosition());
			
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				setPressed(true);
				m_offset = localMousePos - m_button.getPosition();
			}

			if(m_eventCallback) // send gui event
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_UP:
		if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			setPressed(false);
		break;
		
	default:
		break;
	}
}

void MGuiSlide::updatePosition(void)
{
	// update slide position
	m_button.setPosition(getPointfromValue(getValue()));
}

void MGuiSlide::onChange(void)
{
	// update slide position
	updatePosition();

	// send linked variable
	sendVariable();

	// send on change gui event
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_ON_CHANGE);
}

void MGuiSlide::setValue(float value)
{
	float old = m_value;

	m_value = value;

	// clamp value
	if(m_value < m_minValue)
	{
		m_value = m_minValue;
	}
	else if(m_value > m_maxValue)
	{
		m_value = m_maxValue;
	}

	if(m_value != old)
		onChange();
}

void MGuiSlide::setDirection(const MVector2 & direction)
{
	m_direction = direction;
	setScale(getDirection() + m_button.getScale());
}

float MGuiSlide::getValueFromPoint(const MVector2 & point)
{
	float pos;
	float val;

	if(getDirection().x > getDirection().y)
	{
		pos = getPosition().x + m_offset.x;
		val = (point.x - pos) / getDirection().x;
	}
	else
	{
		pos = getPosition().y + m_offset.y;
		val = (point.y - pos) / getDirection().y;
	}

	if(val < 0)
		val = 0;

	if(val > 1)
		val = 1;

	return getMinValue() + (val * (getMaxValue() - getMinValue()));
}

MVector2 MGuiSlide::getPointfromValue(float value)
{
	float nValue;
	MVector2 point;

	// variable pointer
	if(getVariablePointer())
	{
		if(getVariableType() == M_VARIABLE_INT || getVariableType() == M_VARIABLE_UINT)
		{
			int iValue;

			if(value >= 0)
				iValue = (int)(value+0.5f);
			else
				iValue = (int)(value-0.5f);

			nValue = (iValue - getMinValue()) / (getMaxValue() - getMinValue());

			if(nValue < 0)
				nValue = 0;

			if(nValue > 1)
				nValue = 1;

			point = getPosition() + (getDirection() * nValue);
			return point;
		}
	}

	// normal
	nValue = (value - getMinValue()) / (getMaxValue() - getMinValue());

	if(nValue < 0)
		nValue = 0;

	if(nValue > 1)
		nValue = 1;

	point = getPosition() + (getDirection() * nValue);
	return point;
}

void MGuiSlide::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	if(! isVisible())
		return;

	if(! isPressed())
		updateFromVariable();

	if(isPressed()) // pressed
	{
		render->setColor4(getPressedColor());

		if(hasPressedTexture())
			m_button.drawTexturedQuad(getPressedTexture()->getTextureId());
		else
			m_button.drawQuad();
	}
	else if(isHighLight()) // highLight
	{
		render->setColor4(getHighLightColor());

		if(hasHighLightTexture())
			m_button.drawTexturedQuad(getHighLightTexture()->getTextureId());
		else
			m_button.drawQuad();
	}
	else // normal
	{
		render->setColor4(getNormalColor());

		if(hasNormalTexture())
			m_button.drawTexturedQuad(getNormalTexture()->getTextureId());
		else
			m_button.drawQuad();
	}
}
