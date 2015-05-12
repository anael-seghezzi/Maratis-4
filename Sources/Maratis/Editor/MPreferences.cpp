/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPreferences.cpp
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


#include <tinyxml.h>
#include "MEditor.h"


static bool readUByteColor(TiXmlElement * node, const char * name, float * vector)
{
	const char * data = node->Attribute(name);
	if(data)
	{
		char * tok;
		const char filter[] = " \t";

		tok = strtok((char*)data, filter);

		unsigned int pass = 0;
		while((tok != NULL) && (pass < 4))
		{
			int value;
			sscanf(tok, "%d", &value);
			*vector = value/255.0f;

			tok = strtok(NULL, filter);
			vector++;
			pass++;
		}

		return true;
	}

	return false;
}

bool MShortcut::isEngaged(MWindow * rootWindow)
{
	if(key1>=0)
	{
		if(rootWindow->getKey() != key1)
			return false;
	}
	
	if(key2>=0)
	{
		if(! rootWindow->isKeyPressed(key2))
			return false;
	}
	
	if(key3>=0)
	{
		if(! rootWindow->isKeyPressed(key3))
			return false;
	}
 
	return true;
}

MPreferences::MPreferences(void):
m_fontSize(16),
m_fontFilename("fonts/GenR102.TTF"),
m_guiPath("gui")
{
	initKeys();
    initShortcuts();
	initThemes();
}

MPreferences::~MPreferences(void)
{
	map<string, MShortcut*>::iterator
		mit (m_shortcuts.begin()),
		mend(m_shortcuts.end());

	for(; mit!=mend; mit++)
	{
		SAFE_DELETE(mit->second);
	}
}

void MPreferences::initKeys(void)
{
    m_keys["Escape"] = MKEY_ESCAPE;
    m_keys["Return"] = MKEY_RETURN;
    m_keys["Enter"] = MKEY_KP_ENTER;
    m_keys["Tab"] = MKEY_TAB;
    m_keys["Backspace"] = MKEY_BACKSPACE;
    m_keys["Space"] = MKEY_SPACE;
    m_keys["Ctrl"] = MKEY_LCONTROL;
    m_keys["Super"] = MKEY_LSUPER;
    m_keys["Alt"] = MKEY_LALT;
    m_keys["Shift"] = MKEY_LSHIFT;
    m_keys["F1"] = MKEY_F1;
    m_keys["F2"] = MKEY_F2;
    m_keys["F3"] = MKEY_F3;
    m_keys["F4"] = MKEY_F4;
    m_keys["F5"] = MKEY_F5;
    m_keys["F6"] = MKEY_F6;
    m_keys["F7"] = MKEY_F7;
    m_keys["F8"] = MKEY_F8;
    m_keys["F9"] = MKEY_F9;
    m_keys["F10"] = MKEY_F10;
    m_keys["F11"] = MKEY_F11;
    m_keys["F12"] = MKEY_F12;
    m_keys["Print"] = MKEY_PRINT;
    m_keys["Pause"] = MKEY_PAUSE;
    m_keys["Insert"] = MKEY_INSERT;
    m_keys["Home"] = MKEY_HOME;
    m_keys["PageUp"] = MKEY_PAGEUP;
    m_keys["PageDown"] = MKEY_PAGEDOWN;
    m_keys["End"] = MKEY_END;
    m_keys["Delete"] = MKEY_DELETE;
    m_keys["NumLock"] = MKEY_NUMLOCK;
    m_keys["Num1"] = MKEY_KP_1;
    m_keys["Num2"] = MKEY_KP_2;
    m_keys["Num3"] = MKEY_KP_3;
    m_keys["Num4"] = MKEY_KP_4;
    m_keys["Num5"] = MKEY_KP_5;
    m_keys["Num6"] = MKEY_KP_6;
    m_keys["Num7"] = MKEY_KP_7;
    m_keys["Num8"] = MKEY_KP_8;
    m_keys["Num9"] = MKEY_KP_9;
    m_keys["Num0"] = MKEY_KP_0;
	m_keys["Add"] = MKEY_KP_ADD;
	m_keys["Sub"] = MKEY_KP_SUB;
	m_keys["Mul"] = MKEY_KP_MUL;
	m_keys["Div"] = MKEY_KP_DIV;
	m_keys["Decimal"] = MKEY_KP_DECIMAL;
	m_keys["Equal"] = MKEY_KP_EQUAL;
    m_keys["Left"] = MKEY_LEFT;
    m_keys["Right"] = MKEY_RIGHT;
    m_keys["Down"] = MKEY_DOWN;
    m_keys["Up"] = MKEY_UP;
}

int MPreferences::getKeyCode(const char * name)
{
	if(name)
	{
		if(strlen(name) == 1 && (name[0] >=32 && name[0]<=126))
			return name[0];

		map<string, int>::iterator iter = m_keys.find(name);
		if(iter != m_keys.end())
		{
			return iter->second;
		}
	}
	
	return -1;
}

