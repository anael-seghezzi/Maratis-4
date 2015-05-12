/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MLevelLoad.cpp
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


#include <MEngine.h>
#include <tinyxml.h>

#include "MLevelLoad.h"

static char rep[256];
static MLevel * level;

struct LinkTo
{
	MObject3d * object;
	string parentName;
};

vector <LinkTo> g_links;


bool readBool(TiXmlElement * node, const char * name, bool * variable)
{
	const char * str = node->Attribute(name);
	if(str)
	{
		if(strcmp(str, "true") == 0)
			*variable = true;
		else
			*variable = false;
		return true;
	}
	return false;
}

bool readFloatValues(TiXmlElement * node, const char * name, float * vector, unsigned int size)
{
	const char * data = node->Attribute(name);
	if(data)
	{
		char * tok;
		const char filter[] = " \t";

		tok = strtok((char*)data, filter);

		unsigned int pass = 0;
		while((tok != NULL) && (pass < size))
		{
			sscanf(tok, "%f", vector);
			tok = strtok(NULL, filter);
			vector++;
			pass++;
		}

		return true;
	}

	return false;
}

void readPhysics(TiXmlElement * node, MPhysicsProperties * physicsProperties)
{
	// shape
	const char * str = node->Attribute("shape");
	if(str)
	{
		if(strcmp(str, "Box") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_BOX);
		if(strcmp(str, "Sphere") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_SPHERE);
		if(strcmp(str, "Cone") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_CONE);
		if(strcmp(str, "Capsule") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_CAPSULE);
		if(strcmp(str, "Cylinder") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_CYLINDER);
		if(strcmp(str, "ConvexHull") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_CONVEX_HULL);
		if(strcmp(str, "TriangleMesh") == 0)
			physicsProperties->setCollisionShape(M_COLLISION_SHAPE_TRIANGLE_MESH);
	}

	// ghost
	bool ghost;
	if(readBool(node, "ghost", &ghost))
		physicsProperties->setGhost(ghost);

	// mass
	float mass;
	if(node->QueryFloatAttribute("mass", &mass) == TIXML_SUCCESS)
		physicsProperties->setMass(mass);

	// friction
	float friction;
	if(node->QueryFloatAttribute("friction", &friction) == TIXML_SUCCESS)
		physicsProperties->setFriction(friction);

	// restitution
	float restitution;
	if(node->QueryFloatAttribute("restitution", &restitution) == TIXML_SUCCESS)
		physicsProperties->setRestitution(restitution);

	// linearDamping
	float linearDamping;
	if(node->QueryFloatAttribute("linearDamping", &linearDamping) == TIXML_SUCCESS)
		physicsProperties->setLinearDamping(linearDamping);

	// angularDamping
	float angularDamping;
	if(node->QueryFloatAttribute("angularDamping", &angularDamping) == TIXML_SUCCESS)
		physicsProperties->setAngularDamping(angularDamping);

	// angularFactor
	float angularFactor;
	if(node->QueryFloatAttribute("angularFactor", &angularFactor) == TIXML_SUCCESS)
		physicsProperties->setAngularFactor(angularFactor);

	// linearFactor
	MVector3 linearFactor;
	if(readFloatValues(node, "linearFactor", linearFactor, 3))
		physicsProperties->setLinearFactor(linearFactor);
}

void readConstraint(TiXmlElement * node, MPhysicsConstraint * constraint)
{
	// parent
	const char * str = node->Attribute("parent");
	if(str)
		constraint->parentName = str;
		
	readFloatValues(node, "pivot", constraint->pivot, 3);
	readFloatValues(node, "lowerLinearLimit", constraint->lowerLinearLimit, 3);
	readFloatValues(node, "upperLinearLimit", constraint->upperLinearLimit, 3);
	readFloatValues(node, "lowerAngularLimit", constraint->lowerAngularLimit, 3);
	readFloatValues(node, "upperAngularLimit", constraint->upperAngularLimit, 3);

	readBool(node, "disableParentCollision", &constraint->disableParentCollision);
}

