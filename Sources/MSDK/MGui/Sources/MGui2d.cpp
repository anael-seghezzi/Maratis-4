/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui2d.cpp
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


MGui2d::MGui2d(void):
m_variableType(M_VARIABLE_NULL),
m_variablePointer(NULL),
m_isScrollAffect(true),
m_isVisible(true),
m_isHighLight(false),
m_isPressed(false),
m_isDeleted(false),
m_textMargin(0),
m_textObject(NULL),
m_parentWindow(NULL),
m_scale(16, 16),
m_normalTexture(NULL),
m_pressedTexture(NULL),
m_highLightTexture(NULL),
m_isDrawingText(true),
m_autoScaleFromText(true),
m_hasShadow(false),
m_shadowOpacity(0.1f),
m_shadowDir(8, 8),
m_userPointer(NULL)
{
	setNormalColor(MVector3(1, 1, 1));
	setHighLightColor(MVector3(1, 1, 1));
	setPressedColor(MVector3(1, 1, 1));
}

MGui2d::~MGui2d(void)
{}

MVector2 MGui2d::getAlignedTextPosition(void)
{
	MBox3d * box = m_textObject.getBoundingBox();
	
	if(box->min == box->max)
	{
		return MVector2(m_textMargin, getTextSize() + m_textMargin);
	}
	
	switch(m_textObject.getAlign())
	{
		default:
		case M_ALIGN_LEFT:
		{
			return MVector2(-box->min.x + m_textMargin, getTextSize() + m_textMargin);
		}
		case M_ALIGN_RIGHT:
		{
			return MVector2(-box->min.x + (m_scale.x - (box->max.x - box->min.x)) - m_textMargin, getTextSize() + m_textMargin);
		}
		case M_ALIGN_CENTER:
		{
			return MVector2(
				-box->min.x + (m_scale.x - (box->max.x - box->min.x))*0.5f,
				-box->min.y + (m_scale.y - (box->max.y - box->min.y))*0.5f
			);
		}
	}
}

void MGui2d::autoScaleFromText(void)
{
	if(! isDrawingText())
		return;

	MBox3d * box = m_textObject.getBoundingBox();
	m_scale.x = box->max.x - box->min.x;
	
	if(getTextAlign() == M_ALIGN_CENTER)
		m_scale.y = box->max.y - box->min.y;
	else
		m_scale.y = getTextSize() + box->max.y;
		
	m_scale += m_textMargin*2;
}

void MGui2d::setNormalTexture(MTextureRef * texture)
{
	m_normalTexture = texture;
	
	if(texture)
	{
		if(m_pressedTexture == NULL)
			m_pressedTexture = texture;

		if(m_highLightTexture == NULL)
			m_highLightTexture = texture;
	}
}

void MGui2d::setPressedTexture(MTextureRef * texture)
{
	m_pressedTexture = texture;
}

void MGui2d::setHighLightTexture(MTextureRef * texture)
{
	m_highLightTexture = texture;
}

void MGui2d::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	if(! isVisible())
		return;

	if(isPressed()) // pressed
	{  
		render->setColor4(getPressedColor());
		if(hasPressedTexture())
			drawTexturedQuad(getPressedTexture()->getTextureId());
		else
			drawQuad();
	}
	else if(isHighLight()) // highLight
	{
		render->setColor4(getHighLightColor());
		if(hasHighLightTexture())
			drawTexturedQuad(getHighLightTexture()->getTextureId());
		else
			drawQuad();
	}
	else // normal	
	{
		render->setColor4(getNormalColor());
		if(hasNormalTexture())
			drawTexturedQuad(getNormalTexture()->getTextureId());
		else
			drawQuad();
	}

	if(isDrawingText())
	{
		drawText();
	}

	// draw shadows
	if(hasShadow())
	{
		render->pushMatrix();
		render->translate(MVector3(getPosition().x, getPosition().y, 0));
		drawShadow();
		render->popMatrix();
	}
}

