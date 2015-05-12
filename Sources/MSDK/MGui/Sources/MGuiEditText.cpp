/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiEditText.cpp
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


#include <tinyutf8.h>
#include <MEngine.h>
#include "../Includes/MGui.h"


//constructor
MGuiEditText::MGuiEditText(void):
m_isSingleLine(false),
m_charId(0),
m_limitLength(false),
m_maxLength(0),
m_startSelectionId(0),
m_endSelectionId(0),
m_eventCallback(NULL)
{
	setColor(MVector4(0, 0, 0, 0));
	setText("");
}

void MGuiEditText::decodeUTF8(vector <unsigned int> * text32)
{
	const char * text = m_textObject.getText();

	if(strlen(text) == 0)
		return;

	unsigned int charCode;
	unsigned int state = 0;
	unsigned char* s = (unsigned char *)text;
	for(; *s; ++s)
	{
		if(utf8_decode(&state, &charCode, *s) == UTF8_ACCEPT)
			text32->push_back(charCode);
	}
}

void MGuiEditText::encodeUTF8(const vector <unsigned int> & text32)
{
	unsigned int i, size = text32.size();
	char * text = new char[size*4+1];
	
	unsigned char * itext = (unsigned char *)text;
	for(i=0; i<size; i++)
		itext = utf8_encode((unsigned int)text32[i], itext);
	*itext = 0;
	
	m_textObject.setText(text);
	delete [] text;
}

void MGuiEditText::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
		case M_VARIABLE_BOOL:
			{
				bool * value = (bool *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);

				if(i == 1)
					*value = true;
				else if(i == 0)
					*value = false;
			}
			break;
		case M_VARIABLE_INT:
			{
				int * value = (int *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);
				if(i == (*value))
					return;

				*value = i;
			}
			break;
		case M_VARIABLE_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);
				if(i < 0) i = 0;
				if((unsigned int)i == (*value))
					return;

				*value = (unsigned int)i;
			}
			break;
		case M_VARIABLE_FLOAT:
			{
				float * value = (float *)getVariablePointer();

				float f;
				sscanf(getText(), "%f", &f);
				if(f == (*value))
					return;

				*value = f;
			}
			break;
		case M_VARIABLE_STRING:
			{
				MString * value = (MString *)getVariablePointer();
				value->set(getText());
			}
			break;
		default:
			break;
		}

		unsigned int count;
		utf8_len((unsigned char *)getText(), &count);
	
		if(getCharId() > count)
			setCharId(count);
	}

	// send on change gui event
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_SEND_VARIABLE);
}