void readSceneProperties(TiXmlElement * node, MScene * scene)
{
	// data mode
	const char * data = node->Attribute("data");
	if(data)
	{
		if(strcmp(data, "Static") == 0)
			scene->setDataMode(M_DATA_STATIC);
		else if(strcmp(data, "Dynamic") == 0)
			scene->setDataMode(M_DATA_DYNAMIC);
		else if(strcmp(data, "Stream") == 0)
			scene->setDataMode(M_DATA_STREAM);
	}

	// gravity
	MVector3 gravity;
	if(readFloatValues(node, "gravity", gravity, 3))
		scene->setGravity(gravity);
}

void readEntityProperties(TiXmlElement * node, MOEntity * entity)
{
	// invisible
	bool invisible;
	if(readBool(node, "invisible", &invisible))
		entity->setInvisible(invisible);
}

void readSoundProperties(TiXmlElement * node, MOSound * sound)
{
	// loop
	bool loop;
	if(readBool(node, "loop", &loop))
		sound->setLooping(loop);

	// pitch
	float pitch;
	if(node->QueryFloatAttribute("pitch", &pitch) == TIXML_SUCCESS)
		sound->setPitch(pitch);

	// gain
	float gain;
	if(node->QueryFloatAttribute("gain", &gain) == TIXML_SUCCESS)
		sound->setGain(gain);

	// radius
	float radius;
	if(node->QueryFloatAttribute("radius", &radius) == TIXML_SUCCESS)
		sound->setRadius(radius);

	// rolloff
	float rolloff;
	if(node->QueryFloatAttribute("rolloff", &rolloff) == TIXML_SUCCESS)
		sound->setRolloff(rolloff);

	// relative
	bool relative;
	if(readBool(node, "relative", &relative))
		sound->setRelative(relative);
}

void readTextProperties(TiXmlElement * node, MOText * text)
{
	// size 
	float size;
	if(node->QueryFloatAttribute("size", &size) == TIXML_SUCCESS)
		text->setSize(size);

	// align
	const char * align = node->Attribute("align");
	if(align)
	{
		if(strcmp(align, "Left") == 0)
			text->setAlign(M_ALIGN_LEFT);
		else if(strcmp(align, "Right") == 0)
			text->setAlign(M_ALIGN_RIGHT);
		else if(strcmp(align, "Center") == 0)
			text->setAlign(M_ALIGN_CENTER);
	}

	// color 
	MVector4 color(1, 1, 1, 1);
	if(readFloatValues(node, "color", color, 4))
		text->setColor(color);
}

void readTextData(TiXmlElement * node, MOText * text)
{
	const char * textData = node->GetText();
	if(textData)
		text->setText(textData);
}

void readCameraProperties(TiXmlElement * node, MOCamera * camera)
{
	// clear color
	MVector3 clearColor;
	if(readFloatValues(node, "clearColor", clearColor, 3))
		camera->setClearColor(clearColor);

	// ortho
	bool ortho;
	if(readBool(node, "ortho", &ortho))
		camera->enableOrtho(ortho);

	// fov
	float fov;
	if(node->QueryFloatAttribute("fov", &fov) == TIXML_SUCCESS)
		camera->setFov(fov);

	// clippingNear
	float clippingNear;
	if(node->QueryFloatAttribute("clippingNear", &clippingNear) == TIXML_SUCCESS)
		camera->setClippingNear(clippingNear);

	// clippingFar
	float clippingFar;
	if(node->QueryFloatAttribute("clippingFar", &clippingFar) == TIXML_SUCCESS)
		camera->setClippingFar(clippingFar);

	// fog
	bool fog;
	if(readBool(node, "fog", &fog))
		camera->enableFog(fog);

	// fogDistance
	float fogDistance;
	if(node->QueryFloatAttribute("fogDistance", &fogDistance) == TIXML_SUCCESS)
		camera->setFogDistance(fogDistance);
}

