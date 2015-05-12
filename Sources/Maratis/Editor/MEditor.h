/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MEditor.h
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


#ifndef _MARATIS_EDITOR_H
#define _MARATIS_EDITOR_H

#ifdef WIN32

	// M_EDITOR_EXPORT
	#if defined(MEDITOR_DLL)
		#define M_EDITOR_EXPORT __declspec( dllexport )
	#elif defined(MEDITOR_STATIC)
		#define M_EDITOR_EXPORT
	#else
		#define M_EDITOR_EXPORT __declspec( dllimport )
	#endif

#else

	// M_EDITOR_EXPORT
	#define M_EDITOR_EXPORT

#endif


#include <MEngine.h>
#include <MGui.h>

#include "MViewport.h"
#include "MUserView.h"
#include "MV3dView.h"
#include "MV3dEdit.h"
#include "MPreferences.h"
#include "MUIConstants.h"
#include "MSelectionManager.h"


class M_EDITOR_EXPORT MEditor
{
public:

	// constructor / destructor
	MEditor(void);
	~MEditor(void);

	// instance
	static MEditor * getInstance(void)
	{
		static MEditor m_instance;
		return &m_instance;
	}

private:
	
	MLevel m_guiData;
	MPreferences m_preferences;
	MSelectionManager m_selectionManager;
	
public:

	void init(void);
	void clear(void);
	
	inline MPreferences * getPreferences(void){ return &m_preferences; }
	inline MLevel * getGuiData(void){ return &m_guiData; }
	inline MSelectionManager * getSelectionManager(void){ return &m_selectionManager; }
	
	// windows
	MWindow * createWindow(const char * title, int x, int y, unsigned int width, unsigned int height, void (* eventCallback)(MWindow * rootWindow, MWIN_EVENT_TYPE event) = NULL);
	void closeWindow(MWindow * window);
};

#endif