void MGuiEditText::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;

	switch(getVariableType())
	{
	case M_VARIABLE_BOOL:
		{
			bool * value = (bool *)getVariablePointer();

			if(*value)
				setText("1");
			else
				setText("0");
		}
		break;
	case M_VARIABLE_INT:
		{
			int * value = (int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VARIABLE_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VARIABLE_FLOAT:
		{
			float * value = (float *)getVariablePointer();

			char text[256];
			sprintf(text, "%0.3f", *value);

			setText(text);
		}
		break;
	case M_VARIABLE_STRING:
		{
			MString * value = (MString *)getVariablePointer();
			setText(value->getSafeString());
		}
		break;
	default:
		break;
	}
}

void MGuiEditText::setText(const char * text)
{
	m_textObject.setText(text);

	vector<unsigned int> strText;
	decodeUTF8(&strText);

	if(m_limitLength && strText.size() > m_maxLength)
	{
		unsigned int sStart = m_maxLength;
		unsigned int sEnd = strText.size();
		strText.erase(strText.begin() + sStart, strText.begin() + sEnd);
		encodeUTF8(strText);
	}
	
	unsigned int tSize = strText.size();
	if(getCharId() > tSize)
		setCharId(tSize);

	autoScaleFromText();
}

void MGuiEditText::setPressed(bool pressed)
{
	MGui2d::setPressed(pressed);
	if(! pressed)
		setSelection(0, 0);
}

unsigned int MGuiEditText::findPointedCharacter(MVector2 point)
{
	point = point - m_position - getAlignedTextPosition();

	MFont * font = m_textObject.getFont();
	const char * text = m_textObject.getText();
	vector <float> * linesOffset = m_textObject.getLinesOffset();

	if(! (strlen(text) > 0 && font))
		return 0;

	if(linesOffset->size() == 0)
		return 0;

	unsigned int lineId = 0;
	float lineOffset = (*linesOffset)[0];

	float size = m_textObject.getSize();
	float tabSize = size*2;
	float xc = 0, yc = 0;

	float ptx = point.x - lineOffset;
	float pty = point.y;

	unsigned int id = 0;

	unsigned int charCode;
	unsigned int state = 0;
	unsigned char* s = (unsigned char *)text;
	for(; *s; ++s)
	{
		if(utf8_decode(&state, &charCode, *s) != UTF8_ACCEPT)
			continue;
			
		if(charCode == '\n') // return
		{
			// find character extreme right
			if(pty >= yc-size*0.75f && pty <=yc+size*0.25f)
			{
				if(ptx >= xc)
					return id;
			}
		
			lineId++;
			lineOffset = (*linesOffset)[lineId];
			ptx = point.x - lineOffset;
			
			yc += size;
			xc = 0;
			
			// find character extreme left
			if(pty >= yc-size*0.75f && pty <=yc+size*0.25f)
			{
				if(ptx <= xc)
					return id+1;
			}
			
			id++;
			continue;
		}

		float prev_xc = xc;
		
		if(charCode == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
		}
		else
		{
			// get character
			MCharacter * character = font->getCharacter(charCode);
			if(! character)
			{
				id++;
				continue;
			}

			// move to next character
			xc += character->getXAdvance() * size;
		}
		
		// find character
		if(pty >= yc-size*0.75f && pty <=yc+size*0.25f)
		{
			if(ptx >= prev_xc && ptx <=xc)
			{
				if(ABS(ptx-prev_xc) < ABS(ptx-xc))
					return id;
				else
					return id+1;
			}
		}
		
		id++;
	}

	if(pty >= yc-size*0.75f && pty <=yc+size*0.25f)
	{
		if(ptx >= xc)
			return id;
	}
	
	if(pty > yc+size*0.25f)
	{
		return id;
	}

	return 0;
}

MVector2 MGuiEditText::getCharacterPosition(unsigned int characterId)
{
	MFont * font = m_textObject.getFont();
	const char * text = m_textObject.getText();
	vector <float> * linesOffset = m_textObject.getLinesOffset();

	if(! (strlen(text) > 0 && font))
		return m_position + getAlignedTextPosition();

	if(linesOffset->size() == 0)
		return m_position + getAlignedTextPosition();

	unsigned int lineId = 0;
	float lineOffset = (*linesOffset)[0];

	float size = m_textObject.getSize();
	float tabSize = size*2;
	float xc = 0, yc = 0;

	unsigned int id = 0;

	unsigned int charCode;
	unsigned int state = 0;
	unsigned char* s = (unsigned char *)text;
	for(; *s; ++s)
	{
		if(utf8_decode(&state, &charCode, *s) != UTF8_ACCEPT)
			continue;
			
		if(id == characterId)
			break;
	
		if(charCode == '\n') // return
		{
			lineId++;
			lineOffset = (*linesOffset)[lineId];
	
			yc += size;
			xc = 0;
			id++;
			continue;
		}

		if(charCode == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			id++;
			continue;
		}

		// get character
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
		{
			id++;
			continue;
		}

		// move to next character
		xc += character->getXAdvance() * size;
		id++;
	}
	
	return m_position + getAlignedTextPosition() + MVector2(xc+lineOffset, yc);
}

void MGuiEditText::autoScrolling(void)
{
	MGuiWindow * parent = getParentWindow();

	MVector2 newScroll = parent->getScroll();
	MVector2 charPos;

	// min, max window (with scroll)
	MVector2 min = - parent->getScroll();
	MVector2 max = parent->getScale() - parent->getScroll();

	// selection
	unsigned int sIds[2];
	bool selection = getSelectionIds(&sIds[0], &sIds[1]);

	if(selection)
	{
		// charPos
		charPos = getCharacterPosition(m_endSelectionId);
	}
	else
	{
		// charPos
		charPos = getCharacterPosition(getCharId());
	}

	// new scrolling
	if((charPos.x + getTextSize()) > max.x) newScroll.x -= (charPos.x + getTextSize()) - max.x;
	if((charPos.y + getTextSize()*0.15f) > max.y) newScroll.y -= (charPos.y + getTextSize()*0.15f) - max.y;

	if(charPos.x < min.x) newScroll.x += min.x - charPos.x;
	if((charPos.y - getTextSize()) < min.y) newScroll.y += min.y - (charPos.y - getTextSize());

	parent->setScroll(newScroll);
	parent->resizeScroll();
}

void MGuiEditText::upCharId(int direction)
{
	MVector2 pos = getCharacterPosition(getCharId());
	pos += MVector2(0, getTextSize() * direction);

	if(pos.y < getPosition().y)
		return;

	if(pos.y > (getPosition().y + getScale().y))
		return;

	unsigned int id = findPointedCharacter(pos);

	setCharId(id);
}

void MGuiEditText::setCharId(unsigned int id)
{
	unsigned int count;
	utf8_len((unsigned char *)getText(), &count);
	
	if(id <= count)
		m_charId = id;
}

void MGuiEditText::addCharId(void)
{
	unsigned int count;
	utf8_len((unsigned char *)getText(), &count);
	
	if(m_charId < count)
		m_charId++;
}

void MGuiEditText::subCharId(void)
{
	if(m_charId > 0)
		m_charId--;
}

void MGuiEditText::autoScaleFromText(void)
{
	MGui2d::autoScaleFromText();
	
	float size = getTextSize();
	m_scale.x = MAX(m_scale.x, size);
	m_scale.y = MAX(m_scale.y, size);
}

void MGuiEditText::onChange(void)
{
	// send on change gui event
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_ON_CHANGE);
}

