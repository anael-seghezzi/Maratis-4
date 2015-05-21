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

#include <SDL.h>
#ifdef WIN32
#include <Windows.h>
#include <WinBase.h>
#endif

#include <MEngine.h>
#include <MGui.h>
#include <tinyutf8.h>

#include "MGUI.h"

static SDL_GLContext g_sdlContext = NULL;


// thread window
class MSDLWindow : public MWindow
{
public:

    MSDLWindow(SDL_Window * sdlWin, int x, int y, unsigned int width, unsigned int height, MGUI_EVENT_CALLBACK):
        MWindow(x, y, width, height),
        init(false),
        running(true),
        paused(false),
        pause(false),
        focus(false),
        sdlWindow(sdlWin)
    {
        setEventCallback(eventCallback);
    }

    ~MSDLWindow(void)
    {
        running = false;
        onEvent(MWIN_EVENT_DESTROY);
        SDL_DestroyWindow(sdlWindow);
        clear();
    }

    bool init;
    bool running;
    bool paused;
    bool pause;
    bool focus;
    SDL_Window * sdlWindow;
};


// glfw callbacks
static int getMaratisKey(int key)
{
    if(key >=32 && key<=126)
        return key;

    switch(key)
    {
    case SDLK_BACKSPACE:	return MKEY_BACKSPACE;
    case SDLK_TAB:			return MKEY_TAB;
    case SDLK_RETURN:		return MKEY_RETURN;
    case SDLK_PAUSE:		return MKEY_PAUSE;
    case SDLK_ESCAPE:		return MKEY_ESCAPE;
    case SDLK_SPACE:		return MKEY_SPACE;
    case SDLK_DELETE:		return MKEY_DELETE;
    case SDLK_KP_0:			return MKEY_KP_0;
    case SDLK_KP_1:			return MKEY_KP_1;
    case SDLK_KP_2:			return MKEY_KP_2;
    case SDLK_KP_3:			return MKEY_KP_3;
    case SDLK_KP_4:			return MKEY_KP_4;
    case SDLK_KP_5:			return MKEY_KP_5;
    case SDLK_KP_6:			return MKEY_KP_6;
    case SDLK_KP_7:			return MKEY_KP_7;
    case SDLK_KP_8:			return MKEY_KP_8;
    case SDLK_KP_9:			return MKEY_KP_9;
    case SDLK_KP_ENTER:		return MKEY_KP_ENTER;
    case SDLK_KP_PLUS:		return MKEY_KP_ADD;
    case SDLK_KP_MINUS:     return MKEY_KP_SUB;
    case SDLK_KP_MULTIPLY:	return MKEY_KP_MUL;
    case SDLK_KP_DIVIDE:	return MKEY_KP_DIV;
    case SDLK_KP_DECIMAL:	return MKEY_KP_DECIMAL;
    case SDLK_KP_EQUALS:	return MKEY_KP_EQUAL;
    case SDLK_UP:			return MKEY_UP;
    case SDLK_DOWN:			return MKEY_DOWN;
    case SDLK_RIGHT:		return MKEY_RIGHT;
    case SDLK_LEFT:			return MKEY_LEFT;
    case SDLK_INSERT:		return MKEY_INSERT;
    case SDLK_HOME:			return MKEY_HOME;
    case SDLK_END:			return MKEY_END;
    case SDLK_PAGEUP:		return MKEY_PAGEUP;
    case SDLK_PAGEDOWN:     return MKEY_PAGEDOWN;
    case SDLK_F1:			return MKEY_F1;
    case SDLK_F2:			return MKEY_F2;
    case SDLK_F3:			return MKEY_F3;
    case SDLK_F4:			return MKEY_F4;
    case SDLK_F5:			return MKEY_F5;
    case SDLK_F6:			return MKEY_F6;
    case SDLK_F7:			return MKEY_F7;
    case SDLK_F8:			return MKEY_F8;
    case SDLK_F9:			return MKEY_F9;
    case SDLK_F10:			return MKEY_F10;
    case SDLK_F11:			return MKEY_F11;
    case SDLK_F12:			return MKEY_F12;
    case SDLK_CAPSLOCK:     return MKEY_CAPSLOCK;
    case SDLK_NUMLOCKCLEAR:	return MKEY_NUMLOCK;
    case SDLK_RSHIFT :      return MKEY_RSHIFT;
    case SDLK_LSHIFT:       return MKEY_LSHIFT;
    case SDLK_RCTRL:        return MKEY_RCONTROL;
    case SDLK_LCTRL:        return MKEY_LCONTROL;
    case SDLK_RALT:         return MKEY_RALT;
    case SDLK_LALT:         return MKEY_LALT;
    case SDLK_PRINTSCREEN:	return MKEY_PRINT;
    case SDLK_MENU:			return MKEY_MENU;
    case SDLK_RGUI:         return MKEY_RSUPER;
    case SDLK_LGUI:         return MKEY_LSUPER;
    default:				return MKEY_DUMMY;
    }
}

