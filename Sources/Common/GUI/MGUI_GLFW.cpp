/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MGUI.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include <tinycthread.h>
#include <GLFW/glfw3.h>

#include <MEngine.h>
#include <MGui.h>

#ifdef WIN32
#include <WinBase.h>
#endif

#include "MGUI.h"


// thread window
class MGLFWWindow : public MWindow
{
public:

	MGLFWWindow(GLFWwindow * glfwWin, int x, int y, unsigned int width, unsigned int height, MGUI_EVENT_CALLBACK):
		MWindow(x, y, width, height),
		init(false),
		running(true),
		paused(false),
		pause(false),
		glfwWindow(glfwWin)
	{
		setEventCallback(eventCallback);
	}

	~MGLFWWindow(void)
	{
		running = false;
		glfwMakeContextCurrent(glfwWindow);
		onEvent(MWIN_EVENT_DESTROY);
		glfwMakeContextCurrent(NULL);
		glfwDestroyWindow(glfwWindow);
		clear();
	}

	bool init;
	bool running;
	bool paused;
	bool pause;
	GLFWwindow * glfwWindow;
};


// glfw callbacks
static int getMaratisKey(int key)
{
	if(key >=32 && key<=126)
		return key;

	switch(key)
	{
	case GLFW_KEY_BACKSPACE:	return MKEY_BACKSPACE;
	case GLFW_KEY_TAB:			return MKEY_TAB;
	case GLFW_KEY_ENTER:		return MKEY_RETURN;
	case GLFW_KEY_PAUSE:		return MKEY_PAUSE;
	case GLFW_KEY_ESCAPE:		return MKEY_ESCAPE;
	case GLFW_KEY_SPACE:		return MKEY_SPACE;
	case GLFW_KEY_DELETE:		return MKEY_DELETE;
	case GLFW_KEY_KP_0:			return MKEY_KP_0;
	case GLFW_KEY_KP_1:			return MKEY_KP_1;
	case GLFW_KEY_KP_2:			return MKEY_KP_2;
	case GLFW_KEY_KP_3:			return MKEY_KP_3;
	case GLFW_KEY_KP_4:			return MKEY_KP_4;
	case GLFW_KEY_KP_5:			return MKEY_KP_5;
	case GLFW_KEY_KP_6:			return MKEY_KP_6;
	case GLFW_KEY_KP_7:			return MKEY_KP_7;
	case GLFW_KEY_KP_8:			return MKEY_KP_8;
	case GLFW_KEY_KP_9:			return MKEY_KP_9;
	case GLFW_KEY_KP_ENTER:		return MKEY_KP_ENTER;
	case GLFW_KEY_KP_ADD:		return MKEY_KP_ADD;
	case GLFW_KEY_KP_SUBTRACT:	return MKEY_KP_SUB;
	case GLFW_KEY_KP_MULTIPLY:	return MKEY_KP_MUL;
	case GLFW_KEY_KP_DIVIDE:	return MKEY_KP_DIV;
	case GLFW_KEY_KP_DECIMAL:	return MKEY_KP_DECIMAL;
	case GLFW_KEY_KP_EQUAL:		return MKEY_KP_EQUAL;
	case GLFW_KEY_UP:			return MKEY_UP;
	case GLFW_KEY_DOWN:			return MKEY_DOWN;
	case GLFW_KEY_RIGHT:		return MKEY_RIGHT;
	case GLFW_KEY_LEFT:			return MKEY_LEFT;
	case GLFW_KEY_INSERT:		return MKEY_INSERT;
	case GLFW_KEY_HOME:			return MKEY_HOME;
	case GLFW_KEY_END:			return MKEY_END;
	case GLFW_KEY_PAGE_UP:		return MKEY_PAGEUP;
	case GLFW_KEY_PAGE_DOWN:	return MKEY_PAGEDOWN;
	case GLFW_KEY_F1:			return MKEY_F1;
	case GLFW_KEY_F2:			return MKEY_F2;
	case GLFW_KEY_F3:			return MKEY_F3;
	case GLFW_KEY_F4:			return MKEY_F4;
	case GLFW_KEY_F5:			return MKEY_F5;
	case GLFW_KEY_F6:			return MKEY_F6;
	case GLFW_KEY_F7:			return MKEY_F7;
	case GLFW_KEY_F8:			return MKEY_F8;
	case GLFW_KEY_F9:			return MKEY_F9;
	case GLFW_KEY_F10:			return MKEY_F10;
	case GLFW_KEY_F11:			return MKEY_F11;
	case GLFW_KEY_F12:			return MKEY_F12;
	case GLFW_KEY_CAPS_LOCK:	return MKEY_CAPSLOCK;
	case GLFW_KEY_NUM_LOCK:		return MKEY_NUMLOCK;
	case GLFW_KEY_RIGHT_SHIFT:	return MKEY_RSHIFT;
	case GLFW_KEY_LEFT_SHIFT:	return MKEY_LSHIFT;
	case GLFW_KEY_RIGHT_CONTROL:return MKEY_RCONTROL;
	case GLFW_KEY_LEFT_CONTROL:	return MKEY_LCONTROL;
	case GLFW_KEY_RIGHT_ALT:	return MKEY_RALT;
	case GLFW_KEY_LEFT_ALT:		return MKEY_LALT;
	case GLFW_KEY_PRINT_SCREEN:	return MKEY_PRINT;
	case GLFW_KEY_MENU:			return MKEY_MENU;
	case GLFW_KEY_RIGHT_SUPER:	return MKEY_RSUPER;
	case GLFW_KEY_LEFT_SUPER:	return MKEY_LSUPER;
	default:					return MKEY_DUMMY;
	}
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	unsigned int mkey = getMaratisKey(key);
	if(mkey >= MWIN_MAX_KEYS)
		return;

	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);

	switch(action)
	{
	case GLFW_REPEAT:
	case GLFW_PRESS:
		rootWindow->onKeyDown(mkey);
		break;
	case GLFW_RELEASE:
		rootWindow->onKeyUp(mkey);
		break;
	default:
		break;
	}
}