bool MGuiEditText::getSelectionIds(unsigned int * start, unsigned int * end)
{
	if(m_endSelectionId > m_startSelectionId)
	{
		*start = m_startSelectionId;
		*end = m_endSelectionId;
	}
	else
	{
		*start = m_endSelectionId;
		*end = m_startSelectionId;
	}

	unsigned int count;
	utf8_len((unsigned char *)getText(), &count);

	if(*start >= count)
		*start = count;

	if(*end >= count)
		*end = count;

	if(*start == *end)
		return false;

	return true;
}

bool MGuiEditText::canAddCharacter(void)
{
	if(! m_limitLength)
		return true;

	unsigned int count;
	utf8_len((unsigned char *)getText(), &count);
	if(count < m_maxLength)
		return true;

	return false;
}

void MGuiEditText::editText(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	unsigned int sStart;
	unsigned int sEnd;

	bool selection = getSelectionIds(&sStart, &sEnd);

	// events
	if(event == MWIN_EVENT_KEY_DOWN)
	{
		switch(rootWindow->getKey())
		{
		case MKEY_UP:
			if(! isSingleLine())
			{
				if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
					if(m_endSelectionId == m_startSelectionId)
						m_startSelectionId = m_charId;
		
				upCharId(-1);

				if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
					m_endSelectionId = m_charId;
				else
					setSelection(0, 0);
			}
			return;

		case MKEY_DOWN:
			if(! isSingleLine())
			{
				if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
					if(m_endSelectionId == m_startSelectionId)
						m_startSelectionId = m_charId;
				
				upCharId(1);

				if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
					m_endSelectionId = m_charId;
				else
					setSelection(0, 0);
			}
			return;

		case MKEY_RIGHT:
			if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
				if(m_endSelectionId == m_startSelectionId)
					m_startSelectionId = m_charId;
					
			addCharId();
			
			if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
				m_endSelectionId = m_charId;
			else
				setSelection(0, 0);
			return;

		case MKEY_LEFT:
			if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
				if(m_endSelectionId == m_startSelectionId)
					m_startSelectionId = m_charId;
					
			subCharId();
			
			if(rootWindow->isKeyPressed(MKEY_LSHIFT) || rootWindow->isKeyPressed(MKEY_RSHIFT))
				m_endSelectionId = m_charId;
			else
				setSelection(0, 0);
			return;

		case MKEY_TAB:
			if(! isSingleLine())
			{
				vector<unsigned int> strText;
				decodeUTF8(&strText);
				
				if(selection)
				{
					strText.erase(strText.begin()+sStart, strText.begin() + sEnd);
					setSelection(0, 0);
					setCharId(sStart);
				}

				if(! canAddCharacter())
				{
					encodeUTF8(strText);
					return;
				}

				strText.insert(strText.begin()+getCharId(), '\t');
				encodeUTF8(strText);
				
				addCharId();
				autoScaleFromText();
				onChange();
			}
			return;

		case MKEY_BACKSPACE:
			if(selection)
			{
				vector<unsigned int> strText;
				decodeUTF8(&strText);
				
				strText.erase(strText.begin() + sStart, strText.begin() + sEnd);
				encodeUTF8(strText);
				
				setSelection(0, 0);
				setCharId(sStart);
				autoScaleFromText();
				onChange();
			}
			else if(getCharId() > 0)
			{
				vector<unsigned int> strText;
				decodeUTF8(&strText);
				
				strText.erase(strText.begin() + getCharId() - 1);
				encodeUTF8(strText);
				
				subCharId();
				autoScaleFromText();
				onChange();
			}
			return;

		case MKEY_DELETE:
		{
			unsigned int count;
			utf8_len((unsigned char *)getText(), &count);
	
			if(getCharId() < count)
			{
				vector<unsigned int> strText;
				decodeUTF8(&strText);
				
				if(selection)
				{
					strText.erase(strText.begin() + sStart, strText.begin() + sEnd);
					setSelection(0, 0);
					setCharId(sStart);
				}
				else
				{
					strText.erase(strText.begin() + getCharId());
				}

				encodeUTF8(strText);
				autoScaleFromText();
				onChange();
			}
			return;
		}

		case MKEY_KP_ENTER:
		case MKEY_RETURN:
			if(! isSingleLine())
			{
				vector<unsigned int> strText;
				decodeUTF8(&strText);
				
				if(selection)
				{
					strText.erase(strText.begin()+sStart, strText.begin() + sEnd);
					setSelection(0, 0);
					setCharId(sStart);
				}

				if(! canAddCharacter())
				{
					encodeUTF8(strText);
					return;
				}

				strText.insert(strText.begin()+getCharId(), '\n');
				encodeUTF8(strText);
				
				addCharId();
				autoScaleFromText();
				onChange();
			}
			else
			{
				setHighLight(false);
				autoScaleFromText();
				setSelection(0, 0);
				sendVariable();
			}
			return;
			
		default:
			break;
		}
	}

	if(event == MWIN_EVENT_CHAR)
	{
		int character = rootWindow->getKey();
	
		// add character
		if(character)
		{
			vector<unsigned int> strText;
			decodeUTF8(&strText);
			
			if(selection)
			{
				strText.erase(strText.begin()+sStart, strText.begin() + sEnd);
				setSelection(0, 0);
				setCharId(sStart);
			}

			if(! canAddCharacter())
			{
				encodeUTF8(strText);
				return;
			}

			strText.insert(strText.begin()+getCharId(), (unsigned int)character);
			encodeUTF8(strText);
			
			addCharId();
			autoScaleFromText();
			onChange();
		}
	}
}

