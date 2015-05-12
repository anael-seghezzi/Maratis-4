/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// main.cpp
//
// GUI test
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
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

// MSDK
#include <MEngine.h>
#include <MGui.h>

// Common
#include <GUI/MGUI.h>
#include <Contexts/GL/MGLContext.h>
#include <Loaders/FreeImage/MFreeImageLoader.h>
#include <Loaders/Freetype/MFreetypeLoader.h>
#include <Renderers/Fixed/MFixedRenderer.h>
#include <Renderers/Standard/MStandardRenderer.h>


// test data
static MLevel * guiData = NULL;
static MRenderingContext * render = NULL;
static MRenderer * renderer = NULL;

void winEvents(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEngine * engine = MEngine::getInstance();
	
	switch(event)
	{
	case MWIN_EVENT_CREATE:
		{
			// create a rendering context
			render = new MGLContext();
			engine->setRenderingContext(render);
			
			// create a renderer
			renderer = new MStandardRenderer();
			engine->setRenderer(renderer);
			
			// create a level to handle the gui data
			guiData = new MLevel();
		
			
			// create a gui window (sub-window of rootWindow handled by MGui)
			MGuiWindow * gw = rootWindow->addNewWindow();
			gw->setScale(MVector2(800, 600));
			gw->setColor(MVector3(0.5f, 0.5f, 0.5f));
			
			
			// load a font
			MFontRef * fontRef = guiData->loadFont("data/Gentium102/GenR102.TTF");
			
			// add some editable text
			MGuiEditText * text = gw->addNewEditText();
			text->setPosition(MVector2(100, 100));
			text->setFont(fontRef);
			//text->setTextAlign(M_ALIGN_RIGHT);
			text->setTextSize(16);
			text->setText("Hello world !");
			text->setTextColor(MVector4(1, 1, 1, 1));
		}
		break;
		
	case MWIN_EVENT_CLOSE:
		{
			MGUI_closeWindow(rootWindow);
		}
		break;
		
	case MWIN_EVENT_DESTROY:
		{
			SAFE_DELETE(guiData);
			SAFE_DELETE(renderer);
			SAFE_DELETE(render);
		}
		break;
		
	default:
		break;
	}
}


void drawCallback(MWindow * rootWindow)
{
	// update data if needed
	guiData->updateQueueDatas();
}



int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

	char dir[256];
	getDirectory(dir, argv[0]);
	MGUI_setCurrentDirectory(dir);
	

	// get engine
	MEngine * engine = MEngine::getInstance();

	// init
	if(! MGUI_init())
		return EXIT_FAILURE;
		
		
	// add loaders
	M_initFreeImage();
	engine->getImageLoader()->addLoader(M_loadImage);
	engine->getFontLoader()->addLoader(M_loadFont);


	// create main window
	MWindow * window = MGUI_createWindow("test1", 10, 10, 800, 600, winEvents);
	if(! window)
	{
		MGUI_close();
		M_closeFreeImage();
		return EXIT_FAILURE;
	}

	window->setDrawCallback(drawCallback);
	
	
	// create secondary window
	//MWindow * window2 = MGUI_createWindow("test2", 200, 10, 400, 400, NULL);
	
	
	// update
	while(1)
	{
		if(! MGUI_update())
			break;
	}
	
	
	MGUI_close();
	M_closeFreeImage();
	return EXIT_SUCCESS;
}
