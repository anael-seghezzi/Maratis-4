/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPreferences.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
//  Mario Pispek, 2012
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


#ifndef _M_SHORTCUT_MANAGER_H
#define _M_SHORTCUT_MANAGER_H


// Shortcut
class M_EDITOR_EXPORT MShortcut
{
public:

	int key1;
	int key2;
	int key3;
	
	MShortcut(void):key1(-1), key2(-1), key3(-1){}
	bool isEngaged(MWindow * rootWindow);
};


// Preferences
class M_EDITOR_EXPORT MPreferences
{
public:

	// constructor
	MPreferences(void);
	~MPreferences(void);

private:

	// keys
	map<string, int> m_keys;
	
	// shortcuts
    map<string, MShortcut*> m_shortcuts;
	
	// theme
	unsigned int m_fontSize;
	string m_fontFilename;
	string m_guiPath;
	map<string, MVector4> m_colors;
	
private:

	// init
	void initKeys(void);
	void initShortcuts(void);
	void initThemes(void);
	
public:
    
	// keys
	int getKeyCode(const char * name);
	
	// loading
    void load(const char * filename);
	
	// shortcuts
	MShortcut * getShortcut(const char * name);
	bool isShortCutEngaged(MWindow * rootWindow, const char * name);
	
	// theme
	unsigned int getFontSize(void){ return m_fontSize; }
	const char * getFontFilename(void){ return m_fontFilename.c_str(); }
	const char * getGuiPath(void){ return m_guiPath.c_str(); }
	MVector4 getColor(const char * name);
};

#endif