static void key_callback(MSDLWindow * rootWindow, SDL_Keycode key, unsigned int action)
{
    unsigned int mkey = getMaratisKey((int)key);
    if(mkey >= MWIN_MAX_KEYS)
        return;

    switch(action)
    {
    case SDL_KEYDOWN:
        rootWindow->onKeyDown(mkey);
        break;
    case SDL_KEYUP:
        rootWindow->onKeyUp(mkey);
        break;
    default:
        break;
    }
}

static void char_callback(MSDLWindow * rootWindow, unsigned int key)
{
    rootWindow->onChar(key);
}

static void mousebutton_callback(MSDLWindow * rootWindow, int button, int action)
{
    if(button >= MWIN_MAX_MOUSE_BUTTONS)
        return;

    switch(action)
    {
    case SDL_MOUSEBUTTONDOWN:
        rootWindow->onMouseButtonDown(button);
        break;
    case SDL_MOUSEBUTTONUP:
        rootWindow->onMouseButtonUp(button);
        break;
    default:
        break;
    }
}

static void cursorpos_callback(MSDLWindow * rootWindow, double x, double y)
{
    rootWindow->onMouseMove(MVector2(x, y));
}

static void scroll_callback(MSDLWindow * rootWindow, double xoffset, double yoffset)
{
    rootWindow->onMouseScroll(MVector2(xoffset, yoffset));
}

static void close_callback(MSDLWindow * rootWindow)
{
    MLOG(6, "SDL Window Event Close");

    rootWindow->onClose();
}

static void size_callback(MSDLWindow * rootWindow, int width, int height)
{
    MLOG(6, "SDL Window Event Resize");

    rootWindow->onResize(width, height);
}

static void event_callback(MSDLWindow * rootWindow, SDL_Event event)
{
    //MLOG(6, "SDL Event : " << event.type);

    unsigned int type = event.type;
    switch (type)
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        key_callback(rootWindow, event.key.keysym.sym, type);
        break;
	case SDL_TEXTINPUT:
		{
			unsigned int charCode;
			unsigned int state = 0;
			unsigned char* s = (unsigned char *)event.edit.text;
			for(; *s; ++s)
			{
				if(utf8_decode(&state, &charCode, *s) == UTF8_ACCEPT)
					char_callback(rootWindow, charCode);
			}
		}
		break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
		switch(event.button.button)
		{
		case 1:
			mousebutton_callback(rootWindow, MMOUSE_BUTTON_LEFT, type);
			break;
		case 2:
			mousebutton_callback(rootWindow, MMOUSE_BUTTON_MIDDLE, type);
			break;
		case 3:
			mousebutton_callback(rootWindow, MMOUSE_BUTTON_RIGHT, type);
			break;
		default:
			mousebutton_callback(rootWindow, event.button.button - 1, type);
		}
        break;
    case SDL_MOUSEMOTION:
        cursorpos_callback(rootWindow, event.motion.x, event.motion.y);
        break;
    case SDL_MOUSEWHEEL:
        scroll_callback(rootWindow, event.wheel.x, event.wheel.y);
        break;
    case SDL_WINDOWEVENT:
    {
        //MLOG(6, "SDL Window Event : " << (event.window.event + 0));

        switch(event.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            close_callback(rootWindow);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            size_callback(rootWindow, event.window.data1, event.window.data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_GL_MakeCurrent(rootWindow->sdlWindow, g_sdlContext);
            break;
        case SDL_WINDOWEVENT_SHOWN:
            rootWindow->focus = true;
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            rootWindow->focus = false;
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

// thread main
static void drawWindow(MSDLWindow * window)
{
    MEngine * engine = MEngine::getInstance();
    MRenderingContext * render = engine->getRenderingContext();

    if(! MGUI_isFocused() && window->init)
    {
        window->paused = window->pause;
        //thrd_yield();
		SDL_Delay(0);
        return;
    }

    SDL_GL_MakeCurrent(window->sdlWindow, g_sdlContext);

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

            SDL_GL_SwapWindow(window->sdlWindow);
            window->init = true;
        }
    }

    SDL_GL_MakeCurrent(NULL, NULL);
    //thrd_yield();
	SDL_Delay(0);
}

static vector <MSDLWindow *> windows;

static int app_event_callback(void * userData, SDL_Event * event)
{
    MLOG(6, "SDL App Event : " << event->type);

    MSDLWindow * window = windows[0];
    unsigned int type = event->type;
    switch (type)
    {
    case SDL_APP_DIDENTERFOREGROUND:
        //android and ios app enter foreground
        MGUI_unpauseWindow(window);
        return 0;
    case SDL_APP_DIDENTERBACKGROUND:
        //android and ios app enter background
        MGUI_pauseWindow(window);
        return 0;
    default:
        return 1;
    }
}

bool MGUI_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

	SDL_StartTextInput();
	//SDL_CaptureMouse(1); SDL 2.04
    return true;
}

MWindow * MGUI_createWindow(const char * title, int x, int y, unsigned int width, unsigned int height, MGUI_EVENT_CALLBACK)
{
    SDL_Window * sdlWindow = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_OPENGL);
    if (sdlWindow == NULL)
    {
        return NULL;
    }

	if (g_sdlContext == NULL)
	{
		g_sdlContext = SDL_GL_CreateContext(sdlWindow);
		if (g_sdlContext == NULL)
		{
			SDL_DestroyWindow(sdlWindow);
			return NULL;
		}
	}

    MSDLWindow * window = new MSDLWindow(sdlWindow, x, y, width, height, eventCallback);
    windows.push_back(window);

    window->onCreate();

    return window;
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
    MSDLWindow * thWin = (MSDLWindow *)window;
    thWin->running = false;
}

