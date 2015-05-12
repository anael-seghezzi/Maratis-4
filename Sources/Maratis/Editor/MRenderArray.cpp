/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MRenderArray.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#include <MCore.h>
#include "MRenderArray.h"


#define M_MAX_ARRAY 100000

static M_PRIMITIVE_TYPES g_primitiveType;
static unsigned int g_verticesNumber;
static MVector3 g_vertices[M_MAX_ARRAY];
static MVector4 g_colors[M_MAX_ARRAY];
static MVector2 g_texCoords[M_MAX_ARRAY];

static MVector4 previousColor(1, 1, 1, 1);
static MVector2 previousTexcoords(0, 0);
static bool renderColors = false;
static bool renderTexcoords = false;

void beginDraw(M_PRIMITIVE_TYPES primitiveType)
{
	g_verticesNumber = 0;
	g_primitiveType = primitiveType;
	renderColors = false;
	renderTexcoords = false;
	previousColor = MVector4(1, 1, 1, 1);
	previousTexcoords = MVector2(0, 0);
}

void setColor(const MVector4 & color)
{
	previousColor = color;
	renderColors = true;
}

void setTexcoords(const MVector2 & texcoords)
{
	previousTexcoords = texcoords;
	renderTexcoords = true;
}

void pushVertex(const MVector3 & vertex)
{
	if(g_verticesNumber < M_MAX_ARRAY)
	{
		g_vertices[g_verticesNumber] = vertex;
		g_colors[g_verticesNumber] = previousColor;
		g_texCoords[g_verticesNumber] = previousTexcoords;
		g_verticesNumber++;
	}
}

void endDraw(MRenderingContext * render)
{
	if(g_verticesNumber == 0)
		return;
	
	if(renderColors)
	{
		render->enableColorArray();
		render->setColorPointer(M_FLOAT, 4, g_colors);
	}
	else
	{
		render->disableColorArray();
	}
	
	if(renderTexcoords)
	{
		render->enableTexCoordArray();
		render->setTexCoordPointer(M_FLOAT, 2, g_texCoords);
	}
	else
	{
		render->disableTexCoordArray();
	}
	
	render->disableNormalArray();
	render->enableVertexArray();

	render->setVertexPointer(M_FLOAT, 3, g_vertices);
	render->drawArray(g_primitiveType, 0, g_verticesNumber);
}

