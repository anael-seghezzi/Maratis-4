/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManualUse example
// main.cpp
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

// This example is showing how to use Maratis-Engine manually
// code is basically the same as MaratisPlayer, without Plugin support

// MSDK
#include <MEngine.h>
#include <MGui.h>

// Common
#include <GUI/MGUI.h>

#include <Contexts/GL/MGLContext.h>
#include <Contexts/Input/MInput.h>
#include <Contexts/Bullet/MBulletContext.h>
#include <Loaders/FreeImage/MFreeImageLoader.h>
#include <Loaders/Freetype/MFreetypeLoader.h>
#include <Loaders/Mesh/MMeshLoad.h>
#include <Renderers/Fixed/MFixedRenderer.h>
#include <Renderers/Standard/MStandardRenderer.h>

// My Game
#include "MyGame.h"

// test data
static MLevel *level = NULL;
static MyGame *game = NULL;
static MRenderingContext *render = NULL;
static MInputContext *input = NULL;
static MBulletContext *physics = NULL;
static MRenderer *renderer = NULL;

// window events
void winEvents(MWindow *rootWindow, MWIN_EVENT_TYPE event)
{
    //MLOG(6, "Window Event : " << event);

    MEngine *engine = MEngine::getInstance();

    switch (event)
    {
    case MWIN_EVENT_CREATE:
    {
        // create a rendering context
        render = new MGLContext();
        engine->setRenderingContext(render);

        // create a input context
        input = new MInput();
        engine->setInputContext(input);

        // create a physics context
        physics = new MBulletContext();
        engine->setPhysicsContext(physics);

        // create a renderer
        renderer = new MStandardRenderer();
        engine->setRenderer(renderer);

        // create a level
        level = new MLevel();
        engine->setLevel(level);

        // create a game
        game = new MyGame(); // MyGame
        engine->setGame(game);

        game->begin();
    }
    break;

    case MWIN_EVENT_CLOSE:
    {
        MGUI_closeWindow(rootWindow);
    }
    break;

    case MWIN_EVENT_DESTROY:
    {
        game->end();

        SAFE_DELETE(level);
        SAFE_DELETE(game);
        SAFE_DELETE(renderer);
        SAFE_DELETE(render);
        SAFE_DELETE(input);
        SAFE_DELETE(physics);
    }
    break;

    default:
        break;
    }
}

void drawCallback(MWindow *rootWindow)
{
    // update data if needed
    level->updateQueueDatas();

    //game->update();
    game->draw();
}

// main
int main(int argc, char **argv)
{
    setlocale(LC_NUMERIC, "C");

    char dir[256];
    getDirectory(dir, argv[0]);
    MGUI_setCurrentDirectory(dir);

    // get engine
    MEngine *engine = MEngine::getInstance();

    // init
    if (!MGUI_init())
        return EXIT_FAILURE;

    // add loaders
    M_initFreeImage();

    engine->getImageLoader()->addLoader(M_loadImage); // image loader
    //engine->getSoundLoader()->addLoader(M_loadSound); // sound loader
    //engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader : uncomment if wanted
    engine->getFontLoader()->addLoader(M_loadFont); // font loader
    //engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader : uncomment if wanted

    // mesh loader
    engine->getMeshLoader()->addLoader(xmlMeshLoad);
    engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
    engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
    engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);

    // create main window
    MWindow *window = MGUI_createWindow("Manual Use", 10, 10, 800, 600, winEvents);
    if (!window)
    {
        MGUI_close();
        M_closeFreeImage();
        return EXIT_FAILURE;
    }

    window->setDrawCallback(drawCallback);

    const float frequency = 60.0;
    double currentTick, startTick = MGUI_getTime();
    unsigned int numFrame = 0;
    unsigned int currFrame = 0;
    unsigned int prevFrame = startTick * frequency;
    unsigned int steps, i;

    // update
    while (1)
    {
        ++numFrame;
        currentTick = MGUI_getTime();

        if (currentTick - startTick >= 1.0)
        {
            startTick += 1.0;

            MLOG(6, "fps: " << numFrame << ", ft: " << 1000.0f / numFrame << "ms");

            numFrame = 0;
        }

        currFrame = currentTick * frequency;
        steps = currFrame - prevFrame;
        prevFrame = currFrame;

        for (i = 0; i < steps; i++)
            game->update();

        if (!MGUI_update())
            break;
    }

    MGUI_close();
    M_closeFreeImage();
    return EXIT_SUCCESS;
}