void readLightProperties(TiXmlElement * node, MOLight * light)
{
	// radius
	float radius;
	if(node->QueryFloatAttribute("radius", &radius) == TIXML_SUCCESS)
		light->setRadius(radius);

	// color 
	MVector3 color;
	if(readFloatValues(node, "color", color, 3))
		light->setColor(color);

	// intensity
	float intensity;
	if(node->QueryFloatAttribute("intensity", &intensity) == TIXML_SUCCESS)
		light->setIntensity(intensity);

	// spotAngle
	float spotAngle;
	if(node->QueryFloatAttribute("spotAngle", &spotAngle) == TIXML_SUCCESS)
		light->setSpotAngle(spotAngle);

	// spotExponent
	float spotExponent;
	if(node->QueryFloatAttribute("spotExponent", &spotExponent) == TIXML_SUCCESS)
		light->setSpotExponent(spotExponent);
	
	// shadow
	bool shadow;
	if(readBool(node, "shadow", &shadow))
		light->castShadow(shadow);
	
	if(shadow)
	{
		// shadow bias
		float shadowBias;
		if(node->QueryFloatAttribute("shadowBias", &shadowBias) == TIXML_SUCCESS)
			light->setShadowBias(shadowBias);
	
		// shadow blur
		float shadowBlur;
		if(node->QueryFloatAttribute("shadowBlur", &shadowBlur) == TIXML_SUCCESS)
			light->setShadowBlur(shadowBlur);
		
		// shadow quality
		unsigned int shadowQuality;
		if(node->QueryUIntAttribute("shadowQuality", &shadowQuality) == TIXML_SUCCESS)
			light->setShadowQuality(shadowQuality);
	}
}

void readObjectActive(TiXmlElement * node, MObject3d * object)
{
	TiXmlElement * activeNode = node->FirstChildElement("active");
	if(activeNode)
	{
		bool active;
		if(readBool(activeNode, "value", &active))
			object->setActive(active);
	}
}

void readObjectTransform(TiXmlElement * node, MObject3d * object)
{
	// parent
	const char * parentName = node->Attribute("parent");
	if(parentName)
	{
		LinkTo link;
		link.object = object;
		link.parentName = parentName;
		g_links.push_back(link);
	}

	// position
	MVector3 position;
	if(readFloatValues(node, "position", position, 3))
		object->setPosition(position);

	// rotation
	MVector3 rotation;
	if(readFloatValues(node, "rotation", rotation, 3))
		object->setEulerRotation(rotation);

	// scale
	MVector3 scale;
	if(readFloatValues(node, "scale", scale, 3))
		object->setScale(scale);
}

void readBehaviorProperties(TiXmlElement * node, MBehavior * behavior)
{
	unsigned int i;
	unsigned int size = behavior->getVariablesNumber();
	for(i=0; i<size; i++)
	{
		MVariable variable = behavior->getVariable(i);

		M_VARIABLE_TYPE varType = variable.getType();
		if(varType == M_VARIABLE_NULL)
			continue;

		const char * name = variable.getName();

		switch(variable.getType())
		{
		case M_VARIABLE_BOOL:
			readBool(node, name, (bool*)variable.getPointer());
			break;
		case M_VARIABLE_INT:
		case M_VARIABLE_UINT:
			node->QueryIntAttribute(name, (int*)variable.getPointer());
			break;
		case M_VARIABLE_FLOAT:
			node->QueryFloatAttribute(name, (float*)variable.getPointer());
			break;
		case M_VARIABLE_STRING:
			{
				const char * str = node->Attribute(name);
				if(str)
					((MString*)variable.getPointer())->set(str);
			}
			break;
		case M_VARIABLE_VEC2:
			readFloatValues(node, name, *((MVector2*)variable.getPointer()), 2);
			break;
		case M_VARIABLE_VEC3:
			readFloatValues(node, name, *((MVector3*)variable.getPointer()), 3);
			break;
		case M_VARIABLE_VEC4:
			readFloatValues(node, name, *((MVector4*)variable.getPointer()), 4);
			break;
		case M_VARIABLE_TEXTURE_REF:
			{
				MTextureRef** textureRef = (MTextureRef**)variable.getPointer();
				
				const char * str = node->Attribute(name);
				if(str)
				{
					char filename[256];
					getGlobalFilename(filename, rep, str);
					*textureRef = level->loadTexture(filename);
				}
				
				break;
			}
		}
	}
}

