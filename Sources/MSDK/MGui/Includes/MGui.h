/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2013 Anael Seghezzi <www.maratis3d.com>
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


#ifndef _M_GUI_H
#define _M_GUI_H


#ifdef WIN32

	// M_GUI_EXPORT
	#if defined(MGUI_DLL)
		#define M_GUI_EXPORT __declspec( dllexport )
	#elif defined(MGUI_STATIC)
		#define M_GUI_EXPORT
	#else
		#define M_GUI_EXPORT __declspec( dllimport )
	#endif

#else

	// M_GUI_EXPORT
	#define M_GUI_EXPORT

#endif

#define M_GUI_WINDOW	1
#define M_GUI_IMAGE		2
#define M_GUI_TEXT		3
#define M_GUI_BUTTON	4
#define M_GUI_MENU		5
#define M_GUI_EDIT_TEXT 6
#define M_GUI_SLIDE		7
#define M_GUI_NODE		8

class MGuiWindow;
class MGuiImage;
class MGuiText;
class MGuiButton;
class MGuiMenu;
class MGuiEditText;
class MGuiSlide;
class MGuiNode;

#include "MWinEvents.h"
#include "MWindow.h"
#include "MGuiEvents.h"
#include "MGui2d.h"
#include "MGuiButton.h"
#include "MGuiSlide.h"
#include "MGuiNode.h"
#include "MGuiWindow.h"
#include "MGuiMenu.h"
#include "MGuiText.h"
#include "MGuiImage.h"
#include "MGuiEditText.h"
#include "MGuiFileBrowser.h"
#include "MGuiColorPicker.h"

#endif
