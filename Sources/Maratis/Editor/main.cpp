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
#include <Loaders/Mesh/MMeshLoad.h>

#include "MEditor.h"


// test data
static MRenderingContext * render = NULL;
static MRenderer * renderer = NULL;
static MSystemContext * systemContext = NULL;
static MLevel * level = NULL;

static MViewport * my3dView = NULL;



// syntax highlighting

static int sh_is_white(int c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

int sh_isalpha_special(int c)
{
	return isalpha(c) || c == '_';
}

int sh_isalnum_special(int c)
{
	return isalnum(c) || c == '_';
}

int sh_isquote(int c)
{
	return c == '\'' || c == '\"';
}

static int sh_compare_key(char *string, char *key, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if ((*string) != (*key))
			return 0;
		if (*string == 0 || *key == 0)
			return 0;
		string++;
		key++;
	}
	return 1;
}

static int sh_lua_compare_key(char *string, char *key, int len)
{
	char *next = string + len;
	return (
		sh_compare_key(string, key, len) &&
		(sh_is_white(*next) || !sh_isalnum_special(*next))
	);
}

static int sh_lua_instruction(char *string)
{
	if (sh_lua_compare_key(string, "and", 3)) return 3;
	if (sh_lua_compare_key(string, "break", 5)) return 5;
	if (sh_lua_compare_key(string, "do", 2)) return 2;
	if (sh_lua_compare_key(string, "else", 4)) return 4;
	if (sh_lua_compare_key(string, "elseif", 6)) return 6;
	if (sh_lua_compare_key(string, "end", 3)) return 3;
	if (sh_lua_compare_key(string, "false", 5)) return 5;
	if (sh_lua_compare_key(string, "for", 3)) return 3;
	if (sh_lua_compare_key(string, "function", 8)) return 8;
	if (sh_lua_compare_key(string, "if", 2)) return 2;
	if (sh_lua_compare_key(string, "in", 2)) return 2;
	if (sh_lua_compare_key(string, "local", 5)) return 5;
	if (sh_lua_compare_key(string, "nil", 3)) return 3;
	if (sh_lua_compare_key(string, "not", 3)) return 3;
	if (sh_lua_compare_key(string, "or", 2)) return 2;
	if (sh_lua_compare_key(string, "repeat", 6)) return 6;
	if (sh_lua_compare_key(string, "return", 6)) return 6;
	if (sh_lua_compare_key(string, "then", 4)) return 4;
	if (sh_lua_compare_key(string, "true", 4)) return 4;
	if (sh_lua_compare_key(string, "until", 5)) return 5;
	if (sh_lua_compare_key(string, "while", 5)) return 5;
	return 0;
}

static void sh_lua_syntax(const char *string, map<unsigned int, MColor> *coloring)
{
	MEditor *editor = MEditor::getInstance();
	MPreferences *pref = editor->getPreferences();
	int mode = -1; // 0, comment, instruction, number, string, alphanum
	int i = 0;
	char *s = (char *)string;

	coloring->clear();

	for (; *s; s++, i++) {

		int res;

		// comment
		if (mode == 1) {
			if (*s != '\n')
				continue;
		}
		else if (sh_compare_key(s, "--", 2)) {
			(*coloring)[i] = MColor(pref->getColor("lua comment"));
			mode = 1;
			s++;
			i++;
			continue;
		}

		// string
		if (mode == 4) {
			if (! sh_isquote(*s))
				continue;
			else {
				mode = -1;
				continue;
			}
		}
		else if (sh_isquote(*s)) {
			(*coloring)[i] = MColor(pref->getColor("lua string"));
			mode = 4;
			continue;
		}

		// instruction
		res = sh_lua_instruction(s);
		if (mode != 5 && res > 0) {
			(*coloring)[i] = MColor(pref->getColor("lua instruction"));
			s += res - 1;
			i += res - 1;
			mode = 2;
			continue;
		}

		// alphanum
		if (mode == 5) {
			if (sh_isalnum_special(*s))
				continue;
		}
		else if (sh_isalpha_special(*s)) {
			mode = 5;
			continue;
		}

		// number
		if (mode == 3) {
			if (isdigit(*s))
				continue;
		}
		else if (isdigit(*s)) {
			(*coloring)[i] = MColor(pref->getColor("lua number"));
			mode = 3;
			continue;
		}

		// default
		if (mode != 0) {
			(*coloring)[i] = MColor(pref->getColor("lua default"));
			mode = 0;
		}
	}
}




void scriptCallback(MGuiEditText * editText, MGUI_EVENT_TYPE event)
{ 
	if(event == MGUI_EVENT_ON_CHANGE)
		sh_lua_syntax(editText->getText(), editText->getTextColoring());
}