void MGuiEditText::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MGuiWindow * parent = getParentWindow();

	// font
	MFont * font = m_textObject.getFont();
	if(! font)
		return;

	switch(event)
	{
	case MWIN_EVENT_MOUSE_SCROLL:
	case MWIN_EVENT_MOUSE_MOVE:
		if(parent->isHighLight() && isMouseInside(MVector2(12, 0)))
		{
			setHighLight(true);

			if(m_eventCallback) // send mouse move gui event
				m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
		}
		else
		{
			setHighLight(false);
		}

		if(isPressed() && rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_LEFT))
		{
			m_endSelectionId = findPointedCharacter(getPointLocalPosition(rootWindow->getMousePosition()));
			autoScrolling();
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(isHighLight())
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				// unpress all edit text
				unsigned int i;
				unsigned int size = parent->getEditTextsNumber();
				for(i=0; i<size; i++)
					parent->getEditText(i)->setPressed(false);

				setPressed(true);

				setCharId(
					findPointedCharacter(getPointLocalPosition(rootWindow->getMousePosition()))
				);

				// start select
				setSelection(getCharId(), getCharId());
			}

			if(m_eventCallback) // send mouse button down gui event
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
		}
		else
		{
			if(isPressed() && rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				setPressed(false);
				sendVariable();
			}
		}
		break;
		
	case MWIN_EVENT_CHAR:
	case MWIN_EVENT_KEY_DOWN:
		if(isPressed())
		{
			editText(rootWindow, event);
			autoScrolling();
		}
		break;
		
	default:
		break;
	}
}