void MGUI_pauseWindow(MWindow * window)
{
    MSDLWindow * thWin = (MSDLWindow *)window;
    if(! thWin->running)
        return;

    thWin->pause = true;
    while(! thWin->paused)
		SDL_Delay(1);
}

void MGUI_unpauseWindow(MWindow * window)
{
    MSDLWindow * thWin = (MSDLWindow *)window;
    if(! thWin->running)
        return;

    thWin->pause = false;
    while(thWin->paused)
		SDL_Delay(1);
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
            MSDLWindow * thWin = (MSDLWindow *)windows[i];
            thWin->pause = true;
        }
    }

    for(i=0; i<wSize; i++)
    {
        if(windows[i])
        {
            MSDLWindow * thWin = (MSDLWindow *)windows[i];
            if(! thWin->running)
                continue;

            while(! thWin->paused)
				SDL_Delay(1);
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
            MSDLWindow * thWin = (MSDLWindow *)windows[i];
            thWin->pause = false;
        }
    }

    for(i=0; i<wSize; i++)
    {
        if(windows[i])
        {
            MSDLWindow * thWin = (MSDLWindow *)windows[i];
            if(! thWin->running)
                continue;

            while(thWin->paused)
				SDL_Delay(1);
        }
    }
}

bool MGUI_update(void)
{
    unsigned int i, wSize = windows.size();
    if(wSize == 0)
        return false;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        for(i=0; i<wSize; i++)
        {
            MSDLWindow * window = windows[i];
			if(window)
			{
				if(SDL_GetWindowID(window->sdlWindow) == e.window.windowID)
					event_callback(window, e);
			}
        }
    }

    // main window
    MSDLWindow * window0 = windows[0];
    if(! window0->running)
    {
        return false;
    }

    // secondary windows
    for(i=1; i<wSize; i++)
    {
        MSDLWindow * window = windows[i];
        if(window)
        {
            if(! window->running)
                SAFE_DELETE(windows[i]);
        }
    }

    // update windows
    for(i=0; i<wSize; i++)
    {
        MSDLWindow * window = windows[i];
        if(window)
            window->update();
    }

    // draw
    wSize = windows.size();
    for(i=0; i<wSize; i++)
    {
        MSDLWindow * window = windows[i];
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
        MSDLWindow * window = windows[i];
        if(window)
        {
            return window->focus;
        }
    }

    return false;
}

void MGUI_close(void)
{
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

	SDL_GL_DeleteContext(g_sdlContext);
    SDL_Quit();
}

double MGUI_getTime(void)
{
    return SDL_GetTicks() / 1000.0;
}

void MGUI_setCurrentDirectory(const char * path)
{
    #ifdef WIN32
        SetCurrentDirectory(path);
    #else
        chdir(path);
    #endif
}
