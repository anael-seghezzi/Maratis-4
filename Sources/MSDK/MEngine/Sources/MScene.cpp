/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MScene.cpp
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


#include "../Includes/MEngine.h"


MScene::MScene():
m_dataMode(M_DATA_STATIC),
m_currentFrame(0),
m_currentCamera(0),
m_gravity(0, 0, -0.981f)
{}

MScene::~MScene()
{
	stopAllSounds();

	// delete objects
	unsigned int oSize = m_objects.size();
	for(unsigned int i=0; i<oSize; i++)
		SAFE_DELETE(m_objects[i]);

	m_objects.clear();

	m_entities.clear();
	m_cameras.clear();
	m_lights.clear();
	m_sounds.clear();
}

MOCamera * MScene::addNewCamera(void)
{
	MOCamera * newCamera = new MOCamera();
	m_cameras.push_back(newCamera);
	m_objects.push_back(newCamera);
	return newCamera;
}

MOCamera * MScene::addNewCamera(const MOCamera & camera)
{
	MOCamera * newCamera = new MOCamera(camera);
	m_cameras.push_back(newCamera);
	m_objects.push_back(newCamera);
	return newCamera;
}

MOLight * MScene::addNewLight(void)
{
	MOLight * newLight = new MOLight();
	m_lights.push_back(newLight);
	m_objects.push_back(newLight);
	return newLight;
}

MOLight * MScene::addNewLight(const MOLight & light)
{
	MOLight * newLight = new MOLight(light);
	m_lights.push_back(newLight);
	m_objects.push_back(newLight);
	return newLight;
}

MOEntity * MScene::addNewEntity(MMeshRef * meshRef)
{
	MOEntity * newEntity = new MOEntity(meshRef);
	m_entities.push_back(newEntity);
	m_objects.push_back(newEntity);
	return newEntity;
}

MOEntity * MScene::addNewEntity(const MOEntity & entity)
{
	MOEntity * newEntity = new MOEntity(entity);
	m_entities.push_back(newEntity);
	m_objects.push_back(newEntity);
	return newEntity;
}

MOSound * MScene::addNewSound(MSoundRef * soundRef)
{
	MOSound * newSoundSource = new MOSound(soundRef);
	m_sounds.push_back(newSoundSource);
	m_objects.push_back(newSoundSource);
	return newSoundSource;
}

MOSound * MScene::addNewSound(const MOSound & sound)
{
	MOSound * newSoundSource = new MOSound(sound);
	m_sounds.push_back(newSoundSource);
	m_objects.push_back(newSoundSource);
	return newSoundSource;
}

MOText * MScene::addNewText(MFontRef * fontRef)
{
	MOText * newText = new MOText(fontRef);
	m_texts.push_back(newText);
	m_objects.push_back(newText);
	return newText;
}

MOText * MScene::addNewText(const MOText & text)
{
	MOText * newText = new MOText(text);
	m_texts.push_back(newText);
	m_objects.push_back(newText);
	return newText;
}

MOCamera * MScene::getCurrentCamera(void)
{
	unsigned int cSize = getCamerasNumber();
	if(cSize > 0)
	{
		if(m_currentCamera < cSize)
			return getCameraByIndex(m_currentCamera);
		else
			return getCameraByIndex(0);
	}
	
	return NULL;
}

MOEntity * MScene::getRayNearestEntityIntersection(const MVector3 & origin, const MVector3 & direction, float * distance)
{
	float nearestDist = 10E7;
	MOEntity * nearestEntity = NULL;
	
	unsigned int i;
	unsigned int oSize = getEntitiesNumber();
	for(i=0; i<oSize; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		
		if(! entity->isActive() || entity->isInvisible())
			continue;

		MMesh * mesh = entity->getMesh();
		if(! mesh)
			continue;
		
		float dist = entity->getRayNearestIntersectionDistance(origin, direction);
		if(dist > 0)
		{
			if(dist < nearestDist)
			{
				nearestEntity = entity;
				nearestDist = dist;
			}
		}
	}
	
	if(distance && nearestEntity)
		*distance = nearestDist;
	
	return nearestEntity;
}

void MScene::setName(const char * name)
{
	m_name.set(name);
}