void MGui2d::drawShadow(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MVector2 vertices[12];
	MVector4 colors[12];

	render->disableTexture();
	render->disableNormalArray();
	render->disableTexCoordArray();
	render->enableVertexArray();
	render->enableColorArray();

	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->setColorPointer(M_FLOAT, 4, colors);
	
	MVector4 color0 = MVector4(0, 0, 0, 0);
	MVector4 color1 = MVector4(0, 0, 0, m_shadowOpacity);

	MVector2 dir = MVector2(ABS(m_shadowDir.x), ABS(m_shadowDir.y));

	static const unsigned short indices[30] = {
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		5, 6, 4,
		4, 6, 7,
		7, 9, 4,
		4, 9, 8,
		8, 9, 11,
		11, 10, 8
	};

	colors[0] = color0;
	vertices[0] = MVector2(m_scale.x, 0);
	colors[1] = color0;
	vertices[1] = MVector2(m_scale.x, 0) + MVector2(dir.x, 0);
	colors[2] = color1;
	vertices[2] = vertices[0] + MVector2(0, dir.y);
	colors[3] = color0;
	vertices[3] = vertices[0] + dir;
	colors[4] = color1;
	vertices[4] = m_scale;
	colors[5] = color0;
	vertices[5] = vertices[4] + MVector2(dir.x, 0);
	colors[6] = color0;
	vertices[6] = vertices[4] + dir;
	colors[7] = color0;
	vertices[7] = vertices[4] + MVector2(0, dir.y);
	colors[8] = color1;
	vertices[8] = vertices[4] + MVector2(-m_scale.x + dir.x, 0);
	colors[9] = color0;
	vertices[9] = vertices[8] + MVector2(0, dir.y);
	colors[10] = color0;
	vertices[10] = vertices[8] - MVector2(dir.x, 0);
	colors[11] = color0;
	vertices[11] = vertices[10] + MVector2(0, dir.y);
	
	if(m_shadowDir.x < 0)
	{
		for(int i=0; i<12; i++)
			vertices[i].x = m_scale.x - vertices[i].x;
	}
	
	if(m_shadowDir.y < 0)
	{
		for(int i=0; i<12; i++)
			vertices[i].y = m_scale.y - vertices[i].y;
	}
	
	render->drawElement(M_PRIMITIVE_TRIANGLES, 30, M_USHORT, indices);
}

MWindow * MGui2d::getRootWindow(void)
{
	if(m_parentWindow)
		return m_parentWindow->getRootWindow();

	else if(getType() == M_GUI_WINDOW)
		return ((MGuiWindow *)this)->getRootWindow();
	
	return NULL;
}

bool MGui2d::isMouseInside(const MVector2 & margin)
{
	MWindow * rootWindow = getRootWindow();
	if(rootWindow)
		return isPointInside(getPointLocalPosition(rootWindow->getMousePosition()), margin);
		
	return false;
}

bool MGui2d::isPointInside(const MVector2 & point, const MVector2 & margin)
{
	MVector2 position = getPosition() - margin;
	MVector2 scale = getScale() + margin*2;

	if(point.x >= position.x && point.x < (position.x + scale.x) &&
	   point.y >= position.y && point.y < (position.y + scale.y))
	   return true;

	return false;
}

void MGui2d::drawQuad(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MVector2 vertices[8];

	render->disableTexture();

	render->disableNormalArray();
	render->disableTexCoordArray();
	render->disableColorArray();
	render->enableVertexArray();

	vertices[0] = MVector2(m_position.x, m_position.y);
	vertices[1] = MVector2(m_position.x, m_position.y + m_scale.y);
	vertices[3] = MVector2(m_position.x + m_scale.x, m_position.y + m_scale.y);
	vertices[2] = MVector2(m_position.x + m_scale.x, m_position.y);

	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

void MGui2d::drawTexturedQuad(unsigned int textureId)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MVector2 vertices[8];
	MVector2 texCoords[8];

	render->enableTexture();
	
	render->disableNormalArray();
	render->disableColorArray();
	render->enableVertexArray();
	render->enableTexCoordArray();

	vertices[0] = MVector2(m_position.x, m_position.y);
	vertices[1] = MVector2(m_position.x, m_position.y + m_scale.y);
	vertices[3] = MVector2(m_position.x + m_scale.x, m_position.y + m_scale.y);
	vertices[2] = MVector2(m_position.x + m_scale.x, m_position.y);

	texCoords[0] = MVector2(0, 0);
	texCoords[1] = MVector2(0, 1);
	texCoords[3] = MVector2(1, 1);
	texCoords[2] = MVector2(1, 0);

	render->setTexCoordPointer(M_FLOAT, 2, texCoords);
	render->setVertexPointer(M_FLOAT, 2, vertices);

	render->bindTexture(textureId);
	render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

// draw text
void MGui2d::drawText(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MVector2 pos = getPosition() + getAlignedTextPosition();
	{
		// make sure the text is not filtered
		pos.x = (int)(pos.x+0.5f);
		pos.y = (int)(pos.y+0.5f);
	}
	
	render->pushMatrix();
	render->translate(pos);
	engine->getRenderer()->drawText(&m_textObject);
	render->popMatrix();
}

MVector2 MGui2d::getPointLocalPosition(const MVector2 & point)
{
	if(m_parentWindow)
	{
		if(isScrollAffected())
		{
			return (point - m_parentWindow->getPosition() - m_parentWindow->getScroll())/m_parentWindow->getZoom();
		}
		else
			return point - m_parentWindow->getPosition();
	}

	return point;
}
