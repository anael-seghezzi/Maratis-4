/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MBulletContext.cpp
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


#include "MBulletContext.h"


// constructor
MBulletContext::MBulletContext(void):
	m_collisionConfiguration(NULL),
	m_dispatcher(NULL),
	m_overlappingPairCache(NULL),
	m_solver(NULL),
	m_dynamicsWorld(NULL),
	m_quality(1)
{
}

// destructor
MBulletContext::~MBulletContext(void)
{
	clear();
}

// init
void MBulletContext::init(const MVector3 & worldMin, const MVector3 & worldMax)
{
	clear();

	// create NULL id 0
	m_collisionShapes.push_back(NULL);
	m_collisionObjects.push_back(NULL);
	m_constraints.push_back(NULL);
	
	// init
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	m_overlappingPairCache = new btDbvtBroadphase();
	
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0,0,-10));
}

// destroy
void MBulletContext::clear(void)
{
	unsigned int i;
	unsigned int id;

	// delete user data
	for(i=0; i<m_userDatas.size(); i++){
		SAFE_DELETE(m_userDatas[i]);
	}
	
	// delete constraints
	for(i=0; i<m_constraints.size(); i++){
		id = i;
		deleteConstraint(&id);
	}

	// remove the rigidbodies from the dynamics world and delete them
	for(i=0; i<m_collisionObjects.size(); i++){
		id = i;
		deleteObject(&id);
	}

	// delete collision shapes
	for(i=0; i<m_collisionShapes.size(); i++){
		id = i;
		deleteShape(&id);
	}

	m_userDatas.clear();
	m_collisionObjects.clear();
	m_collisionShapes.clear();
	m_constraints.clear();
	
	SAFE_DELETE(m_dynamicsWorld);
	SAFE_DELETE(m_solver);
	SAFE_DELETE(m_overlappingPairCache);
	SAFE_DELETE(m_dispatcher);
	SAFE_DELETE(m_collisionConfiguration);
}

// update simulation
void MBulletContext::setSimulationQuality(unsigned int quality)
{
	m_quality = quality;
}

void MBulletContext::updateSimulation(void)
{
	unsigned int quality = MAX(1, m_quality);
	m_dynamicsWorld->stepSimulation(1.0f/6.0f, quality, 1.0f/(6.0f*quality));
}

// world
void MBulletContext::setWorldGravity(const MVector3 & gravity){
	m_dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

// create object
void MBulletContext::createGhost(unsigned int * objectId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation)
{
	createRigidBody(objectId, shapeId, position, rotation, 0.0000001f);
	m_collisionObjects[*objectId]->setCollisionFlags(m_collisionObjects[*objectId]->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);
}

void MBulletContext::createRigidBody(unsigned int * objectId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation, float mass)
{
	*objectId = m_collisionObjects.size();

	btCollisionShape * shape = m_collisionShapes[shapeId];

	btVector3 localInertia(0,0,0);
	if(mass > 0)
		shape->calculateLocalInertia(mass, localInertia);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));
	startTransform.setRotation(
		btQuaternion(rotation.values[0], rotation.values[1], rotation.values[2], rotation.values[3])
		);

	btDefaultMotionState * motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, localInertia);

	btRigidBody * rigidBody = new btRigidBody(info);
	rigidBody->setSleepingThresholds(0.2f, 0.2f); // default : 0.8f, 1.0f

	m_dynamicsWorld->addRigidBody(rigidBody);
	
	// user data
	MBUserData * userData = new MBUserData();
	userData->m_objectId = *objectId;
	m_userDatas.push_back(userData);

	
	// add collision object
	rigidBody->setUserPointer(userData);
	m_collisionObjects.push_back(rigidBody);
}

// activate / deactivate
void MBulletContext::activateObject(unsigned int objectId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object)
	{
		if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
		{
			btRigidBody * rigidBody = btRigidBody::upcast(object);
			m_dynamicsWorld->addRigidBody(rigidBody);
		}
			
		object->activate();
		clearForces(objectId);
	}
}

