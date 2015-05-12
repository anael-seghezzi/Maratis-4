/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWinEvent.h
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


#ifndef _M_WIN_EVENTS_H
#define _M_WIN_EVENTS_H


#define MMOUSE_BUTTON_LEFT 0
#define MMOUSE_BUTTON_MIDDLE 2
#define MMOUSE_BUTTON_RIGHT 1

#define MKEY_BACKSPACE	128
#define MKEY_TAB		129
#define MKEY_RETURN		130
#define MKEY_PAUSE		131
#define MKEY_ESCAPE		132
#define MKEY_SPACE		133
#define MKEY_DELETE		134
#define MKEY_KP_0		135
#define MKEY_KP_1		136
#define MKEY_KP_2		137
#define MKEY_KP_3		138
#define MKEY_KP_4		139
#define MKEY_KP_5		140
#define MKEY_KP_6		141
#define MKEY_KP_7		142
#define MKEY_KP_8		143
#define MKEY_KP_9		144
#define MKEY_KP_ENTER	145
#define MKEY_KP_ADD		146
#define MKEY_KP_SUB		147
#define MKEY_KP_MUL     148
#define MKEY_KP_DIV     149
#define MKEY_KP_DECIMAL 150
#define MKEY_KP_EQUAL	151
#define MKEY_UP			152
#define MKEY_DOWN		153
#define MKEY_RIGHT		154
#define MKEY_LEFT		155
#define MKEY_INSERT		156
#define MKEY_HOME		157
#define MKEY_END		158
#define MKEY_PAGEUP		159
#define MKEY_PAGEDOWN	160
#define MKEY_F1			161
#define MKEY_F2			162
#define MKEY_F3			163
#define MKEY_F4			164
#define MKEY_F5			165
#define MKEY_F6			166
#define MKEY_F7			167
#define MKEY_F8			168
#define MKEY_F9			169
#define MKEY_F10		170
#define MKEY_F11		171
#define MKEY_F12		172
#define MKEY_CAPSLOCK	173
#define MKEY_NUMLOCK	174
#define MKEY_RSHIFT		175
#define MKEY_LSHIFT		176
#define MKEY_RCONTROL	177
#define MKEY_LCONTROL	178
#define MKEY_RALT		179
#define MKEY_LALT		180
#define MKEY_PRINT		181
#define MKEY_MENU		182
#define MKEY_RSUPER		183
#define MKEY_LSUPER		184
#define MKEY_DUMMY		255

enum MWIN_EVENT_TYPE
{
	MWIN_EVENT_CREATE,
	MWIN_EVENT_RESIZE,
	MWIN_EVENT_MOVE,
	MWIN_EVENT_CLOSE,
	MWIN_EVENT_DESTROY,
	
	MWIN_EVENT_KEY_DOWN,
	MWIN_EVENT_KEY_UP,
	MWIN_EVENT_CHAR,
	
	MWIN_EVENT_MOUSE_MOVE,
	MWIN_EVENT_MOUSE_SCROLL,
	MWIN_EVENT_MOUSE_BUTTON_DOWN,
	MWIN_EVENT_MOUSE_BUTTON_UP
};


#endif