void MPreferences::initShortcuts(void)
{
	// add default shortcuts
	MShortcut * sc;
	
	sc = new MShortcut();
    sc->key1 = getKeyCode("S");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Save"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Z");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Undo"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Y");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Redo"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("O");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Load Level"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Q");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Quit"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("D");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Duplicate"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("A");
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Select All"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Num5");
    m_shortcuts["Orthogonal View"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Num1");
    m_shortcuts["Face View"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Num3");
    m_shortcuts["Right View"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Num7");
    m_shortcuts["Top View"] = sc;

	sc = new MShortcut();
    sc->key1 = m_keys["Num9"];
    m_shortcuts["Bottom View"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("M");
    m_shortcuts["Transform Mouse"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("T");
    m_shortcuts["Transform Position"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("R");
    m_shortcuts["Transform Rotation"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("S");
    m_shortcuts["Transform Scale"] = sc;

	sc = new MShortcut();
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Multiple Selection"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("F");
    m_shortcuts["Focus Selection"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Backspace");
    m_shortcuts["Delete"] = sc;

	sc = new MShortcut();
    sc->key1 = getKeyCode("Delete");
    m_shortcuts["Delete2"] = sc;

	sc = new MShortcut();
    sc->key2 = getKeyCode("Ctrl");
    m_shortcuts["Pan View"] = sc;
}

void MPreferences::initThemes(void)
{
	m_colors["Head"] = MColor(45, 45, 45, 255);
	m_colors["Head Button"] = MColor(0, 0, 0, 0);
	m_colors["Head Button Highlight"] = MColor(56, 128, 180, 255);
	m_colors["Head Button Text"] = MColor(255, 255, 255, 255);
	
	m_colors["Menu"] = MColor(62, 62, 62, 216);
	m_colors["Menu Highlight"] = MColor(213, 221, 146, 191);
	m_colors["Menu Text"] = MColor(255, 255, 255, 255);
	
	m_colors["Edit Background"] = MColor(26, 37, 48, 255);
	m_colors["Edit Head"] = MColor(57, 130, 182, 255);
	m_colors["Edit Text"] = MColor(255, 255, 255, 255);
	m_colors["Edit Variable Text"] = MColor(163, 190, 219, 255);
	m_colors["Edit Component Text"] = MColor(213, 221, 146, 255);
	
	m_colors["Console"] = MColor(45, 45, 45, 255);
}

MShortcut * MPreferences::getShortcut(const char * name)
{
    map<string, MShortcut*>::iterator iter = m_shortcuts.find(name);
    if(iter != m_shortcuts.end())
    {
        return iter->second;
    }
    
    return NULL;
}

bool MPreferences::isShortCutEngaged(MWindow * rootWindow, const char * name)
{
	MShortcut * shortcut = getShortcut(name);
	if(shortcut)
	{
		return shortcut->isEngaged(rootWindow);
	}
	
	return false;
}

MVector4 MPreferences::getColor(const char * name)
{
	map<string, MVector4>::iterator iter = m_colors.find(name);
    if(iter != m_colors.end())
    {
        return iter->second;
    }
	
	return MVector4(1, 0, 1, 1);
}

void MPreferences::load(const char * filename)
{
	TiXmlDocument doc(filename);
	if(! doc.LoadFile())
        return;
		
	char directory[256], tmp[256];
	getDirectory(directory, filename);
		
    TiXmlHandle hDoc(&doc);
	TiXmlElement * rootNode;
	TiXmlHandle hRoot(0);

	// Maratis
	rootNode = hDoc.FirstChildElement().Element();
	if(! rootNode)
		return;

	if(strcmp(rootNode->Value(), "Maratis") != 0)
		return;

	hRoot = TiXmlHandle(rootNode);

	// Preferences
	TiXmlElement * preferencesNode = rootNode->FirstChildElement("Preferences");
	if(! preferencesNode)
		return;

	// Shortcuts
	TiXmlElement * shortcutsNode = preferencesNode->FirstChildElement("Shortcuts");
	if(shortcutsNode)
	{
		// Shortcut
		TiXmlElement * shortcutNode = shortcutsNode->FirstChildElement("Shortcut");
		if(shortcutNode)
		{
			do {
	
				const char * name = shortcutNode->Attribute("name");
				if(! name)
					continue;
		
				MShortcut * shortcut = getShortcut(name);
				if(! shortcut)
				{
					shortcut = new MShortcut();
					m_shortcuts[name] = shortcut;
				}
		
				const char * key1 = shortcutNode->Attribute("key1");
				const char * key2 = shortcutNode->Attribute("key2");
				const char * key3 = shortcutNode->Attribute("key3");
		
				if(key1)
					m_shortcuts[name]->key1 = getKeyCode(key1);
				if(key2)
					m_shortcuts[name]->key2 = getKeyCode(key2);
				if(key3)
					m_shortcuts[name]->key3 = getKeyCode(key3);
		
			} while ((shortcutNode = shortcutNode->NextSiblingElement()));
		}
	}
	
	// Themes
	TiXmlElement * themeNode = preferencesNode->FirstChildElement("Theme");
	if(themeNode)
	{
		// guiPath
		const char * guiPath = themeNode->Attribute("guiPath");
		if(guiPath)
		{
			getGlobalFilename(tmp, directory, guiPath);
			m_guiPath = tmp;
		}
		
		// fontFilename
		const char * fontFilename = themeNode->Attribute("fontFilename");
		if(fontFilename)
		{
			getGlobalFilename(tmp, directory, fontFilename);
			m_fontFilename = tmp;
		}
			
		// fontSize
		themeNode->QueryUIntAttribute("fontSize", &m_fontSize);
		
		// Theme
		TiXmlElement * colorNode = themeNode->FirstChildElement("Color");
		if(colorNode)
		{
			do {
	
				const char * name = colorNode->Attribute("name");
				if(! name)
					continue;
		
				MVector4 color(1, 0, 1, 1);
				if(readUByteColor(colorNode, "value", color))
				{
					m_colors[name] = color;
				}
		
			} while ((colorNode = colorNode->NextSiblingElement()));
		}
	}
}