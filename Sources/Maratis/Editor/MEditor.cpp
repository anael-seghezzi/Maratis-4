/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MEditor.cpp
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


#include "MEditor.h"

#include <Loaders/Mesh/MMeshLoad.h>
#include <GUI/MGUI.h>


MEditor::MEditor(void)
{}

MEditor::~MEditor(void)
{
	clear();
}

void MEditor::init(void)
{
	clear();
	
	// load preferences
	{
		m_preferences.load("Resources/preferences.xml");
		m_preferences.load("Resources/themes/grey/grey.xml");
	}
	
	// load font
	{
		const char * filename = m_preferences.getFontFilename();
		m_guiData.loadFont(filename);
	}
	
	// load gui
	{
		const char * path = m_preferences.getGuiPath();

		char tmp[256];
		vector<string> files;
		readDirectory(path, &files);
		
		unsigned int size = files.size();
		for(unsigned int i=0; i<size; i++)
		{
			getGlobalFilename(tmp, path, files[i].c_str());
			MTextureRef * ref = m_guiData.loadTexture(tmp);
			ref->setFilename(files[i].c_str()); // use only the file name
		}
	}
	
	// load meshes
	{
		const char * path = "Resources/meshes/gui";

		char tmp[256];
		vector<string> files;
		readDirectory(path, &files);
		
		unsigned int size = files.size();
		for(unsigned int i=0; i<size; i++)
		{
			getGlobalFilename(tmp, path, files[i].c_str());
			MMeshRef * ref = m_guiData.loadMesh(tmp);
			ref->setFilename(files[i].c_str()); // use only the file name
		}
	}
}

void MEditor::clear(void)
{
	m_guiData.clear();
}

MWindow * MEditor::createWindow(const char * title, int x, int y, unsigned int width, unsigned int height, void (* eventCallback)(MWindow * rootWindow, MWIN_EVENT_TYPE event))
{
	return MGUI_createWindow(title, x, y, width, height, eventCallback);
}

void MEditor::closeWindow(MWindow * window)
{
	MGUI_closeWindow(window);
}