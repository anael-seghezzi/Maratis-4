/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOText.cpp
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


#include <tinyutf8.h>
#include "../Includes/MEngine.h"


// constructor
MOText::MOText(MFontRef * fontRef):
MObject3d(),
m_align(M_ALIGN_LEFT),
m_size(16),
m_color(1, 1, 1, 1)
{
	setFontRef(fontRef);
}

// copy constructor
MOText::MOText(const MOText & text):
MObject3d(text),
m_fontRef(text.m_fontRef),
m_text(text.m_text),
m_align(text.m_align),
m_size(text.m_size),
m_color(text.m_color),
m_boundingBox(text.m_boundingBox)
{
	prepare();
}

// destructor
MOText::~MOText(void)
{
	MObject3d::clearObject3d();
}

void MOText::setFontRef(MFontRef * fontRef)
{
	m_fontRef = fontRef;
	prepare();
}

MFont * MOText::getFont(void)
{
	if(! m_fontRef)
		return NULL;

	return m_fontRef->getFont();
}

void MOText::setText(const char * text)
{
	m_text.set(text);
	prepare();
}

void MOText::setAlign(M_ALIGN_MODES align)
{
	m_align = align;
	prepare();
}

void MOText::setSize(float size)
{ 
	m_size = size;
	prepare();
}

void MOText::updateLinesOffset(void)
{
	MFont * font = getFont();
	const char * text = m_text.getData();

	float tabSize = m_size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float xc = 0;
	float min = 0;
	float max = 0;

	// bounding box
	MVector3 * boxMin = &m_boundingBox.min;
	MVector3 * boxMax = &m_boundingBox.max;
	
	float globalOffset = boxMax->x - boxMin->x;
	
	// clear lines
	m_linesOffset.clear();

	if(text)
	{
		unsigned int charCode;
		unsigned int state = 0;
		unsigned char* s = (unsigned char*)text;
		for(; *s; ++s)
		{
			if(utf8_decode(&state, &charCode, *s) != UTF8_ACCEPT)
				continue;
			
			if(charCode == '\n') // return
			{
				switch(m_align)
				{
					case M_ALIGN_LEFT:
						m_linesOffset.push_back(0);
						break;
					case M_ALIGN_RIGHT:
						m_linesOffset.push_back((int)(-globalOffset + (boxMax->x - max)));
						break;
					case M_ALIGN_CENTER:
						m_linesOffset.push_back((int)((boxMin->x - min) - globalOffset*0.5f + (globalOffset - (max - min))*0.5f));
						break;
				}
				
				min = 0;
				max = 0;
				xc = 0;
				continue;
			}
			
			if(charCode == '\t') // tab
			{
				int tab = (int)(xc / tabSize) + 1;
				xc = tab*tabSize;
				max = MAX(max, xc);
				continue;
			}
			
			// get character
			MCharacter * character = font->getCharacter(charCode);
			if(! character)
				continue;
			
			MVector2 scale = character->getScale();
			MVector2 offset = character->getOffset() * m_size;
			
			float width = scale.x * widthFactor * m_size;
			
			float charMin = xc + offset.x;
			float charMax = charMin + width;
			
			min = MIN(min, charMin);
			max = MAX(max, charMax);
			max = MAX(max, xc + character->getXAdvance()*m_size);
			
			//move to next character
			xc += character->getXAdvance() * m_size;
		}
	}

	// last char (always should be executed!)
	switch(m_align)
	{
	case M_ALIGN_LEFT:
		m_linesOffset.push_back(0);
		break;
	case M_ALIGN_RIGHT:
		m_linesOffset.push_back((int)(-globalOffset + (boxMax->x - max)));
		break;
	case M_ALIGN_CENTER:
		m_linesOffset.push_back((int)((boxMin->x - min) - globalOffset*0.5f + (globalOffset - (max - min))*0.5f));
		break;
	}

	switch(m_align)
	{
	case M_ALIGN_LEFT:
		break;
	case M_ALIGN_RIGHT:
		boxMin->x -= globalOffset;
		boxMax->x -= globalOffset;
		break;
	case M_ALIGN_CENTER:
		boxMin->x -= globalOffset*0.5f;
		boxMax->x -= globalOffset*0.5f;
		break;
	}
}

void MOText::prepare(void)
{
	MFont * font = getFont();
	const char * text = m_text.getData();
	if(! text)
	{
		m_boundingBox = MBox3d();
		return;
	}
	
	if(! (strlen(text) > 0 && font)){
		m_boundingBox = MBox3d();
		return;
	}

	MVector3 * min = &m_boundingBox.min;
	MVector3 * max = &m_boundingBox.max;

	(*min) = (*max) = MVector3(0, 0, 0);

	float tabSize = m_size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;
	float xc = 0, yc = 0;

	unsigned int charCode;
	unsigned int state = 0;
	unsigned char* s = (unsigned char*)text;
	for(; *s; ++s)
	{
		if(utf8_decode(&state, &charCode, *s) != UTF8_ACCEPT)
			continue;
		
		if(charCode == '\n') // return
		{
			min->y = MIN(min->y, yc-m_size*0.75f);
			yc += m_size;
			xc = 0;
			max->y = MAX(max->y, yc);
			continue;
		}

		if(charCode == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			max->x = MAX(max->x, xc);
			continue;
		}

		// get character
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
			continue;

		MVector2 scale = character->getScale();
		MVector2 offset = character->getOffset() * m_size;
	
		float width = scale.x * widthFactor * m_size;
		float height = scale.y * heightFactor * m_size;

		MVector2 charMin = MVector2(xc, yc) + offset;
		MVector2 charMax = charMin + MVector2(width, height);

		min->x = MIN(min->x, charMin.x);
		min->y = MIN(min->y, charMin.y);
		max->x = MAX(max->x, charMax.x);
		max->y = MAX(max->y, charMax.y);

		min->y = MIN(min->y, yc - m_size*0.75f);
		max->x = MAX(max->x, xc + character->getXAdvance()*m_size);

		//move to next character
		xc += character->getXAdvance()*m_size;
	}

	updateLinesOffset();
}

void MOText::updateVisibility(MOCamera * camera)
{
	MFrustum * frustum = camera->getFrustum();

	MVector3 * min = &m_boundingBox.min;
	MVector3 * max = &m_boundingBox.max;

	MVector3 points[8] = {
		getTransformedVector(MVector3(min->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, min->y, max->z)),
		getTransformedVector(MVector3(min->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, min->y, max->z))
	};

	// is box in frustum
	setVisible(frustum->isPointCloudVisible(points, 8));
}