void MScene::setScriptFilename(const char * scriptFilename)
{
	m_scriptFilename.set(scriptFilename);
}

bool createShape(MOEntity * entity, MPhysicsProperties * phyProps, unsigned int * shapeId)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return false;

	// get bounding box
	MBox3d * box = entity->getBoundingBox();

	MVector3 scale = entity->getTransformedScale();

	// switch shapes
	switch(phyProps->getCollisionShape())
	{
	default:
	case M_COLLISION_SHAPE_BOX:
		physics->createBoxShape(shapeId, (box->max - box->min)*scale*0.5f);
		break;

	case M_COLLISION_SHAPE_SPHERE:
		{
			MVector3 vec = (box->max - box->min)*scale*0.5f;
			float radius = vec.x;
			radius = MAX(radius, vec.y);
			radius = MAX(radius, vec.z);
			physics->createSphereShape(shapeId, radius);
		}
		break;

	case M_COLLISION_SHAPE_CONE:
		{
			MVector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createConeShape(shapeId, radius, height);
		}
		break;

	case M_COLLISION_SHAPE_CAPSULE:
		{
			MVector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createCylinderShape(shapeId, radius, height);
		}
		break;

	case M_COLLISION_SHAPE_CYLINDER:
		{
			MVector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createCylinderShape(shapeId, radius, height);
		}
		break;

	case M_COLLISION_SHAPE_CONVEX_HULL:
		{
			MMesh * mesh = entity->getMesh();
			if(mesh)
			{
				MSubMesh * subMeshs = mesh->getSubMeshs();
				unsigned int subMeshsNumber = mesh->getSubMeshsNumber();
				if(subMeshsNumber == 0)
					return false;

				if(subMeshsNumber == 1)
				{
					MSubMesh * subMesh = &subMeshs[0];
					if(subMesh->getVerticesSize() > 0)
						physics->createConvexHullShape(shapeId, subMesh->getVertices(), subMesh->getVerticesSize(), entity->getScale());
				}
				else
				{
					physics->createMultiShape(shapeId);
					unsigned int s;
					for(s=0; s<subMeshsNumber; s++)
					{
						unsigned int subShapeId;
						MSubMesh * subMesh = &subMeshs[s];
						if(subMesh->getVerticesSize() > 0)
						{
							physics->createConvexHullShape(&subShapeId, subMesh->getVertices(), subMesh->getVerticesSize(), entity->getScale());
							physics->addChildShape(*shapeId, subShapeId, MVector3(), MQuaternion());
						}
					}
				}
			}
			else{
				return false;
			}
		}
		break;

	case M_COLLISION_SHAPE_TRIANGLE_MESH:
		{
			MMesh * mesh = entity->getMesh();
			if(mesh)
			{
				MSubMesh * subMeshs = mesh->getSubMeshs();
				unsigned int subMeshsNumber = mesh->getSubMeshsNumber();
				if(subMeshsNumber == 0)
					return false;

				if(subMeshsNumber == 1)
				{
					MSubMesh * subMesh = &subMeshs[0];
					if(subMesh->getVerticesSize() >= 3)
						physics->createTriangleMeshShape(shapeId,
							subMesh->getVertices(), subMesh->getVerticesSize(),
							subMesh->getIndices(), subMesh->getIndicesSize(), subMesh->getIndicesType(),
							entity->getScale()
						);
				}
				else
				{
					physics->createMultiShape(shapeId);
					unsigned int s;
					for(s=0; s<subMeshsNumber; s++)
					{
						unsigned int subShapeId;
						MSubMesh * subMesh = &subMeshs[s];
						if(subMesh->getVerticesSize() >= 3)
						{
							physics->createTriangleMeshShape(&subShapeId,
								subMesh->getVertices(), subMesh->getVerticesSize(),
								subMesh->getIndices(), subMesh->getIndicesSize(), subMesh->getIndicesType(),
								entity->getScale()
							);
							physics->addChildShape(*shapeId, subShapeId, MVector3(), MQuaternion());
						}
					}
				}
			}
			else{
				return false;
			}
		}
		break;
	}

	return true;
}