void MBulletContext::deactivateObject(unsigned int objectId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object)
	{
		m_dynamicsWorld->removeCollisionObject(object);
	}
}

// delete object
void MBulletContext::deleteObject(unsigned int * objectId)
{
	btCollisionObject * object = m_collisionObjects[*objectId];
	if(object)
	{
		if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
		{
			btRigidBody * body = btRigidBody::upcast(object);
			if(body && body->getMotionState()){
				btMotionState * motionState = body->getMotionState();
				SAFE_DELETE(motionState);
			}
		}

		m_dynamicsWorld->removeCollisionObject(object);
		SAFE_DELETE(m_collisionObjects[*objectId]);
		*objectId = 0;
	}
}

// object properties
void MBulletContext::enableObjectKinematic(unsigned int objectId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
}

void MBulletContext::disableObjectKinematic(unsigned int objectId){}

void MBulletContext::setObjectShape(unsigned int objectId, unsigned int shapeId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	btCollisionShape * shape = m_collisionShapes[shapeId];
	object->setCollisionShape(shape);
}

void MBulletContext::setObjectMass(unsigned int objectId, float mass)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->setMassProps(mass, btVector3(0, 0, 0));
	}
}

void MBulletContext::setObjectRestitution(unsigned int objectId, float restitution)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	object->setRestitution(btScalar(restitution));
}

void MBulletContext::setObjectFriction(unsigned int objectId, float friction)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object){
		object->setFriction(friction);
	}
}

void MBulletContext::setObjectLinearFactor(unsigned int objectId, const MVector3 & linearFactor)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
	}
}

void MBulletContext::setObjectAngularFactor(unsigned int objectId, float angularFactor)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->setAngularFactor(btScalar(angularFactor));
	}
}

void MBulletContext::setObjectDamping(unsigned int objectId, float linearDamping, float angularDamping)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->setDamping(btScalar(linearDamping), btScalar(angularDamping));
	}
}

void MBulletContext::setObjectTransform(unsigned int objectId, const MVector3 & position, const MQuaternion & rotation)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	transform.setRotation(btQuaternion(rotation.values[0], rotation.values[1], rotation.values[2], rotation.values[3]));
	
	btCollisionObject * object = m_collisionObjects[objectId];
	
	
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		if(body->getMotionState())
		{
			if(object->isStaticOrKinematicObject())
			{
				if(object->isKinematicObject())
				{
					m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
						object->getBroadphaseHandle(),
						m_dynamicsWorld->getDispatcher()
					);
					body->activate(true);
				}
			}
			else
			{
				body->activate(true);
			}
			
			btMotionState * motionState = body->getMotionState();
			motionState->setWorldTransform(transform);
			body->proceedToTransform(transform);
		}
	}
	else if(object->getInternalType() == btCollisionObject::CO_GHOST_OBJECT)
	{
		m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
			object->getBroadphaseHandle(),
			m_dynamicsWorld->getDispatcher()
		);

		btGhostObject * ghost = btGhostObject::upcast(object);
		
		ghost->setWorldTransform(transform);
		ghost->activate(true);
	}
}

void MBulletContext::getObjectTransform(unsigned int objectId, MVector3 * position, MQuaternion * rotation)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		if(body->getMotionState())
		{
			btTransform transform;
			btMotionState * motionState = body->getMotionState();
			motionState->getWorldTransform(transform);
			
			btVector3 tPos = transform.getOrigin();
			btQuaternion tRot = transform.getRotation();

			position->x = tPos.getX();
			position->y = tPos.getY();
			position->z = tPos.getZ();

			rotation->values[0] = tRot.getX();
			rotation->values[1] = tRot.getY();
			rotation->values[2] = tRot.getZ();
			rotation->values[3] = tRot.getW();
		}
	}
}

void MBulletContext::setObjectUserPointer(unsigned int objectId, void * userPointer)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object)
	{
		MBUserData * userData = (MBUserData *)object->getUserPointer();
		userData->m_userPointer = userPointer;
	}
}