static void char_callback(GLFWwindow * window, unsigned int key)
{
	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);
	rootWindow->onChar(key);
}

static void mousebutton_callback(GLFWwindow * window, int button, int action, int mods)
{
	if(button >= MWIN_MAX_MOUSE_BUTTONS)
		return;

	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);

	if(action == GLFW_PRESS)
		rootWindow->onMouseButtonDown(button);
	else if(action == GLFW_RELEASE)
		rootWindow->onMouseButtonUp(button);
}

static void cursorpos_callback(GLFWwindow * window, double x, double y)
{
	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);
	rootWindow->onMouseMove(MVector2(x, y));
}

static void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);
	rootWindow->onMouseScroll(MVector2(xoffset, yoffset));
}

static void close_callback(GLFWwindow * window)
{
	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);
	rootWindow->onClose();
}

static void size_callback(GLFWwindow * window, int width, int height)
{
	MGLFWWindow * rootWindow = (MGLFWWindow *)glfwGetWindowUserPointer(window);
	rootWindow->onResize(width, height);
}





// thread main
static void drawWindow(MGLFWWindow * window)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	if(! MGUI_isFocused() && window->init)
	{
		window->paused = window->pause;
		thrd_yield();
		return;
	}

	glfwMakeContextCurrent(window->glfwWindow);
	render->enableFrameBufferSRGB();

	if(window->running)
	{
		if(window->pause)
		{
			window->paused = true;
		}
		else
		{
			window->paused = false;
			render->disableScissorTest();
			render->setClearColor(MVector4(0, 0, 0, 0));
			render->clear(M_BUFFER_COLOR);

			window->draw();

			glfwSwapBuffers(window->glfwWindow);
			window->init = true;
		}
	}

	glfwMakeContextCurrent(NULL);
	thrd_yield();
}

static vector <MGLFWWindow *> windows;
/*
static thrd_t threadId = 0;
static bool running = false;
static int thread_main(void * data)
{
	while(running)
	{
		unsigned int i, wSize = windows.size();
		for(i=0; i<wSize; i++)
		{
			MGLFWWindow * window = windows[i];
			if(window)
				drawWindow(window);
		}
	}

    return 0;
}*/

bool MGUI_init(void)
{
	int init = glfwInit();
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	
	if(init != GL_TRUE)
		return false;

	/*if(thrd_create(&threadId, thread_main, NULL) != thrd_success)
	{
		glfwTerminate();
		return false;
	}

	running = true;*/
	return true;
}

MWindow * MGUI_createWindow(const char * title, int x, int y, unsigned int width, unsigned int height, MGUI_EVENT_CALLBACK)
{
	GLFWwindow * glfwWindow = NULL;

	if(windows.size() == 0)
	{
		glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
	}
	else
	{
		glfwWindow = glfwCreateWindow(width, height, title, NULL, windows[0]->glfwWindow);
	}

	if(glfwWindow)
	{
		glfwSetWindowPos(glfwWindow, x, y);
		glfwGetWindowPos(glfwWindow, &x, &y);
		glfwGetWindowSize(glfwWindow, (int*)&width, (int*)&height);
		glfwShowWindow(glfwWindow);

		MGLFWWindow * window = new MGLFWWindow(glfwWindow, x, y, width, height, eventCallback);
		windows.push_back(window);

		glfwMakeContextCurrent(glfwWindow);
		window->onCreate();
		glfwMakeContextCurrent(NULL);

		glfwSetWindowUserPointer(glfwWindow, window);
		glfwSetCursorPosCallback(glfwWindow, cursorpos_callback);
		glfwSetMouseButtonCallback(glfwWindow, mousebutton_callback);
		glfwSetScrollCallback(glfwWindow, scroll_callback);
		glfwSetKeyCallback(glfwWindow, key_callback);
		glfwSetCharCallback(glfwWindow, char_callback);
		glfwSetWindowCloseCallback(glfwWindow, close_callback);
		glfwSetWindowSizeCallback(glfwWindow, size_callback);

		return window;
	}

	return NULL;
}

