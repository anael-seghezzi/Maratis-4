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

#include <MEngine.h>
#include <MGui.h>
#include <GUI/MGUI.h>

#include "MEditor.h"
#include "MSyntax.h"


static int sh_is_white(int c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

static int sh_isalpha_special(int c)
{
	return isalpha(c) || c == '_';
}

static int sh_isalnum_special(int c)
{
	return isalnum(c) || c == '_';
}

static int sh_isquote(int c)
{
	return c == '\'' || c == '\"';
}

static int sh_compare_key(char *string, char *key, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if ((*string) != (*key))
			return 0;
		if (*string == 0 || *key == 0)
			return 0;
		string++;
		key++;
	}
	return 1;
}

void sh_select_word(const char *string, int id, int *begin, int *end)
{
	int i;
	int c = string[id];
	
	*begin = *end = id;

	/* number */
	if (isdigit(c) || c=='.') {
		
		i = id - 1;
		while (i >= 0) {
			int c2 = string[i];
			if (!isalnum(c2) && c2 != '.')
				break;
			i--;
		}
		*begin = i + 1;

		i = id + 1;
		while (i >= 0) {
			int c2 = string[i];
			if (!isalnum(c2) && c2 != '.')
				break;
			i++;
		}
		*end = i;
	}
	/* name */
	if (sh_isalnum_special(c)) {

		i = id - 1;
		while (i >= 0) {
			int c2 = string[i];
			if (!sh_isalnum_special(c2))
				break;
			i--;
		}
		*begin = MIN(*begin, i + 1);

		i = id + 1;
		while (i >= 0) {
			int c2 = string[i];
			if (!sh_isalnum_special(c2))
				break;
			i++;
		}
		*end = MAX(*end, i);
	}
	/* other */
	else {

		i = id - 1;
		while (i >= 0) {
			int c2 = string[i];
			if (c2 != c)
				break;
			i--;
		}
		*begin = MIN(*begin, i + 1);
		
		i = id + 1;
		while (i >= 0) {
			int c2 = string[i];
			if (c2 != c)
				break;
			i++;
		}
		*end = MAX(*end, i);
	}
}

static int sh_lua_compare_key(char *string, char *key, int len)
{
	char *next = string + len;
	return (
		sh_compare_key(string, key, len) &&
		(sh_is_white(*next) || !sh_isalnum_special(*next))
	);
}

static int sh_lua_instruction(char *string)
{
	if (sh_lua_compare_key(string, "and", 3)) return 3;
	if (sh_lua_compare_key(string, "break", 5)) return 5;
	if (sh_lua_compare_key(string, "do", 2)) return 2;
	if (sh_lua_compare_key(string, "else", 4)) return 4;
	if (sh_lua_compare_key(string, "elseif", 6)) return 6;
	if (sh_lua_compare_key(string, "end", 3)) return 3;
	if (sh_lua_compare_key(string, "false", 5)) return 5;
	if (sh_lua_compare_key(string, "for", 3)) return 3;
	if (sh_lua_compare_key(string, "function", 8)) return 8;
	if (sh_lua_compare_key(string, "if", 2)) return 2;
	if (sh_lua_compare_key(string, "in", 2)) return 2;
	if (sh_lua_compare_key(string, "local", 5)) return 5;
	if (sh_lua_compare_key(string, "nil", 3)) return 3;
	if (sh_lua_compare_key(string, "not", 3)) return 3;
	if (sh_lua_compare_key(string, "or", 2)) return 2;
	if (sh_lua_compare_key(string, "repeat", 6)) return 6;
	if (sh_lua_compare_key(string, "return", 6)) return 6;
	if (sh_lua_compare_key(string, "then", 4)) return 4;
	if (sh_lua_compare_key(string, "true", 4)) return 4;
	if (sh_lua_compare_key(string, "until", 5)) return 5;
	if (sh_lua_compare_key(string, "while", 5)) return 5;
	return 0;
}

void sh_lua_syntax(const char *string, map<unsigned int, MColor> *coloring)
{
	MEditor *editor = MEditor::getInstance();
	MPreferences *pref = editor->getPreferences();
	int mode = -1; // 0, comment, instruction, number, string, alphanum
	int i = 0;
	char *s = (char *)string;

	coloring->clear();

	for (; *s; s++, i++) {

		int res;

		// comment
		if (mode == 1) {
			if (*s != '\n')
				continue;
		}
		else if (sh_compare_key(s, "--", 2)) {
			(*coloring)[i] = MColor(pref->getColor("lua comment"));
			mode = 1;
			s++;
			i++;
			continue;
		}

		// string
		if (mode == 4) {
			if (! sh_isquote(*s))
				continue;
			else {
				mode = -1;
				continue;
			}
		}
		else if (sh_isquote(*s)) {
			(*coloring)[i] = MColor(pref->getColor("lua string"));
			mode = 4;
			continue;
		}

		// instruction
		res = sh_lua_instruction(s);
		if (mode != 5 && res > 0) {
			(*coloring)[i] = MColor(pref->getColor("lua instruction"));
			s += res - 1;
			i += res - 1;
			mode = 2;
			continue;
		}

		// alphanum
		if (mode == 5) {
			if (sh_isalnum_special(*s))
				continue;
		}
		else if (sh_isalpha_special(*s)) {
			mode = 5;
			continue;
		}

		// number
		if (mode == 3) {
			if (isdigit(*s))
				continue;
		}
		else if (isdigit(*s)) {
			(*coloring)[i] = MColor(pref->getColor("lua number"));
			mode = 3;
			continue;
		}

		// default
		if (mode != 0) {
			(*coloring)[i] = MColor(pref->getColor("lua default"));
			mode = 0;
		}
	}
}

void sh_lua_callback(MGuiEditText * editText, MGUI_EVENT_TYPE event)
{
	MWindow * rootWindow = editText->getRootWindow();

	if(event == MGUI_EVENT_ON_CHANGE)
	{
		sh_lua_syntax(editText->getText(), editText->getTextColoring());
	}
	else if(event == MGUI_EVENT_MOUSE_BUTTON_DOWN && rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_LEFT))
	{
		if(rootWindow->getPreviousClicElapsedTime() <= 500) // double click
		{
			int charId = (int)editText->getCharId();
			int begin, end;
			sh_select_word(editText->getText(), charId, &begin, &end);
			editText->setSelection(begin, end);
		}
	}
}