void MGuiEditText::drawSelection(void)
{	
	unsigned int id1, id2;
	getSelectionIds(&id1, &id2);
	if(id1 == id2)
		return;
	
	MGuiImage box;
	float textSize = getTextSize();
	
	MVector2 pos1 = getCharacterPosition(id1);
	MVector2 pos2 = getCharacterPosition(id2);
	
	pos2.x += 2;
	
	if(pos2.y > pos1.y)
	{
		// center
		box.setPosition(MVector2(m_position.x, pos1.y));
		box.setScale(MVector2(m_position.x+m_scale.x, pos2.y-textSize) - box.getPosition());
		box.drawQuad();
		
		// top
		box.setPosition(MVector2(pos1.x, pos1.y-textSize));
		box.setScale(MVector2(m_position.x+m_scale.x, pos1.y) - box.getPosition());
		box.drawQuad();
		
		// bottom
		box.setPosition(MVector2(m_position.x, pos2.y-textSize));
		box.setScale(MVector2(pos2.x, pos2.y) - box.getPosition());
		box.drawQuad();
	}
	else
	{
		// top
		box.setPosition(MVector2(pos1.x, pos1.y-textSize));
		box.setScale(MVector2(pos2.x, pos2.y) - box.getPosition());
		box.drawQuad();
	}
}

void MGuiEditText::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	
	if(! isVisible())
		return;

	if(! isPressed())
		updateFromVariable();

	// draw selection
	if(isPressed())
	{
		render->pushMatrix();
		render->translate(MVector3(0, getTextSize()*0.15f, 0));
		render->setColor4(MVector4(1, 0.1f, 0.1f, 0.3f));
		
		drawSelection();
		
		render->popMatrix();
	}

	// draw back
	{
		if(isPressed() && getPressedColor().w > 0)
		{
			render->setColor4(getPressedColor());
			drawQuad();
		}
		else if(isHighLight() && getHighLightColor().w > 0)
		{
			render->setColor4(getHighLightColor());
			drawQuad();
		}
		else if(getNormalColor().w > 0)
		{
			render->setColor4(getNormalColor());
			drawQuad();
		}
	}

	// draw text
	drawText();

	if(isPressed() && (m_startSelectionId == m_endSelectionId)) // cursor
	{
		render->disableTexture();
		render->setColor4(MVector4(1, 0, 0, 0.5f));

		// position
		MVector2 position = getCharacterPosition(getCharId());

		// scale
		MVector2 scale = MVector2(MIN(2, getTextSize() * 0.1f), getTextSize());

		float px = (float)((int)position.x);
		float py = (float)((int)position.y + getTextSize()*0.15f);

		// draw
		render->disableNormalArray();
		render->disableTexCoordArray();
		render->disableColorArray();
		render->enableVertexArray();

		MVector2 vertices[4] = {
			MVector2(px, py - scale.y),
			MVector2(px, py),
			MVector2(px + scale.x, py - scale.y),
			MVector2(px + scale.x, py)
		};

		render->setVertexPointer(M_FLOAT, 2, vertices);
		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
	}
}
