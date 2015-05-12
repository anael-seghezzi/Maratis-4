/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MImage.cpp
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


#include "../Includes/MCore.h"


MImage::MImage(void)
{
	m_raw = m_image_identity();
}

MImage::MImage(const MImage & image)
{
	m_raw = m_image_identity();
	copyFrom(image);
}

MImage::~MImage(void)
{
	clear();
}

void MImage::copyFrom(const MImage & image)
{
	m_image_copy(&m_raw, &image.m_raw);
}

void MImage::clear(void)
{
	m_image_destroy(&m_raw);
}

void MImage::create(M_TYPES dataType, unsigned int width, unsigned int height, unsigned int components)
{
	m_image_create(&m_raw, dataType, width, height, components);
}

void MImage::writePixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_raw.width * y) + x)*m_raw.comp;

	switch(m_raw.type)
	{
	case M_UBYTE:
		for(c=0; c<m_raw.comp; c++)
			((unsigned char *)m_raw.data)[pixelId+c] = ((unsigned char *)color)[c];
		break;
	case M_USHORT:
		for(c=0; c<m_raw.comp; c++)
			((unsigned short *)m_raw.data)[pixelId+c] = ((unsigned short *)color)[c];
		break;
	case M_INT:
		for(c=0; c<m_raw.comp; c++)
			((int *)m_raw.data)[pixelId+c] = ((int *)color)[c];
		break;
	case M_FLOAT:
		for(c=0; c<m_raw.comp; c++)
			((float *)m_raw.data)[pixelId+c] = ((float *)color)[c];
		break;
	default:
		break;
	}
}

void MImage::readPixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_raw.width * y) + x)*m_raw.comp;

	switch(m_raw.type)
	{
	case M_UBYTE:
		for(c=0; c<m_raw.comp; c++)
			((unsigned char *)color)[c] = ((unsigned char *)m_raw.data)[pixelId+c];
		break;
	case M_USHORT:
		for(c=0; c<m_raw.comp; c++)
			((unsigned short *)color)[c] = ((unsigned short *)m_raw.data)[pixelId+c];
		break;
	case M_INT:
		for(c=0; c<m_raw.comp; c++)
			((int *)color)[c] = ((int *)m_raw.data)[pixelId+c];
		break;
	case M_FLOAT:
		for(c=0; c<m_raw.comp; c++)
			((float *)color)[c] = ((float *)m_raw.data)[pixelId+c];
		break;
	default:
		break;
	}
}