void MScene::prepareCollisionShape(MOEntity * entity)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;
		
	MPhysicsProperties * phyProps = entity->getPhysicsProperties();

	if(! phyProps)
		return;

	unsigned int shapeId = 0;
	if(createShape(entity, phyProps, &shapeId))
	{
		// has physics child
		bool hasPhysicsChild = false;
		unsigned int o;
		unsigned int oSize = entity->getChildsNumber();
		for(o=0; o<oSize; o++)
		{
			MObject3d * childObject = entity->getChild(o);
			if(childObject->getType() == M_OBJECT3D_ENTITY)
			{
				MOEntity * childEntity = (MOEntity*)childObject;
				MPhysicsProperties * childPhyProps = childEntity->getPhysicsProperties();
				if(childPhyProps)
				{
					if(! childPhyProps->isGhost())
					{
						hasPhysicsChild = true;
						break;
					}
				}
			}
		}

		// create multi shape
		if(hasPhysicsChild)
		{
			unsigned int subShapeId = shapeId;
			physics->createMultiShape(&shapeId);
			physics->addChildShape(shapeId, subShapeId, MVector3(), MQuaternion());
		}

		phyProps->setShapeId(shapeId);
	}
}

void MScene::prepareCollisionObject(MOEntity * entity)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;
		
	MPhysicsProperties * phyProps = entity->getPhysicsProperties();
	if(! phyProps)
		return;

	unsigned int shapeId = phyProps->getShapeId();
	if(shapeId == 0)
		return;

	// has physics parent
	MPhysicsProperties * parentPhyProps = NULL;
	MOEntity * parentEntity = NULL;
	if(! phyProps->isGhost())
	{
		MObject3d * parentObject = entity->getParent();
		if(parentObject)
		{
			if(parentObject->getType() == M_OBJECT3D_ENTITY)
			{
				parentEntity = (MOEntity*)parentObject;
				parentPhyProps = parentEntity->getPhysicsProperties();
			}
		}
	}

	if(parentPhyProps) // add shape to parent multi-shape
	{
		MVector3 position = entity->getPosition() * parentEntity->getTransformedScale();
		MQuaternion rotation = entity->getRotation();

		phyProps->setShapeId(shapeId);
		physics->addChildShape(parentPhyProps->getShapeId(), shapeId, position, rotation);
	}
	else // create collision object
	{
		unsigned int collisionObjectId;

		if(phyProps->isGhost())
		{
			MVector3 euler = entity->getTransformedRotation();

			physics->createGhost(
				&collisionObjectId, shapeId,
				entity->getTransformedPosition(),
				MQuaternion(euler.x, euler.y, euler.z)
			);

			phyProps->setShapeId(shapeId);
			phyProps->setCollisionObjectId(collisionObjectId);
		}
		else
		{
			physics->createRigidBody(
				&collisionObjectId, shapeId,
				entity->getPosition(), entity->getRotation(),
				phyProps->getMass()
			);

			phyProps->setShapeId(shapeId);
			phyProps->setCollisionObjectId(collisionObjectId);

			physics->setObjectRestitution(collisionObjectId, phyProps->getRestitution());
			physics->setObjectDamping(collisionObjectId, phyProps->getLinearDamping(), phyProps->getAngularDamping());
			physics->setObjectAngularFactor(collisionObjectId, phyProps->getAngularFactor());
			physics->setObjectLinearFactor(collisionObjectId, *phyProps->getLinearFactor());
			physics->setObjectFriction(collisionObjectId, phyProps->getFriction());
		}

		// deactivate
		if(! entity->isActive())
			physics->deactivateObject(collisionObjectId);

		// set user pointer (entity)
		physics->setObjectUserPointer(collisionObjectId, entity);
	}
}