void * MBulletContext::getObjectUserPointer(unsigned int objectId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object)
	{
		MBUserData * userData = (MBUserData *)object->getUserPointer();
		return userData->m_userPointer;
	}
	
	return NULL;
}


// affectors
void MBulletContext::addCentralForce(unsigned int objectId, const MVector3 & force)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->activate(true);
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void MBulletContext::getCentralForce(unsigned int objectId, MVector3 * force)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);

		btVector3 totalForce = body->getTotalForce();
		force->x = totalForce.getX();
		force->y = totalForce.getY();
		force->z = totalForce.getZ();
	}
}

void MBulletContext::addTorque(unsigned int objectId, const MVector3 & torque)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->activate(true);
		body->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
}

void MBulletContext::getTorque(unsigned int objectId, MVector3 * torque)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);

		btVector3 totalTorque = body->getTotalTorque();
		torque->x = totalTorque.getX();
		torque->y = totalTorque.getY();
		torque->z = totalTorque.getZ();
	}
}

void MBulletContext::clearForces(unsigned int objectId)
{
	btCollisionObject * object = m_collisionObjects[objectId];
	if(object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
	{
		btRigidBody * body = btRigidBody::upcast(object);
		body->clearForces();
		body->setLinearVelocity(btVector3(0, 0, 0));
		body->setAngularVelocity(btVector3(0, 0, 0));
	}
}

// objects collision
int MBulletContext::isObjectInCollision(unsigned int objectId, unsigned int * collisionList, unsigned int size)
{
	int nbColl = 0;
	btCollisionObject * object = m_collisionObjects[objectId];

	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for(int i=0; i<numManifolds; i++)
	{
		btPersistentManifold * contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject * obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject * obB = (btCollisionObject*)(contactManifold->getBody1());

		if((object == obA) || (object == obB))
		{
			int numContacts = contactManifold->getNumContacts();
			for(int j=0; j<numContacts; j++)
			{
				btManifoldPoint & pt = contactManifold->getContactPoint(j);
				if(pt.getDistance() < 0.0f)
				{
					if(collisionList && nbColl < size)
					{
						MBUserData * userData;
						
						if(obA != object)
							userData = (MBUserData *)obA->getUserPointer();
						else
							userData = (MBUserData *)obB->getUserPointer();
						
						collisionList[nbColl] = userData->m_objectId;
					}
						
					nbColl++;
					break;
				}
			}
		}
	}

	return nbColl;
}

bool MBulletContext::isObjectsCollision(unsigned int object1Id, unsigned int object2Id)
{
	btCollisionObject * object1 = m_collisionObjects[object1Id];
	btCollisionObject * object2 = m_collisionObjects[object2Id];
	

	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for(int i=0; i<numManifolds; i++)
	{
		btPersistentManifold * contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject * obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject * obB = (btCollisionObject*)(contactManifold->getBody1());

		if((object1 == obA && object2 == obB) || (object1 == obB && object2 == obA))
		{
			int numContacts = contactManifold->getNumContacts();
			for(int j=0; j<numContacts; j++)
			{
				btManifoldPoint & pt = contactManifold->getContactPoint(j);
				if(pt.getDistance() < 0.0f)
					return true;
			}
		}
	}

	return false;
}

bool MBulletContext::isRayHit(const MVector3 & start, const MVector3 & end, unsigned int * objectId, MVector3 * point, MVector3 * normal)
{
	//m_dynamicsWorld->updateAabbs();
	//m_dynamicsWorld->computeOverlappingPairs();
	
	btVector3 bstart(start.x, start.y, start.z);
	btVector3 bend(end.x, end.y, end.z);
	
	btCollisionWorld::ClosestRayResultCallback rayCallback(bstart, bend);
	m_dynamicsWorld->rayTest(bstart, bend, rayCallback);
	
	
	if(rayCallback.hasHit())
	{
		if(objectId)
		{
			MBUserData * userData = (MBUserData *)rayCallback.m_collisionObject->getUserPointer();
			*objectId = userData->m_objectId;
		}
		
		if(point)
		{
			point->x = rayCallback.m_hitPointWorld.getX();
			point->y = rayCallback.m_hitPointWorld.getY();
			point->z = rayCallback.m_hitPointWorld.getZ();
		}
		
		if(normal)
		{
			normal->x = rayCallback.m_hitNormalWorld.getX();
			normal->y = rayCallback.m_hitNormalWorld.getY();
			normal->z = rayCallback.m_hitNormalWorld.getZ();
		}
		
		return true;
    }
	
	return false;
}

// create shape
void MBulletContext::createMultiShape(unsigned int * shapeId)
{
	*shapeId = m_collisionShapes.size();
	btCompoundShape * shape = new btCompoundShape();
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createBoxShape(unsigned int * shapeId, const MVector3 & scale)
{
	*shapeId = m_collisionShapes.size();
	btCollisionShape * shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createSphereShape(unsigned int * shapeId, float radius)
{
	*shapeId = m_collisionShapes.size();
	btCollisionShape * shape = new btSphereShape(btScalar(radius));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createConeShape(unsigned int * shapeId, float radius, float height)
{
	*shapeId = m_collisionShapes.size();
	btCollisionShape * shape = new btConeShape(btScalar(radius), btScalar(height));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createCapsuleShape(unsigned int * shapeId, float radius, float height)
{
	*shapeId = m_collisionShapes.size();
	btCollisionShape * shape = new btCapsuleShape(btScalar(radius), btScalar(height));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createCylinderShape(unsigned int * shapeId, float radius, float height)
{
	*shapeId = m_collisionShapes.size();
	btCollisionShape * shape = new btCylinderShape(btVector3(radius, height*0.5f, radius));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createConvexHullShape(unsigned int * shapeId, const MVector3 * vertices, unsigned int verticesNumber, const MVector3 scale)
{
	*shapeId = m_collisionShapes.size();
	btConvexHullShape * shape = new btConvexHullShape((btScalar*)vertices, verticesNumber, sizeof(MVector3));
	shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	m_collisionShapes.push_back(shape);
}

void MBulletContext::createTriangleMeshShape(unsigned int * shapeId, const MVector3 * vertices, unsigned int verticesNumber, const void * indices, unsigned int indicesNumber, M_TYPES indicesType, const MVector3 scale)
{
	*shapeId = m_collisionShapes.size();

	PHY_ScalarType iType = PHY_INTEGER;
	int triIndexStride = sizeof(int)*3;
	
	if(indicesType == M_USHORT)
	{
		iType = PHY_SHORT;
		triIndexStride = sizeof(short)*3;
	}

	btIndexedMesh iMesh;
	iMesh.m_numTriangles = (int)(indicesNumber/3);
	iMesh.m_numVertices = (int)verticesNumber;
	iMesh.m_vertexStride = sizeof(MVector3);
	iMesh.m_triangleIndexStride = triIndexStride;
	iMesh.m_triangleIndexBase = (const unsigned char *)indices;
	iMesh.m_vertexBase = (const unsigned char *)vertices;

	btTriangleIndexVertexArray * triMesh = new btTriangleIndexVertexArray();
	triMesh->addIndexedMesh(iMesh, iType);

	btBvhTriangleMeshShape * shape = new btBvhTriangleMeshShape(triMesh, true);
	shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	m_collisionShapes.push_back(shape);
}

// delete shape
void MBulletContext::deleteShape(unsigned int * shapeId)
{
	btCollisionShape * shape = m_collisionShapes[*shapeId];
	if(shape)
	{
		SAFE_DELETE(m_collisionShapes[*shapeId]);
		*shapeId = 0;
	}
}

// add child shape to multishape
void MBulletContext::addChildShape(unsigned int multiShapeId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	transform.setRotation(
		btQuaternion(rotation.values[0], rotation.values[1], rotation.values[2], rotation.values[3])
		);

	btCompoundShape * multiShape = (btCompoundShape *)m_collisionShapes[multiShapeId];
	btCollisionShape * shape = m_collisionShapes[shapeId];
	multiShape->addChildShape(transform, shape);
}

// create constraint
void MBulletContext::createConstraint(unsigned int * constraintId, unsigned int parentObjectId, unsigned int objectId, const MVector3 & pivot, bool disableParentCollision)
{
	btRigidBody * bA = btRigidBody::upcast(m_collisionObjects[parentObjectId]);
	btRigidBody * bB = btRigidBody::upcast(m_collisionObjects[objectId]);
		
	if(bA && bB)
	{
		bA->setActivationState(DISABLE_DEACTIVATION);
		bB->setActivationState(DISABLE_DEACTIVATION);
		
		*constraintId = m_constraints.size();
		
		MVector3 position, euler;
		MQuaternion rotation;
		MMatrix4x4 matrix, matrix1, matrix2, rotMatrix;
		
		rotMatrix.setRotationEuler(90, 0, 0);
		
		getObjectTransform(parentObjectId, &position, &rotation);
		matrix1.setRotationAxis(rotation.getAngle(), rotation.getAxis());
		matrix1.setTranslationPart(position);
		
		getObjectTransform(objectId, &position, &rotation);
		matrix2.setRotationAxis(rotation.getAngle(), rotation.getAxis());
		matrix2.setTranslationPart(position + matrix2.getRotatedVector3(pivot));
		
		matrix = (matrix1.getInverse() * matrix2) * rotMatrix;
		
		euler = matrix.getEulerAngles();
		position = matrix.getTranslationPart();
		rotation.setFromAngles(euler.x, euler.y, euler.z);
		
		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(position.x, position.y, position.z));
		frameInA.setRotation(btQuaternion(rotation.values[0], rotation.values[1], rotation.values[2], rotation.values[3]));
		
		rotation.setFromAngles(90, 0, 0);
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(pivot.x, pivot.y, pivot.z));
		frameInB.setRotation(btQuaternion(rotation.values[0], rotation.values[1], rotation.values[2], rotation.values[3]));
			
		btGeneric6DofSpringConstraint * constraint = new btGeneric6DofSpringConstraint(
			*bA, *bB, frameInA, frameInB, true
		);
		
		m_constraints.push_back(constraint);
		m_dynamicsWorld->addConstraint(constraint, disableParentCollision);
	}
}

void MBulletContext::setLinearLimit(unsigned int constraintId, const MVector3 & lower, const MVector3 & upper)
{
	btGeneric6DofSpringConstraint * constraint = (btGeneric6DofSpringConstraint *)m_constraints[constraintId];
	if(constraint)
	{
		constraint->setLinearLowerLimit(btVector3(lower.x, lower.z, -upper.y));
		constraint->setLinearUpperLimit(btVector3(upper.x, upper.z, -lower.y));
	}
}

void MBulletContext::setAngularLimit(unsigned int constraintId, const MVector3 & lower, const MVector3 & upper)
{
	btGeneric6DofSpringConstraint * constraint = (btGeneric6DofSpringConstraint *)m_constraints[constraintId];
	if(constraint)
	{
		constraint->setAngularLowerLimit(btVector3(btScalar(lower.x*DEG_TO_RAD), btScalar(lower.z*DEG_TO_RAD), btScalar(-upper.y*DEG_TO_RAD)));
		constraint->setAngularUpperLimit(btVector3(btScalar(upper.x*DEG_TO_RAD), btScalar(upper.z*DEG_TO_RAD), btScalar(-lower.y*DEG_TO_RAD)));
	}
}

// delete constraint
void MBulletContext::deleteConstraint(unsigned int * constraintId)
{
	btTypedConstraint * constraint = m_constraints[*constraintId];
	if(constraint)
	{
		m_dynamicsWorld->removeConstraint(constraint);
		SAFE_DELETE(m_constraints[*constraintId]);
		*constraintId = 0;
	}
}