MWindow * MGUI_getWindow(unsigned int id)
{
	return windows[id];
}

unsigned int MGUI_getWindowsNumber(void)
{
	return windows.size();
}

void MGUI_closeWindow(MWindow * window)
{
	MGLFWWindow * thWin = (MGLFWWindow *)window;
	thWin->running = false;
}

void MGUI_pauseWindow(MWindow * window)
{
	MGLFWWindow * thWin = (MGLFWWindow *)window;
	if(! thWin->running)
		return;

	thWin->pause = true;
	while(! thWin->paused)
		SLEEP(1);
}

void MGUI_unpauseWindow(MWindow * window)
{
	MGLFWWindow * thWin = (MGLFWWindow *)window;
	if(! thWin->running)
		return;

	thWin->pause = false;
	while(thWin->paused)
		SLEEP(1);
}

void MGUI_closeAllWindows(void)
{
	unsigned int i, wSize = windows.size();
	for(i=0; i<wSize; i++)
	{
		if(windows[i])
			MGUI_closeWindow(windows[i]);
	}
}

void MGUI_pauseAllWindows(void)
{
	unsigned int i, wSize = windows.size();
	for(i=0; i<wSize; i++)
	{
		if(windows[i])
		{
			MGLFWWindow * thWin = (MGLFWWindow *)windows[i];
			thWin->pause = true;
		}
	}

	for(i=0; i<wSize; i++)
	{
		if(windows[i])
		{
			MGLFWWindow * thWin = (MGLFWWindow *)windows[i];
			if(! thWin->running)
				continue;

			while(! thWin->paused)
				SLEEP(1);
		}
	}
}

void MGUI_unpauseAllWindows(void)
{
	unsigned int i, wSize = windows.size();
	for(i=0; i<wSize; i++)
	{
		if(windows[i])
		{
			MGLFWWindow * thWin = (MGLFWWindow *)windows[i];
			thWin->pause = false;
		}
	}

	for(i=0; i<wSize; i++)
	{
		if(windows[i])
		{
			MGLFWWindow * thWin = (MGLFWWindow *)windows[i];
			if(! thWin->running)
				continue;

			while(thWin->paused)
				SLEEP(1);
		}
	}
}

bool MGUI_update(void)
{
	glfwPollEvents();

	unsigned int i, wSize = windows.size();
	if(wSize == 0)
		return false;

	// main window
	MGLFWWindow * window0 = windows[0];
	if(! window0->running)
	{
		return false;
	}

	// secondary windows
	for(i=1; i<wSize; i++)
	{
		MGLFWWindow * window = windows[i];
		if(window)
		{
			if(! window->running)
				SAFE_DELETE(windows[i]);
		}
	}

	// update windows
	for(i=0; i<wSize; i++)
	{
		MGLFWWindow * window = windows[i];
		if(window)
			window->update();
	}

	// draw
	wSize = windows.size();
	for(i=0; i<wSize; i++)
	{
		MGLFWWindow * window = windows[i];
		if(window)
			drawWindow(window);
	}

	return true;
}

bool MGUI_isFocused(void)
{
	unsigned int i, wSize = windows.size();
	for(i=0; i<wSize; i++)
	{
		MGLFWWindow * window = windows[i];
		if(window)
		{
			if(glfwGetWindowAttrib(window->glfwWindow, GLFW_FOCUSED))
				return true;
		}
	}

	return false;
}

void MGUI_close(void)
{
	/*int result;
	running = false;
	thrd_join(threadId, &result);*/

	unsigned int i;
	unsigned int wSize = windows.size();
	if(wSize > 0)
	{
		// clear sub-windows
		for(i=1; i<wSize; i++)
			SAFE_DELETE(windows[i]);

		// clear main window
		SAFE_DELETE(windows[0]);
	}

	glfwTerminate();
}

double MGUI_getTime(void)
{
	return glfwGetTime();
}

void MGUI_setCurrentDirectory(const char * path)
{
	#ifdef WIN32
		SetCurrentDirectory(path);
	#else
		chdir(path);
	#endif
}
