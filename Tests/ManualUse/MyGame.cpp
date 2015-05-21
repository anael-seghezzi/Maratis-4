/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManualUse example
// MyGame.cpp
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

// This game example is showing how to manually create a scene and load objects
// it can also be used in Maratis as game plugin (like WaterGameDemo)


#include "MyGame.h"
#include <MLog.h>

MyGame::MyGame(void):
MGame()
{
    MLOG(6, "new MyGame...");
}

MyGame::~MyGame(void)
{
}

void MyGame::onBegin(void)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
	{
	    MLOG(4, "Cannot get level from engine");
	    return;
	}

	// create a scene manually
	if(level->getScenesNumber() == 0)
	{
		// add scene
		MScene * scene = level->addNewScene();
		
		// camera
		MOCamera * camera = scene->addNewCamera();
		
		camera->setClearColor(MVector3(0.5f, 0.5f, 0.5f)); // set grey clear color
		camera->setPosition(MVector3(0.0f, -80.0f, 20.0f));
		camera->setEulerRotation(MVector3(90.0f, 0.0f, 0.0f));

		// add robot entity
		MMeshRef * meshRef = level->loadMesh("data/robot.mesh");
		MOEntity * robot =scene->addNewEntity(meshRef);

		// add cubes with physics
		{
			// create entities
			meshRef = level->loadMesh("data/box.mesh");

			MOEntity * box1 = scene->addNewEntity(meshRef);
			MOEntity * box2 = scene->addNewEntity(meshRef);
			MOEntity * box3 = scene->addNewEntity(meshRef);

			// set coords
			box1->setPosition(MVector3(70, 65, 0));
			box1->setScale(MVector3(4, 4, 0.2f));

			box2->setPosition(MVector3(70, 65, 40));
			box2->setEulerRotation(MVector3(0, -35, 0));

			box3->setPosition(MVector3(75, 65, 70));

			// enable physics, MPhysicsProperties create a static box shape by default
			MPhysicsProperties * phyProps = box1->createPhysicsProperties();

			phyProps = box2->createPhysicsProperties();
			phyProps->setMass(1);

			phyProps = box3->createPhysicsProperties();
			phyProps->setMass(1);
		}

		// add text
		MFontRef * fontRef = level->loadFont("data/Gentium102/GenR102.TTF");
		MOText * text = scene->addNewText(fontRef);
		text->setPosition(MVector3(0, 0, 40));
		text->setEulerRotation(MVector3(-90, 0, 0));
		text->setText("Hello");
		text->setSize(8);
		text->setAlign(M_ALIGN_CENTER);

		// add light
		MOLight * light = scene->addNewLight();
		
		light->setPosition(MVector3(0.0f, 0.0f, 100.0f));
		light->setRadius(1000.0f);
	}
}

void MyGame::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MInputContext * input = engine->getInputContext();

	MGame::update(); // call standard update, or replace it with cutom code

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
	{
	    return;
	}

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
	{
	    return;
	}

	// rotate the entity
	MOEntity * entity = scene->getEntityByIndex(0);
	if (entity)
        entity->addAxisAngleRotation(MVector3(0.0f, 0.0f, 1.0f), 1.0f);

	// change light intensity with keyboard
	MOLight * light = scene->getLightByIndex(0);

	if(light && input->isKeyPressed("UP"))
	{
	    // inputs are also virtual, you can create your hown keys or axis
		light->setIntensity(light->getIntensity() + 0.1f);
	}

	if(light && input->isKeyPressed("DOWN"))
	{
		light->setIntensity(MAX(0.0f, light->getIntensity() - 0.1f));
	}
}

/*
// custom draw, if not defined, draw is MGame::draw()
void MyGame::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;

	render->enableDepthTest();

	MOCamera * camera = scene->getCameraByIndex(0);

	render->setClearColor(*camera->getClearColor());
	render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

	camera->enable();
	camera->updateListener();
	scene->draw(camera);
}
*/
