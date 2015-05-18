/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MSyntax.h
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


#ifndef _M_SYNTAX
#define _M_SYNTAX

void sh_select_word(const char *string, int id, int *begin, int *end);
void sh_lua_syntax(const char *string, map<unsigned int, MColor> *coloring);
void sh_lua_callback(MGuiEditText * editText, MGUI_EVENT_TYPE event);

#endif