/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MRenderArray.h
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

#ifndef _M_RENDER_ARRAY_H
#define _M_RENDER_ARRAY_H

void beginDraw(M_PRIMITIVE_TYPES primitiveType);
void pushVertex(const MVector3 & vertex);
void setColor(const MVector4 & color);
void setTexcoords(const MVector2 & texcoords);
void endDraw(MRenderingContext * render);

#endif