void readBehaviors(TiXmlElement * node, MObject3d * object)
{
	if(! node)
		return;

	if(! object)
		return;

	MBehaviorManager * bManager = MEngine::getInstance()->getBehaviorManager();

	// check first behavior
	TiXmlElement * behaviorNode = node->FirstChildElement("Behavior");
	if(! behaviorNode)
		return;

	// behaviors
	for(behaviorNode; behaviorNode; behaviorNode=behaviorNode->NextSiblingElement("Behavior"))
	{
		// name
		const char * behaviorName = behaviorNode->Attribute("name");

		MBehaviorCreator * bCreator = bManager->getBehaviorByName(behaviorName);
		if(! bCreator)
		{
			printf("Warning : unable to load behavior \"%s\"\n", behaviorName);
			continue;
		}

		MBehavior * behavior = bCreator->getNewBehavior(object);
		object->addBehavior(behavior);

		// properties
		TiXmlElement * propertiesNode = behaviorNode->FirstChildElement("properties");
		if(propertiesNode)
			readBehaviorProperties(propertiesNode, behavior);
	}
}

bool xmlLevelLoad(const char * filename, void * data){
	return M_loadLevel(filename, data);
}

bool M_loadLevel(const char * filename, void * data, const bool clearData)
{
	char scriptFilename[256];
	char soundFilename[256];
	char meshFilename[256];
	char textFilename[256];

	// get rep
	getRepertory(rep, filename);

	// read document
	TiXmlDocument doc(filename);
	//if(! doc.LoadFile())
	//	return false;
	
	// load xml file
	{
		char * buf = readTextFile(filename);
		if(! buf)
			return false;
			
		if(! doc.LoadBuffer(buf, strlen(buf)))
		{
			SAFE_FREE(buf);
			return false;
		}
		
		SAFE_FREE(buf);
	}
	
	// xml handle
	TiXmlHandle hDoc(&doc);
	TiXmlElement * rootNode;
	TiXmlHandle hRoot(0);

	// maratis
	rootNode = hDoc.FirstChildElement().Element();
	if(! rootNode)
		return false;

	if(strcmp(rootNode->Value(), "Maratis") != 0)
		return false;

	hRoot = TiXmlHandle(rootNode);

	// level
	TiXmlElement * levelNode = rootNode->FirstChildElement("Level");
	if(! levelNode)
		return false;

	// check first scene
	TiXmlElement * sceneNode = levelNode->FirstChildElement("Scene");
	if(! sceneNode)
		return false;

	// get current level
	MEngine * engine = MEngine::getInstance();
	MLevel * oldLevel = engine->getLevel();

	// create new level
	level = (MLevel *)data;
	if(clearData)
		level->clear();
	else
		level->clearScenes();

	// set current level
	engine->setLevel(level);
	
	// properties
	unsigned int sceneId = 0;
	TiXmlElement * levelPropertiesNode = levelNode->FirstChildElement("properties");
	if(levelPropertiesNode){
		// currentScene
		levelPropertiesNode->QueryUIntAttribute("currentScene", &sceneId);
	}

	// scene
	for(sceneNode; sceneNode; sceneNode=sceneNode->NextSiblingElement())
	{
		MScene * scene = level->addNewScene();

		// clear links
		g_links.clear();

		// name
		const char * sceneName = sceneNode->Attribute("name");
		scene->setName(sceneName);

		// script
		TiXmlElement * scriptNode = sceneNode->FirstChildElement("script");
		if(scriptNode)
		{
			const char * file = scriptNode->Attribute("file");
			if(file)
			{
				if(strlen(file) > 0)
				{
					getGlobalFilename(scriptFilename, rep, file);
					scene->setScriptFilename(scriptFilename);
				}
			}
		}

		// properties
		TiXmlElement * propertiesNode = sceneNode->FirstChildElement("properties");
		if(propertiesNode)
			readSceneProperties(propertiesNode, scene);

		// preload
		bool preload = false;
		M_DATA_MODES dataMode = scene->getDataMode();

		if(dataMode == M_DATA_STATIC) // preload if scene static
			preload = true;
		else if(dataMode == M_DATA_DYNAMIC || dataMode == M_DATA_STREAM) // preload if scene is current scene dynamic
			preload = (level->getScenesNumber() == (sceneId+1));

		// light
		TiXmlElement * lightNode = sceneNode->FirstChildElement("Light");
		for(lightNode; lightNode; lightNode=lightNode->NextSiblingElement("Light"))
		{
			MOLight * light = scene->addNewLight();

			// name
			const char * lightName = lightNode->Attribute("name");
			light->setName(lightName);

			// active
			readObjectActive(lightNode, light);

			// transform
			TiXmlElement * transformNode = lightNode->FirstChildElement("transform");
			if(transformNode)
				readObjectTransform(transformNode, light);

			// properties
			propertiesNode = lightNode->FirstChildElement("properties");
			if(propertiesNode)
				readLightProperties(propertiesNode, light);

			// behaviors
			readBehaviors(lightNode, light);
		}

		// camera
		TiXmlElement * cameraNode = sceneNode->FirstChildElement("Camera");
		for(cameraNode; cameraNode; cameraNode=cameraNode->NextSiblingElement("Camera"))
		{
			MOCamera * camera = scene->addNewCamera();

			// name
			const char * cameraName = cameraNode->Attribute("name");
			camera->setName(cameraName);

			// active
			readObjectActive(cameraNode, camera);

			// transform
			TiXmlElement * transformNode = cameraNode->FirstChildElement("transform");
			if(transformNode)
				readObjectTransform(transformNode, camera);

			// properties
			propertiesNode = cameraNode->FirstChildElement("properties");
			if(propertiesNode)
				readCameraProperties(propertiesNode, camera);

			// behaviors
			readBehaviors(cameraNode, camera);
		}

		// sound
		TiXmlElement * soundNode = sceneNode->FirstChildElement("Sound");
		for(soundNode; soundNode; soundNode=soundNode->NextSiblingElement("Sound"))
		{
			// name
			const char * soundName = soundNode->Attribute("name");
			
			// file
			const char * file = soundNode->Attribute("file");
			
			MSoundRef * soundRef = NULL;
			if(file)
			{
				getGlobalFilename(soundFilename, rep, file);
				soundRef = level->loadSound(soundFilename, preload);
				if(! soundRef)
					printf("ERROR loading sound : sound %s doesn't exist\n", file);
			}

			// create sound
			MOSound * sound = scene->addNewSound(soundRef);
			sound->setName(soundName);

			// active
			readObjectActive(soundNode, sound);

			// transform
			TiXmlElement * transformNode = soundNode->FirstChildElement("transform");
			if(transformNode)
				readObjectTransform(transformNode, sound);

			// properties
			propertiesNode = soundNode->FirstChildElement("properties");
			if(propertiesNode)
				readSoundProperties(propertiesNode, sound);

			// behaviors
			readBehaviors(soundNode, sound);
		}

		// entity
		TiXmlElement * entityNode = sceneNode->FirstChildElement("Entity");
		for(entityNode; entityNode; entityNode=entityNode->NextSiblingElement("Entity"))
		{
			// name
			const char * entityName = entityNode->Attribute("name");
			
			// file
			const char * file = entityNode->Attribute("file");
			
			MMeshRef * meshRef = NULL;
			if(file)
			{
				getGlobalFilename(meshFilename, rep, file);
				meshRef = level->loadMesh(meshFilename, preload);
			}

			// create entity
			MOEntity * entity = scene->addNewEntity(meshRef);
			entity->setName(entityName);

			// active
			readObjectActive(entityNode, entity);

			// bounding box
			TiXmlElement * boxNode = entityNode->FirstChildElement("BoundingBox");
			if(boxNode)
			{
				readFloatValues(boxNode, "min", entity->getBoundingBox()->min, 3);
				readFloatValues(boxNode, "max", entity->getBoundingBox()->max, 3);
			}

			// anim
			TiXmlElement * animNode = entityNode->FirstChildElement("anim");
			if(animNode)
			{
				unsigned int animId;
				if(animNode->QueryUIntAttribute("id", &animId) == TIXML_SUCCESS)
					entity->changeAnimation(animId);
			}

			// transform
			TiXmlElement * transformNode = entityNode->FirstChildElement("transform");
			if(transformNode)
				readObjectTransform(transformNode, entity);

			// properties
			propertiesNode = entityNode->FirstChildElement("properties");
			if(propertiesNode)
				readEntityProperties(propertiesNode, entity);

			// physics
			TiXmlElement * physicsNode = entityNode->FirstChildElement("physics");
			if(physicsNode)
			{
				MPhysicsProperties * physicsProperties = entity->createPhysicsProperties();
				readPhysics(physicsNode, physicsProperties);
				
				// constraint
				TiXmlElement * constraintNode = entityNode->FirstChildElement("constraint");
				if(constraintNode)
				{
					MPhysicsConstraint * constraint = physicsProperties->createConstraint();
					readConstraint(constraintNode, constraint);
				}
			}

			// behaviors
			readBehaviors(entityNode, entity);
		}

		// text
		TiXmlElement * textNode = sceneNode->FirstChildElement("Text");
		for(textNode; textNode; textNode=textNode->NextSiblingElement("Text"))
		{
			// name
			const char * textName = textNode->Attribute("name");
			
			// file
			const char * file = textNode->Attribute("file");
			
			MFontRef * fontRef = NULL;
			if(file)
			{
				getGlobalFilename(textFilename, rep, file);
				fontRef = level->loadFont(textFilename);
				if(! fontRef)
					printf("ERROR loading text : font %s doesn't exist\n", file);
			}

			// create text
			MOText * text = scene->addNewText(fontRef);
			text->setName(textName);

			// active
			readObjectActive(textNode, text);

			// transform
			TiXmlElement * transformNode = textNode->FirstChildElement("transform");
			if(transformNode)
				readObjectTransform(transformNode, text);

			// properties
			propertiesNode = textNode->FirstChildElement("properties");
			if(propertiesNode)
				readTextProperties(propertiesNode, text);

			// text
			TiXmlElement * textDataNode = textNode->FirstChildElement("textData");
			if(textDataNode)
				readTextData(textDataNode, text);

			// behaviors
			readBehaviors(textNode, text);
		}

		// links
		unsigned int l;
		unsigned int lSize = g_links.size();
		for(l=0; l<lSize; l++)
		{
			LinkTo * link = &g_links[l];
			MObject3d * parent = scene->getObjectByName(link->parentName.c_str());
			if(parent)
				link->object->linkTo(parent);
		}
	}

	// add default scene
	if(level->getScenesNumber() == 0)
		level->addNewScene();

	// set current scene
	level->setCurrentSceneId(sceneId);

	// restore current level
	engine->setLevel(oldLevel);

	return true;
}