void MScene::prepareConstraints(MOEntity * entity)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;
		
	MPhysicsProperties * phyProps = entity->getPhysicsProperties();

	if(! phyProps)
		return;

	unsigned int objectId = phyProps->getCollisionObjectId();
	MPhysicsConstraint * constraint = phyProps->getConstraint();

	if((! constraint) || (objectId == 0) || phyProps->isGhost())
		return;

	if(! entity->getParent())
	{
		MOEntity * constraintParent = getEntityByName(constraint->parentName.getSafeString());
		if(constraintParent)
		{
			MPhysicsProperties * parentPhyProps = constraintParent->getPhysicsProperties();
			if(parentPhyProps)
			{
				unsigned int parentObjectId = parentPhyProps->getCollisionObjectId();
				if(parentObjectId != 0)
				{
					physics->createConstraint(
											  &constraint->constraintId,
											  parentObjectId,
											  objectId,
											  constraint->pivot,
											  constraint->disableParentCollision
											  );

					constraint->parentObjectId = parentObjectId;
					physics->setLinearLimit(constraint->constraintId, constraint->lowerLinearLimit, constraint->upperLinearLimit);
					physics->setAngularLimit(constraint->constraintId, constraint->lowerAngularLimit, constraint->upperAngularLimit);
				}
			}
		}
	}
}

void MScene::preparePhysics(void)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;

	physics->init(MVector3(-10000), MVector3(10000));
	physics->setWorldGravity(m_gravity);

	// create shapes
	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		prepareCollisionShape(entity);
	}

	// create objects
	for(i=0; i<size; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		prepareCollisionObject(entity);
	}

	// create constraints
	for(i=0; i<size; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		prepareConstraints(entity);
	}
}

MString MScene::getNewObjectName(const char * baseName)
{
	unsigned int count = 0;
	
	char name[256];
	sprintf(name, "%s%d", baseName, count);
	
	int size = getObjectsNumber();
	for(int i=0; i<size; i++)
	{
		MObject3d * object = getObjectByIndex(i);
		if(object->getName())
		{
            if(strcmp(name, object->getName()) == 0) // name already exists
            {
                count++;
                sprintf(name, "%s%d", baseName, count);
                i = -1;
            }
		}
	}
	
	return MString(name);
}

void MScene::deleteObject(MObject3d * object)
{
	if(! object)
		return;

	unsigned int i;
	unsigned int oSize;

	// objects
	switch(object->getType())
	{
		case M_OBJECT3D_ENTITY:
		{
			// entities
			oSize = m_entities.size();
			for(i=0; i<oSize; i++){
				MObject3d * obj = m_entities[i];
				if(obj == object){
					m_entities.erase(m_entities.begin() + i);
					break;
				}
			}
		}
		break;

		case M_OBJECT3D_CAMERA:
		{
			// cameras
			oSize = m_cameras.size();
			for(i=0; i<oSize; i++){
				MObject3d * obj = m_cameras[i];
				if(obj == object){
					m_cameras.erase(m_cameras.begin() + i);
					break;
				}
			}
		}
		break;

		case M_OBJECT3D_LIGHT:
		{
			// lights
			oSize = m_lights.size();
			for(i=0; i<oSize; i++){
				MObject3d * obj = m_lights[i];
				if(obj == object){
					m_lights.erase(m_lights.begin() + i);
					break;
				}
			}
		}
		break;

		case M_OBJECT3D_SOUND:
		{
			// sounds
			oSize = m_sounds.size();
			for(i=0; i<oSize; i++){
				MObject3d * obj = m_sounds[i];
				if(obj == object){
					m_sounds.erase(m_sounds.begin() + i);
					break;
				}
			}
		}
		break;

		case M_OBJECT3D_TEXT:
		{
			// sounds
			oSize = m_texts.size();
			for(i=0; i<oSize; i++){
				MObject3d * obj = m_texts[i];
				if(obj == object){
					m_texts.erase(m_texts.begin() + i);
					break;
				}
			}
		}
		break;
	}

	// objects pointer
	oSize = m_objects.size();
	for(i=0; i<oSize; i++)
	{
		MObject3d * obj = m_objects[i];
		if(obj == object)
		{
			obj->unLink();
			obj->unlinkChilds();

			m_objects.erase(m_objects.begin() + i);
			SAFE_DELETE(obj);
			return;
		}
	}
}

bool MScene::getObjectIndex(const char * name, unsigned int * id)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		if(strcmp(name, m_objects[i]->getName()) == 0)
		{
			(*id) = i;
			return true;
		}
	}

	return false;
}

MObject3d * MScene::getObjectByName(const char * name)
{
	unsigned int id;
	if(getObjectIndex(name, &id)){
		return m_objects[id];
	}

	return NULL;
}

