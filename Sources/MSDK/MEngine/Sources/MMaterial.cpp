/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaterial.cpp
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


#include "../Includes/MEngine.h"


MMaterial::MMaterial(void):
m_type(0),
m_FXRef(NULL),
m_ZFXRef(NULL),
m_blendMode(M_BLENDING_NONE),
m_opacity(1),
m_shininess(0),
m_customValue(0),
m_diffuse(1, 1, 1),
m_specular(0, 0, 0),
m_emit(0, 0, 0),
m_customColor(0, 0, 0),
m_texturesPassNumber(0),
m_texturesPass(NULL)
{}

MMaterial::MMaterial(const MMaterial & material):
m_type(material.m_type),
m_FXRef(material.m_FXRef),
m_ZFXRef(material.m_ZFXRef),
m_blendMode(material.m_blendMode),
m_opacity(material.m_opacity),
m_shininess(material.m_shininess),
m_customValue(material.m_customValue),
m_diffuse(material.m_diffuse),
m_specular(material.m_specular),
m_emit(material.m_emit),
m_customColor(material.m_customColor),
m_texturesPassNumber(0),
m_texturesPass(NULL)
{
	allocTexturesPass(m_texturesPassNumber);

	unsigned int i;
	for(i=0; i<material.m_texturesPassNumber; i++)
		addTexturePass(
		material.m_texturesPass[i]->getTexture(),
		material.m_texturesPass[i]->getCombineMode(),
		material.m_texturesPass[i]->getMapChannel()
		);
}

MMaterial::~MMaterial(void)
{
	clearTexturesPass();
}

void MMaterial::allocTexturesPass(unsigned int size)
{
	clearTexturesPass();
	if(size == 0)
		return;
	m_texturesPass = (MTexturePass **) malloc(sizeof(void *)*size);
}

void MMaterial::clearTexturesPass(void)
{
	unsigned int i;
	for(i=0; i<m_texturesPassNumber; i++)
		SAFE_DELETE(m_texturesPass[i]);

	m_texturesPassNumber = 0;
	SAFE_FREE(m_texturesPass);
}

void MMaterial::addTexturePass(MTexture * texture, M_TEX_COMBINE_MODES combineMode, unsigned int mapChannel)
{
	m_texturesPass[m_texturesPassNumber] = new MTexturePass(texture, combineMode, mapChannel);
	m_texturesPassNumber++;
}