void winEvents2(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	
	switch(event)
	{
	case MWIN_EVENT_CREATE:
		{
			char filename[256];
			const char * workingDir = systemContext->getWorkingDirectory();
			MLevel * guiData = editor->getGuiData();

			// create a gui window (sub-window of rootWindow handled by MGui)
			MGuiWindow * gw = rootWindow->addNewWindow();
			gw->setScale(MVector2(rootWindow->getWidth(), rootWindow->getHeight()));
			gw->setColor(MVector3(0.3f, 0.3f, 0.3f));
			
			// load a font
			getGlobalFilename(filename, workingDir, "Resources/fonts/GenR102.TTF");
			MFontRef * fontRef = guiData->loadFont(filename);
			
			// add some editable text
			MGuiEditText * text = gw->addNewEditText();
			text->setPosition(MVector2(16, 16));
			text->setFont(fontRef);
			text->setTextSize(16);
			text->setTextColor(MVector4(1, 1, 1, 1));
			text->setEventCallback(scriptCallback);

			char *script = readTextFile("Resources/test/script.lua");
			text->setText(script);
			SAFE_FREE(script);
		}
		break;
		
	case MWIN_EVENT_CLOSE:
		{
			MGUI_closeWindow(rootWindow);
		}
		break;
		
	default:
		break;
	}
}

void winEvents(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	
	
	switch(event)
	{
	case MWIN_EVENT_CREATE:
		{
			// add loaders
			engine->getImageLoader()->addLoader(M_loadImage);
			engine->getFontLoader()->addLoader(M_loadFont);
			engine->getMeshLoader()->addLoader(xmlMeshLoad);
			engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
			engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
			engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
			
			// system
			systemContext = new MGUIContext();
			engine->setSystemContext(systemContext);
			
			// rendering context
			render = new MGLContext();
			engine->setRenderingContext(render);
			
			// renderer
			renderer = new MStandardRenderer();
			engine->setRenderer(renderer);
			
			// engine level
			level = new MLevel();
			engine->setLevel(level);
			
			// test scene
			{
				char filename[256];
				const char * workingDir = systemContext->getWorkingDirectory();
		
				MScene * scene = level->addNewScene();
				getGlobalFilename(filename, workingDir, "Resources/meshes/default/box.mesh");
				MOEntity * entity = scene->addNewEntity(level->loadMesh(filename));
				entity->setPosition(MVector3(0, 0, 10));
				entity->setScale(MVector3(10));
				
				MOCamera * camera = scene->addNewCamera();
				camera->setPosition(MVector3(200, 100, 200));
				camera->enableOrtho(true);
				
				MOSound * sound = scene->addNewSound(NULL);
				sound->setPosition(MVector3(100, 0, 0));
				
				getGlobalFilename(filename, workingDir, "Resources/meshes/default/box.mesh");
				MOEntity * entity2 = scene->addNewEntity(level->loadMesh(filename));
				entity2->setScale(MVector3(100, 100, 1));
				MMaterial * material = entity2->createLocalMaterial(0);
				material->setDiffuse(MVector3(0.8f));
				
				MOLight * light = scene->addNewLight();
				light->setPosition(MVector3(150, 0, 100));
				light->setColor(MVector3(1, 1, 1));
				light->setRadius(500);
				light->setLightType(M_LIGHT_DIRECTIONAL);

				light->addAxisAngleRotation(MVector3(1, 0, 0), 30);
				light->setShadowBlur(0.05f);
				light->setShadowQuality(1024);
				light->setShadowBias(0.1f);

				scene->update();
				scene->updateObjectsMatrices();
			}
			
			// editor init
			editor->init();
			
			my3dView = new MV3dEdit();
			my3dView->create(rootWindow);
			my3dView->resize(MVector2(0, 0), MVector2(rootWindow->getWidth(), rootWindow->getHeight()));
		}
		break;
		
	case MWIN_EVENT_CLOSE:
		{
			MGUI_closeWindow(rootWindow);
		}
		break;

	case MWIN_EVENT_RESIZE:
		if(my3dView)
			my3dView->resize(MVector2(0, 0), MVector2(rootWindow->getWidth(), rootWindow->getHeight()));
		break;

	case MWIN_EVENT_DESTROY:
		{
			SAFE_DELETE(my3dView);
			
			editor->clear();
			SAFE_DELETE(renderer);
			SAFE_DELETE(render);
			SAFE_DELETE(systemContext);
		}
		break;
		
	default:
		break;
	}
	
	// my3dView
	if(my3dView)
		my3dView->onEvent(rootWindow, event);
}


void drawCallback(MWindow * rootWindow)
{
	level->getCurrentScene()->update();
	level->getCurrentScene()->updateObjectsMatrices();
}



int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");
	
	char dir[256];
	getDirectory(dir, argv[0]);
	MGUI_setCurrentDirectory(dir);
		
	// init
	if(! MGUI_init())
		return EXIT_FAILURE;
	
	M_initFreeImage();
	
	// create main window
	MWindow * window = MGUI_createWindow("test1", 10, 200, 800, 600, winEvents);
	if(! window)
	{
		MGUI_close();
		M_closeFreeImage();
		return EXIT_FAILURE;
	}

	window->setDrawCallback(drawCallback);
	
	MGUI_createWindow("test2", 850, 200, 512, 512, winEvents2);
	
	printf("> Maratis");

	// update
	while(1)
	{
		if(! MGUI_update())
			break;
	}
	
	// close
	MGUI_close();
	M_closeFreeImage();
	return EXIT_SUCCESS;
}