MOLight * MScene::getLightByName(const char * name)
{
	unsigned int i;
	unsigned int size = getLightsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_lights[i]->getName()) == 0)
			return m_lights[i];
	}

	return NULL;
}

MOCamera * MScene::getCameraByName(const char * name)
{
	unsigned int i;
	unsigned int size = getCamerasNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_cameras[i]->getName()) == 0)
			return m_cameras[i];
	}

	return NULL;
}

MOEntity * MScene::getEntityByName(const char * name)
{
	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_entities[i]->getName()) == 0)
			return m_entities[i];
	}

	return NULL;
}

MOSound * MScene::getSoundByName(const char * name)
{
	unsigned int i;
	unsigned int size = getSoundsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_sounds[i]->getName()) == 0)
			return m_sounds[i];
	}

	return NULL;
}

MOText * MScene::getTextByName(const char * name)
{
	unsigned int i;
	unsigned int size = getTextsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_texts[i]->getName()) == 0)
			return m_texts[i];
	}

	return NULL;
}

void MScene::begin(void)
{
	MEngine * engine = MEngine::getInstance();
	MScriptContext * scriptContext = engine->getScriptContext();

	updateObjectsMatrices();
	playLoopSounds();
	preparePhysics();

	// run script
	if(scriptContext)
		scriptContext->runScript(getScriptFilename());
}

void MScene::end(void)
{
	stopAllSounds();
}

void MScene::updatePhysics(void)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;

	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		if(! entity->isActive())
			continue;

		MPhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(! phyProps)
			continue;

		if(phyProps->getCollisionObjectId() > 0)
		{
			MObject3d * parent = entity->getParent();
			if(parent && phyProps->isGhost())
			{
				MVector3 euler = entity->getTransformedRotation();
				physics->setObjectTransform(
					phyProps->getCollisionObjectId(),
					entity->getTransformedPosition(),
					MQuaternion(euler.x, euler.y, euler.z)
				);
			}
			else if(entity->needToUpdate())
			{
				physics->setObjectTransform(
					phyProps->getCollisionObjectId(),
					entity->getPosition(),
					entity->getRotation()
				);
			}
		}
	}

	physics->setWorldGravity(m_gravity);
	physics->updateSimulation();

	for(i=0; i<size; i++)
	{
		MOEntity * entity = getEntityByIndex(i);
		if(! entity->isActive())
			continue;

		MPhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(phyProps)
		{
			if((phyProps->getCollisionObjectId() > 0) && (! phyProps->isGhost()))
			{
				MVector3 position = entity->getPosition();
				MQuaternion rotation = entity->getRotation();

				physics->getObjectTransform(phyProps->getCollisionObjectId(), &position, &rotation);

				entity->setPosition(position);
				entity->setRotation(rotation);
			}
		}
	}
}

void MScene::updateObjectsBehaviors(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		object->updateBehaviors();
	}
}

void MScene::drawObjectsBehaviors(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		object->drawBehaviors();
	}
}

void MScene::updateObjectsMatrices(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		if(! object->hasParent())
		{
			if(object->needToUpdate())
				object->computeLocalMatrix();
			object->computeChildsMatrices();
		}
	}
}

void MScene::update(void)
{
	// update objects
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getObjectByIndex(i);
		if(object->isActive())
			object->update();
	}

	// update current frame
	m_currentFrame++;
}

void MScene::playLoopSounds()
{
	unsigned int i;

	// sound sources
	unsigned int sSize = getSoundsNumber();
	for(i=0; i<sSize; i++)
	{
		if(m_sounds[i]->isActive() && m_sounds[i]->isLooping())
			m_sounds[i]->play();
	}
}

void MScene::stopAllSounds()
{
	unsigned int i;

	// sound sources
	unsigned int sSize = getSoundsNumber();
	for(i=0; i<sSize; i++)
		m_sounds[i]->stop();
}

void MScene::draw(MOCamera * camera)
{
	MEngine * engine = MEngine::getInstance();
	MRenderer * renderer = engine->getRenderer();
	if(renderer)
		renderer->drawScene(this, camera);
}
