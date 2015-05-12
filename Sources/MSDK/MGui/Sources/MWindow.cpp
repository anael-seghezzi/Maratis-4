/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWindow.cpp
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


#include <MEngine.h>
#include "../Includes/MGui.h"


MWindow::MWindow(int x, int y, unsigned int width, unsigned int height)
{
	m_drawCallback = NULL;
	m_eventCallback = NULL;
	m_currentKey = 0;
	m_currentMouseButton = 0;
	m_pos[0] = x;
	m_pos[1] = y;
	m_width = width;
	m_height = height;
	memset(m_keys, 0, sizeof(bool)*MWIN_MAX_KEYS);
	memset(m_mouseButtons, 0, sizeof(bool)*MWIN_MAX_MOUSE_BUTTONS);
}

MWindow::~MWindow(void)
{
	unsigned int i;
	unsigned int wSize = m_windows.size();
	for(i=0; i<wSize; i++)
		SAFE_DELETE(m_windows[i]);
		
	m_windows.clear();
}

void MWindow::clear(void)
{
	unsigned int i;
	unsigned int wSize = m_windows.size();
	for(i=0; i<wSize; i++)
		m_windows[i]->deleteMe();
}

MGuiWindow * MWindow::addNewWindow(void)
{
	MGuiWindow * window = new MGuiWindow(this);
	m_windows.push_back(window);
	return window;
}

void MWindow::update(void)
{
	unsigned int i, size = m_windows.size();
	for(i=0; i<size; i++)
		m_windows[i]->update();
		
	for(i=0; i<size; i++)
	{
		unsigned int id = (size-1)-i;
		if(m_windows[id]->isDeleted())
		{
			SAFE_DELETE(m_windows[id]);
			m_windows.erase(m_windows.begin() + id);
		}
	}
}

void MWindow::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	unsigned int i;
	unsigned int wSize = getWindowsNumber();

	// 2d viewport
	render->setViewport(0, 0, m_width, m_height);
	render->setMatrixMode(M_MATRIX_PROJECTION);
	render->loadIdentity();
	render->setOrthoView(0, (float)(m_width), (float)(m_height), 0, 1.0f, -1.0f);
	render->setMatrixMode(M_MATRIX_MODELVIEW);
	render->loadIdentity();

	// windows
	for(i=0; i<wSize; i++)
		m_windows[i]->draw();

	// menus
	for(i=0; i<wSize; i++)
		m_windows[i]->drawWindowMenus();
		
	// callback
	if(m_drawCallback)
		m_drawCallback(this);
}

bool MWindow::isSomethingEditing(void)
{
	unsigned int i;

	// windows
	unsigned int wSize = m_windows.size();
	for(i=0; i<wSize; i++)
	{
		if(m_windows[i]->isVisible() && m_windows[i]->isSomethingEditing())
			return true;
	}

	return false;
}

void MWindow::unHighLightAllWindows(void)
{
	unsigned int i;
	unsigned int wSize = m_windows.size();

	// windows
	for(i=0; i<wSize; i++)
		m_windows[i]->setHighLight(false);
}

void MWindow::onEvent(MWIN_EVENT_TYPE event)
{
	int i, j;
	int wSize = (int)m_windows.size();

	if(wSize == 0)
	{
		if(m_eventCallback)
			m_eventCallback(this, event);
		return;
	}

	// menus windows
	for(i=0; i<wSize; i++)
	{
		if((! m_windows[i]->isVisible()) || m_windows[i]->isIgnoringEvents())
			continue;

		int result = m_windows[i]->onWindowMenusEvent(this, event);
		if(result != 0)
		{
			if(m_eventCallback)
				m_eventCallback(this, event);
			return;
		}
	}

	// windows
	for(i=wSize-1; i>=0; i--)
	{
		if((! m_windows[i]->isVisible()) || m_windows[i]->isIgnoringEvents())
			continue;
			
		m_windows[i]->onEvent(this, event);

		// priority window
		if(event == MWIN_EVENT_MOUSE_MOVE && m_windows[i]->isHighLight())
		{
			for(j=i-1; j>=0; j--)
			{
				if((! m_windows[j]->isVisible()) || m_windows[j]->isIgnoringEvents())
					continue;

				m_windows[j]->onEvent(this, event);
				m_windows[j]->setHighLight(false);
			}

			break;
		}
	}
	
	if(m_eventCallback)
		m_eventCallback(this, event);
}