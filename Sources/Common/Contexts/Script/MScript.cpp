/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MScript.cpp
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


#include "MScript.h"

static char g_currentDirectory[256] = "";
static unsigned long g_startTick = 0;
const char * LUA_VEC3 = "LUA_VEC3";



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some frequently used macros
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_OBJECT_SUBCLASS_BEGIN(type_, var_, type_enum)	\
	MObject3d * object;	\
	lua_Integer id = lua_tointeger(L, 1);	\
	if((object = getObject3d(id)))	\
	{	\
		if(object->getType() == type_enum)	\
		{	\
			type_ * var_ = (type_*)object;

#define GET_OBJECT_SUBCLASS_END()	\
		}	\
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// useful functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool isFunctionOk(lua_State * L, const char * name, unsigned int nbArgs)
{
	int nbArguments = lua_gettop(L);
	if(nbArguments < (int)nbArgs)
	{
		printf("ERROR script : \"%s\" need at least %d parameter(s)\n", name, nbArgs);
		return false;
	}
	return true;
}

static void pushFloatArray(lua_State * L, float * values, unsigned int nbValues)
{
	lua_newtable(L);
	
	for(unsigned int i=0; i<nbValues; i++)
	{
		lua_pushinteger(L, i+1);
		lua_pushnumber(L, values[i]);
		lua_rawset(L, -3);
	}
	
	if(nbValues == 3) // vec3
	{
		luaL_getmetatable(L, LUA_VEC3);
		lua_setmetatable(L, -2);
	}
}

static void pushIntArray(lua_State * L, lua_Integer * values, unsigned int nbValues)
{
	lua_newtable(L);
	
	for(unsigned int i=0; i<nbValues; i++)
	{
		lua_pushinteger(L, i+1);
		lua_pushinteger(L, values[i]);
		lua_rawset(L, -3);
	}
}

static MObject3d * getObject3d(LUA_INTEGER object)
{
	if(object == 0)
		return NULL;
	
	return (MObject3d*)object;
}

static bool getVector2(lua_State * L, int index, MVector2 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 2))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}

static bool getVector3(lua_State * L, int index, MVector3 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 3))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->z = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}

static bool getVector4(lua_State * L, int index, MVector4 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 4))
	{
		lua_pushnil(L);
		
		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->z = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		lua_next(L, index);
		vector->w = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		return true;
	}
	
	return false;
}

static void linkObjects(MObject3d *parent, MObject3d *child)
{
	if(parent == NULL || child == NULL)
		return;
	
	child->linkTo(parent);
	
	// local matrix
	MMatrix4x4 localMatrix = parent->getMatrix()->getInverse() * (*child->getMatrix());
	
	child->setPosition(localMatrix.getTranslationPart());
	child->setEulerRotation(localMatrix.getEulerAngles());
	
	float xSize = localMatrix.getRotatedVector3(MVector3(1, 0, 0)).getLength();
	float ySize = localMatrix.getRotatedVector3(MVector3(0, 1, 0)).getLength();
	float zSize = localMatrix.getRotatedVector3(MVector3(0, 0, 1)).getLength();
	
	child->setScale(MVector3(xSize, ySize, zSize));
}

static void unlinkObjects(MObject3d *parent, MObject3d *child)
{
	if(parent == NULL || child == NULL)
		return;
	
	child->unLink();
	
	// matrix
	MMatrix4x4 * matrix = child->getMatrix();
	
	child->setPosition(matrix->getTranslationPart());
	child->setEulerRotation(matrix->getEulerAngles());
	
	float xSize = matrix->getRotatedVector3(MVector3(1, 0, 0)).getLength();
	float ySize = matrix->getRotatedVector3(MVector3(0, 1, 0)).getLength();
	float zSize = matrix->getRotatedVector3(MVector3(0, 0, 1)).getLength();
	
	child->setScale(MVector3(xSize, ySize, zSize));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// vector math
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vec3__add(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a+b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a+v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__sub(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a-b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a-v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__mul(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a*b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a*v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__div(lua_State *L)
{
	MVector3 a;
	if(getVector3(L, 1, &a))
	{
		MVector3 b;
		if(getVector3(L, 2, &b))
		{
			pushFloatArray(L, a/b, 3);
			return 1;
		}
		else
		{
			float v = (float)lua_tonumber(L, 2);
			pushFloatArray(L, a/v, 3);
			return 1;
		}
	}
	return 0;
}

static int vec3__unm(lua_State *L)
{
	MVector3 a;
	getVector3(L, 1, &a);
	pushFloatArray(L, -a, 3);
	return 1;
}

void registerVec3(lua_State * L)
{
	// register vec3
	luaL_newmetatable(L, LUA_VEC3);
	
	lua_pushcfunction(L, vec3__add); lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, vec3__sub); lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, vec3__mul); lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, vec3__div); lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, vec3__unm); lua_setfield(L, -2, "__unm");
	
	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");
}

static int vec3(lua_State *L)
{
	if(! isFunctionOk(L, "vec", 3))
		return 0;
	
	float x = (float)lua_tonumber(L, 1);
	float y = (float)lua_tonumber(L, 2);
	float z = (float)lua_tonumber(L, 3);

	pushFloatArray(L, MVector3(x, y, z), 3);
	return 1;
}

static int length(lua_State *L)
{
	if(! isFunctionOk(L, "length", 1))
		return 0;
	
	MVector3 a;
	getVector3(L, 1, &a);
	lua_pushnumber(L, a.getLength());
	return 1;
}

static int normalize(lua_State *L)
{
	if(! isFunctionOk(L, "normalize", 1))
		return 0;
	
	MVector3 a;
	getVector3(L, 1, &a);
	pushFloatArray(L, a.getNormalized(), 3);
	return 1;
}

static int dot(lua_State *L)
{
	if(! isFunctionOk(L, "dot", 2))
		return 0;
	
	MVector3 a, b;
	getVector3(L, 1, &a);
	getVector3(L, 2, &b);
	lua_pushnumber(L, a.dotProduct(b));
	return 1;
}

static int cross(lua_State *L)
{
	if(! isFunctionOk(L, "cross", 2))
		return 0;
	
	MVector3 a, b;
	getVector3(L, 1, &a);
	getVector3(L, 2, &b);
	pushFloatArray(L, a.crossProduct(b), 3);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int getScene(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	
	if(! isFunctionOk(L, "getScene", 1))
		return 0;
	
	const char * name = lua_tostring(L, 1);
	if(name)
	{
		unsigned int sceneIndex;
		if(level->getSceneIndexByName(name, &sceneIndex))
		{
			lua_pushinteger(L, (lua_Integer)sceneIndex);
			return 1;
		}
	}
	
	printf("ERROR script : scene \"%s\" doesn't exit\n", name);
	return 0;
}

int getCurrentCamera(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	

	int nbArguments = lua_gettop(L);
	if(nbArguments == 1)
	{
		unsigned int sceneId = lua_tointeger(L, 0);
		scene = level->getSceneByIndex(sceneId);
	}

	MOCamera * camera = scene->getCurrentCamera();
	if(camera)
	{
		lua_pushinteger(L, (lua_Integer)camera);
		return 1;
	}

	return 0;
}

int getObject(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	

	if(! isFunctionOk(L, "getObject", 1))
		return 0;

	int nbArguments = lua_gettop(L);
	if(nbArguments == 2)
	{
		unsigned int sceneId = lua_tointeger(L, 1);
		scene = level->getSceneByIndex(sceneId);
	}
	
	const char * name = lua_tostring(L, nbArguments);
	if(name)
	{
		MObject3d * object = scene->getObjectByName(name);
		if(object)
		{
			lua_pushinteger(L, (lua_Integer)object);
			return 1;
		}
	}

	printf("ERROR script : object \"%s\" doesn't exit\n", name);
	return 0;
}

int getClone(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	
	if(! isFunctionOk(L, "getClone", 1))
		return 0;
	
	int nbArguments = lua_gettop(L);
	if(nbArguments == 2)
	{
		unsigned int sceneId = lua_tointeger(L, 1);
		scene = level->getSceneByIndex(sceneId);
	}
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, nbArguments);
	
	if((object = getObject3d(id)))
	{
		MObject3d * cloneObj = NULL;
		
		switch(object->getType())
		{
			case M_OBJECT3D_CAMERA:
				cloneObj = scene->addNewCamera(*(MOCamera*)object);
				break;
			case M_OBJECT3D_LIGHT:
				cloneObj = scene->addNewLight(*(MOLight*)object);
				break;
			case M_OBJECT3D_ENTITY:
				cloneObj = scene->addNewEntity(*(MOEntity*)object);
				scene->prepareCollisionObject((MOEntity*)cloneObj);
				break;
			case M_OBJECT3D_SOUND:
				cloneObj = scene->addNewSound(*(MOSound*)object);
				break;
			case M_OBJECT3D_TEXT:
				cloneObj = scene->addNewText(*(MOText*)object);
				break;
		}
		
		if(cloneObj)
		{
			char name[256];
			sprintf(name, "%s_clone%d", object->getName(), scene->getObjectsNumber());
			cloneObj->setName(name);
		}
		
		lua_pushinteger(L, (lua_Integer)cloneObj);
		return 1;
	}
	
	return 0;
}

int getParent(lua_State * L)
{
	if(! isFunctionOk(L, "getParent", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		lua_pushinteger(L, (lua_Integer)object->getParent());
		return 1;
	}
	
	return 0;
}

int getChilds(lua_State * L)
{
	if(! isFunctionOk(L, "getChilds", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		unsigned int cSize = object->getChildsNumber();
		lua_Integer * childs = new lua_Integer[cSize];
		
		for(unsigned int c=0; c<cSize; c++)
			childs[c] = (lua_Integer)object->getChild(c);
		
		pushIntArray(L, childs, cSize);
		
		delete [] childs;
		return 1;
	}
	
	return 0;
}

int getProjectedPoint(lua_State * L)
{
	if(! isFunctionOk(L, "getProjectedPoint", 2))
		return 0;
	
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MVector3 vec;
			if(getVector3(L, 2, &vec))
			{
				MOCamera * camera = (MOCamera *)object;
				MVector3 result = camera->getProjectedPoint(vec);
				result.x = result.x/(float)width;
				result.y = 1.0f - (result.y/(float)height);
				pushFloatArray(L, result, 3);
				return 1;
			}
		}
	}
	
	return 0;
}

int getUnProjectedPoint(lua_State * L)
{
	if(! isFunctionOk(L, "getUnProjectedPoint", 2))
		return 0;
	
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MVector3 vec;
			if(getVector3(L, 2, &vec))
			{
				MOCamera * camera = (MOCamera *)object;
				MVector3 result = camera->getUnProjectedPoint(MVector3(vec.x*width, (1-vec.y)*height, vec.z));
				pushFloatArray(L, result, 3);
				return 1;
			}
		}
	}
	
	return 0;
}

int rotate(lua_State * L)
{
	if(! isFunctionOk(L, "rotate", 3))
		return 0;

	int nbArguments = lua_gettop(L);

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 axis;
		if(getVector3(L, 2, &axis))
		{
			// get angle
			float angle = (float)lua_tonumber(L, 3);

			// is local ?
			bool local = false;
			if(nbArguments > 3)
			{
				const char * mode = lua_tostring(L, 4);
				if(strcmp(mode, "local") == 0)
					local = true;
			}
			
			// rotate
			if(local)
				object->addAxisAngleRotation(axis, angle);
			else
				object->addAxisAngleRotation(object->getInverseRotatedVector(axis), angle);
		}
	}

	return 0;
}

int translate(lua_State * L)
{
	if(! isFunctionOk(L, "translate", 2))
		return 0;

	int nbArguments = lua_gettop(L);

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 axis;
		if(getVector3(L, 2, &axis))
		{
			// is local ?
			bool local = false;
			if(nbArguments > 2)
			{
				const char * mode = lua_tostring(L, 3);
				if(strcmp(mode, "local") == 0)
					local = true;
			}

			// rotate
			if(local)
			{
				axis = object->getRotatedVector(axis);
				object->setPosition(object->getPosition() + axis);
			}
			else
				object->setPosition(object->getPosition() + axis);
		}
	}

	return 0;
}

int getPosition(lua_State * L)
{
	if(! isFunctionOk(L, "getPosition", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getPosition(), 3);
		return 1;
	}

	return 0;
}

int getRotation(lua_State * L)
{
	if(! isFunctionOk(L, "getRotation", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getEulerRotation(), 3);
		return 1;
	}

	return 0;
}

int getScale(lua_State * L)
{
	if(! isFunctionOk(L, "getScale", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getScale(), 3);
		return 1;
	}

	return 0;
}

int getTransformedPosition(lua_State * L)
{
	if(! isFunctionOk(L, "getTransformedPosition", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getTransformedPosition(), 3);
		return 1;
	}
	
	return 0;
}

int getTransformedRotation(lua_State * L)
{
	if(! isFunctionOk(L, "getTransformedRotation", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getTransformedRotation(), 3);
		return 1;
	}
	
	return 0;
}

int getTransformedScale(lua_State * L)
{
	if(! isFunctionOk(L, "getTransformedScale", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getTransformedScale(), 3);
		return 1;
	}
	
	return 0;
}

int setPosition(lua_State * L)
{
	if(! isFunctionOk(L, "setPosition", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 position;
		if(getVector3(L, 2, &position))
		{
			object->setPosition(position);
		}
	}

	return 0;
}

int setRotation(lua_State * L)
{
	if(! isFunctionOk(L, "setRotation", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 rotation;
		if(getVector3(L, 2, &rotation))
		{
			object->setEulerRotation(rotation);
		}
	}

	return 0;
}

int setScale(lua_State * L)
{
	if(! isFunctionOk(L, "setScale", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 scale;
		if(getVector3(L, 2, &scale))
		{
			object->setScale(scale);
		}
	}

	return 0;
}

int updateMatrix(lua_State * L)
{
	if(! isFunctionOk(L, "updateMatrix", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		object->updateMatrix();
	}
	
	return 0;
}

int getMatrix(lua_State * L)
{
	if(! isFunctionOk(L, "getMatrix", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		pushFloatArray(L, object->getMatrix()->entries, 16);
		return 1;
	}
	
	return 0;
}

int getInverseRotatedVector(lua_State * L)
{
	if(! isFunctionOk(L, "getInverseRotatedVector", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		MVector3 vec;
		if(getVector3(L, 2, &vec))
		{
			pushFloatArray(L, object->getInverseRotatedVector(vec), 3);
			return 1;
		}
	}
	
	return 0;
}

int getRotatedVector(lua_State * L)
{
	if(! isFunctionOk(L, "getRotatedVector", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		MVector3 vec;
		if(getVector3(L, 2, &vec))
		{
			pushFloatArray(L, object->getRotatedVector(vec), 3);
			return 1;
		}
	}
	
	return 0;
}

int getInverseVector(lua_State * L)
{
	if(! isFunctionOk(L, "getInverseVector", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		MVector3 vec;
		if(getVector3(L, 2, &vec))
		{
			pushFloatArray(L, object->getInversePosition(vec), 3);
			return 1;
		}
	}
	
	return 0;
}

int getTransformedVector(lua_State * L)
{
	if(! isFunctionOk(L, "getTransformedVector", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		MVector3 vec;
		if(getVector3(L, 2, &vec))
		{
			pushFloatArray(L, object->getTransformedVector(vec), 3);
			return 1;
		}
	}
	
	return 0;
}

int isVisible(lua_State * L)
{
	if(! isFunctionOk(L, "isVisible", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		int vis = object->isVisible();
		lua_pushboolean(L, vis);
		return 1;
	}

	return 0;
}

int activate(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(L, "activate", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		object->setActive(true);
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
				physics->activateObject(phyProps->getCollisionObjectId());
		}
	}

	return 0;
}

int deactivate(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "deactivate", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		object->setActive(false);
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
				physics->deactivateObject(phyProps->getCollisionObjectId());
		}
	}

	return 0;
}

int isActive(lua_State * L)
{
	if(! isFunctionOk(L, "isActive", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		lua_pushboolean(L, object->isActive());
		return 1;
	}
	
	return 0;
}

int getName(lua_State * L)
{
	if(! isFunctionOk(L, "getName", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		lua_pushstring(L, object->getName());
		return 1;
	}
	
	return 0;
}

int setParent(lua_State * L)
{
	if(! isFunctionOk(L, "setParent", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id  = lua_tointeger(L, 1);
	lua_Integer id2 = lua_tointeger(L, 2);
	
	if((object = getObject3d(id)))
	{
		MObject3d * parent = getObject3d(id2);
		if(parent)
			linkObjects(parent, object);
		else
			unlinkObjects(parent, object);
		
		object->updateMatrix();
		return 0;
	}
	
	return 0;
}

int changeAnimation(lua_State * L)
{
	if(! isFunctionOk(L, "changeAnimation", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;

			unsigned int anim = (unsigned int)lua_tointeger(L, 2);
			entity->changeAnimation(anim);
			return 0;
		}
	}

	return 0;
}

int isAnimationOver(lua_State * L)
{
	if(! isFunctionOk(L, "isAnimationOver", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			int isAnimOver = entity->isAnimationOver();
			lua_pushboolean(L, isAnimOver);
			return 1;
		}
	}

	return 0;
}

int getCurrentAnimation(lua_State * L)
{
	if(! isFunctionOk(L, "getCurrentAnimation", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			int anim = (int)entity->getAnimationId();
			lua_pushinteger(L, anim);
			return 1;
		}
	}

	return 0;
}

int setAnimationSpeed(lua_State * L)
{
	if(! isFunctionOk(L, "setAnimationSpeed", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(MOEntity, entity, M_OBJECT3D_ENTITY)
			entity->setAnimationSpeed((float)lua_tonumber(L, 2));
			return 0;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getAnimationSpeed(lua_State * L)
{
	if(! isFunctionOk(L, "getAnimationSpeed", 1))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(MOEntity, entity, M_OBJECT3D_ENTITY)
			lua_pushnumber(L, (lua_Number)entity->getAnimationSpeed());
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int setCurrentFrame(lua_State * L)
{
	if(! isFunctionOk(L, "setCurrentFrame", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(MOEntity, entity, M_OBJECT3D_ENTITY)
			entity->setCurrentFrame((float)lua_tonumber(L, 2));
			return 0;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getCurrentFrame(lua_State * L)
{
	if(! isFunctionOk(L, "getCurrentFrame", 1))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(MOEntity, entity, M_OBJECT3D_ENTITY)
			lua_pushnumber(L, (lua_Number)entity->getCurrentFrame());
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getGravity(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	int nbArguments = lua_gettop(L);
	if(nbArguments == 1)
	{
		unsigned int sceneId = lua_tointeger(L, 1);
		scene = level->getSceneByIndex(sceneId);
	}
	
	pushFloatArray(L, scene->getGravity(), 3);

	return 1;
}

int setGravity(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	if(! isFunctionOk(L, "setGravity", 1))
		return 0;

	int nbArguments = lua_gettop(L);
	if(nbArguments == 2)
	{
		unsigned int sceneId = lua_tointeger(L, 1);
		scene = level->getSceneByIndex(sceneId);
	}
	
	MVector3 gravity;
	if(getVector3(L, nbArguments, &gravity))
		scene->setGravity(gravity);

	return 0;
}

int changeCurrentCamera(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	if(! isFunctionOk(L, "changeCurrentCamera", 1))
		return 0;

	int nbArguments = lua_gettop(L);
	if(nbArguments == 2)
	{
		unsigned int sceneId = lua_tointeger(L, 1);
		scene = level->getSceneByIndex(sceneId);
	}
	
	lua_Integer id = lua_tointeger(L, nbArguments);
	MObject3d * object = getObject3d(id);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			unsigned int i;
			unsigned int size = scene->getCamerasNumber();
			for(i=0; i<size; i++)
			{
				if(object == (MObject3d *)scene->getCameraByIndex(i))
				{
					scene->setCurrentCameraId(i);
					break;
				}
			}
		}
	}

	return 0;
}

int addCentralForce(lua_State * L)
{
	int nbArguments = lua_gettop(L);
	if(! isFunctionOk(L, "addCentralForce", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 force;
			if(getVector3(L, 2, &force))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = lua_tostring(L, 3);
					if(strcmp(mode, "local") == 0)
						local = true;
				}

				MOEntity * entity = (MOEntity*)object;
				MPhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						force = object->getRotatedVector(force);

					MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
					physics->addCentralForce(phyProps->getCollisionObjectId(), force);
				}
			}
		}
	}

	return 0;
}

int clearForces(lua_State * L)
{
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				physics->clearForces(phyProps->getCollisionObjectId());
			}
		}
	}

	return 0;
}

int addTorque(lua_State * L)
{
	int nbArguments = lua_gettop(L);
	if(! isFunctionOk(L, "addTorque", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 torque;
			if(getVector3(L, 2, &torque))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = lua_tostring(L, 3);
					if(strcmp(mode, "local") == 0)
						local = true;
				}

				MOEntity * entity = (MOEntity*)object;
				MPhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						torque = object->getRotatedVector(torque);

					MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
					physics->addTorque(phyProps->getCollisionObjectId(), torque);
				}
			}
		}
	}

	return 0;
}

int getLinearDamping(lua_State * L)
{
	if(! isFunctionOk(L, "getLinearDamping", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = phyProps->getLinearDamping();
				lua_pushnumber(L, (lua_Number)linearDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setLinearDamping(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(L, "setLinearDamping", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = (float)lua_tonumber(L, 2);
				phyProps->setLinearDamping(linearDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int getAngularDamping(lua_State * L)
{
	if(! isFunctionOk(L, "getAngularDamping", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = phyProps->getAngularDamping();
				lua_pushnumber(L, (lua_Number)angularDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setAngularDamping(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(L, "setAngularDamping", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = (float)lua_tonumber(L, 2);
				phyProps->setAngularDamping(angularDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int getCentralForce(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "getCentralForce", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 force;
				physics->getCentralForce(phyProps->getCollisionObjectId(), &force);
				pushFloatArray(L, force, 3);
				return 1;
			}
		}
	}
	
	return 0;
}

int getTorque(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "getTorque", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 force;
				physics->getTorque(phyProps->getCollisionObjectId(), &force);
				pushFloatArray(L, force, 3);
				return 1;
			}
		}
	}
	
	return 0;
}

int getMass(lua_State * L)
{
	if(! isFunctionOk(L, "getMass", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float mass = phyProps->getMass();
				lua_pushnumber(L, (lua_Number)mass);
				return 1;
			}
		}
	}
	
	return 0;
}

int setMass(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "setMass", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float mass = (float)lua_tonumber(L, 2);
				phyProps->setMass(mass);
				physics->setObjectMass(phyProps->getCollisionObjectId(), mass);
				return 0;
			}
		}
	}
	
	return 0;
}

int getFriction(lua_State * L)
{
	if(! isFunctionOk(L, "getFriction", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float friction = phyProps->getFriction();
				lua_pushnumber(L, (lua_Number)friction);
				return 1;
			}
		}
	}
	
	return 0;
}

int setFriction(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "setFriction", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float friction = (float)lua_tonumber(L, 2);
				phyProps->setFriction(friction);
				physics->setObjectFriction(phyProps->getCollisionObjectId(), friction);
				return 0;
			}
		}
	}
	
	return 0;
}

int getRestitution(lua_State * L)
{
	if(! isFunctionOk(L, "getRestitution", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float restitution = phyProps->getRestitution();
				lua_pushnumber(L, (lua_Number)restitution);
				return 1;
			}
		}
	}
	
	return 0;
}

int setRestitution(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "setRestitution", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float restitution = (float)lua_tonumber(L, 2);
				phyProps->setRestitution(restitution);
				physics->setObjectRestitution(phyProps->getCollisionObjectId(), restitution);
				return 0;
			}
		}
	}
	
	return 0;
}

int getAngularFactor(lua_State * L)
{
	if(! isFunctionOk(L, "getAngularFactor", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float aFactor = phyProps->getAngularFactor();
				lua_pushnumber(L, (lua_Number)aFactor);
				return 1;
			}
		}
	}
	
	return 0;
}

int setAngularFactor(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "setAngularFactor", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float aFactor = (float)lua_tonumber(L, 2);
				phyProps->setAngularFactor(aFactor);
				physics->setObjectAngularFactor(phyProps->getCollisionObjectId(), aFactor);
				return 0;
			}
		}
	}
	
	return 0;
}

int getLinearFactor(lua_State * L)
{
	if(! isFunctionOk(L, "getLinearFactor", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 * linFactor = phyProps->getLinearFactor();
				pushFloatArray(L, *linFactor, 3);
				return 1;
			}
		}
	}
	
	return 0;
}

int setLinearFactor(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
	
	if(! isFunctionOk(L, "setLinearFactor", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 linFactor;
				if(getVector3(L, 2, &linFactor))
				{
					phyProps->setLinearFactor(linFactor);
					physics->setObjectLinearFactor(phyProps->getCollisionObjectId(), linFactor);
					return 0;
				}
			}
		}
	}
	
	return 0;
}

int getNumCollisions(lua_State * L)
{
	if(! isFunctionOk(L, "getNumCollisions", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectInCollision(phyProps->getCollisionObjectId());
				lua_pushinteger(L, collision);
				return 1;
			}
		}
	}
	
	return 0;
}

int isCollisionTest(lua_State * L)
{
	if(! isFunctionOk(L, "isCollisionTest", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = MIN(1, physics->isObjectInCollision(phyProps->getCollisionObjectId()));
				lua_pushboolean(L, collision);
				return 1;
			}
		}
	}

	return 0;
}

int isCollisionBetween(lua_State * L)
{
	if(! isFunctionOk(L, "isCollisionBetween", 2))
		return 0;

	MObject3d * object1;
	MObject3d * object2;

	lua_Integer id1 = lua_tointeger(L, 1);
	lua_Integer id2 = lua_tointeger(L, 2);
	if((object1 = getObject3d(id1)) && (object2 = getObject3d(id2)))
	{
		if((object1->getType() == M_OBJECT3D_ENTITY) && (object2->getType() == M_OBJECT3D_ENTITY))
		{
			MOEntity * entity1 = (MOEntity*)object1;
			MOEntity * entity2 = (MOEntity*)object2;

			MPhysicsProperties * phyProps1 = entity1->getPhysicsProperties();
			MPhysicsProperties * phyProps2 = entity2->getPhysicsProperties();
			if(phyProps1 && phyProps2)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectsCollision(
					phyProps1->getCollisionObjectId(),
					phyProps2->getCollisionObjectId()
					);
				lua_pushboolean(L, collision);
				return 1;
			}
		}
	}

	return 0;
}

int rayHit(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	if(! isFunctionOk(L, "rayHit", 2))
		return 0;
	
	int nbArguments = lua_gettop(L);
	
	MVector3 start, end;
	if(getVector3(L, 1, &start) && getVector3(L, 2, &end))
	{
		MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
		
		unsigned int collisionObjId;
		MVector3 point;
		
		// ray test
		if(physics->isRayHit(start, end, &collisionObjId, &point))
		{
			if(nbArguments == 3)
			{
				MObject3d * object;
				lua_Integer id = lua_tointeger(L, 3);
				if((object = getObject3d(id)))
				{
					if(object->getType() == M_OBJECT3D_ENTITY)
					{
						MOEntity * entity = (MOEntity*)object;
						MPhysicsProperties * phyProps = entity->getPhysicsProperties();
						if(phyProps)
						{
							if(phyProps->getCollisionObjectId() == collisionObjId)
							{
								pushFloatArray(L, point, 3);
								return 1;
							}
						}
					}
				}
			}
			else
			{
				pushFloatArray(L, point, 3);
				
				unsigned int e, eSize = scene->getEntitiesNumber();
				for(e=0; e<eSize; e++)
				{
					MOEntity * entity = scene->getEntityByIndex(e);
					MPhysicsProperties * phyProps = entity->getPhysicsProperties();
					if(phyProps)
					{
						if(phyProps->getCollisionObjectId() == collisionObjId)
						{
							lua_pushinteger(L, (lua_Integer)entity);
							return 2;
						}
					}
				}
				
				return 1;
			}
		}
	}
	
	return 0;
}

int isKeyPressed(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "isKeyPressed", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyPressed = input->isKeyPressed(name);
		lua_pushboolean(L, keyPressed);
		return 1;
	}

	return 0;
}

int onKeyDown(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "onKeyDown", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyDown = input->onKeyDown(name);
		lua_pushboolean(L, keyDown);
		return 1;
	}

	return 0;
}

int onKeyUp(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "onKeyUp", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyUp = input->onKeyUp(name);
		lua_pushboolean(L, keyUp);
		return 1;
	}

	return 0;
}

int getAxis(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "getAxis", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		float axis = input->getAxis(name);
		lua_pushnumber(L, axis);
		return 1;
	}

	return 0;
}

int getProperty(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "getProperty", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int prop = input->getProperty(name);
		lua_pushinteger(L, prop);
		return 1;
	}

	return 0;
}

int getTouchPosition(lua_State * L)
{
    MInputContext * input = MEngine::getInstance()->getInputContext();
    
    if (!isFunctionOk(L, "getTouchPosition", 1))
        return 0;
    
    unsigned int touchID = (unsigned int)lua_tointeger(L, 1);
    pushFloatArray(L, input->getTouchPosition(touchID), 2);
    
    return 1;
}

int getLastTouchPosition(lua_State * L)
{
    MInputContext * input = MEngine::getInstance()->getInputContext();
    
    if (!isFunctionOk(L, "getLastTouchPosition", 1))
        return 0;
    
    unsigned int touchID = (unsigned int)lua_tointeger(L, 1);
    pushFloatArray(L, input->getLastTouchPosition(touchID), 2);
    
    return 1;
}

int getTouchPhase(lua_State * L)
{
    MInputContext * input = MEngine::getInstance()->getInputContext();
    
    if (!isFunctionOk(L, "getTouchPhase", 1))
        return 0;
    
    unsigned int touchID = (unsigned int)lua_tointeger(L, 1);
    lua_pushinteger(L, input->getTouchPhase(touchID));
    
    return 1;
}

int playSound(lua_State * L)
{
	if(! isFunctionOk(L, "playSound", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->play();
		}
	}

	return 0;
}

int pauseSound(lua_State * L)
{
	if(! isFunctionOk(L, "pauseSound", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->pause();
		}
	}

	return 0;
}

int stopSound(lua_State * L)
{
	if(! isFunctionOk(L, "stopSound", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->stop();
		}
	}

	return 0;
}

int changeScene(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	if(! isFunctionOk(L, "changeScene", 1))
		return 0;

	lua_Integer id = lua_tointeger(L, 1);
	level->changeCurrentScene(id);
	
	return 0;
}

int getCurrentSceneId(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	lua_pushinteger(L, (int)level->getCurrentSceneId());
	return 1;
}

int getScenesNumber(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	lua_pushinteger(L, level->getScenesNumber());
	return 1;
}

int loadLevel(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();

	if(! isFunctionOk(L, "loadLevel", 1))
		return 0;

	const char * filename = lua_tostring(L, 1);
	if(filename)
		engine->requestLoadLevel(filename);

	return 0;
}

int quit(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	engine->setActive(false);
	return 0;
}

int doesLevelExist(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();

	if(! isFunctionOk(L, "doesLevelExist", 1))
		return 0;

	const char * filename = lua_tostring(L, 1);

	lua_pushnumber(L, (lua_Number)engine->doesLevelExist(filename));
	return 1;
}

int getLightColor(lua_State * L)
{
	if(! isFunctionOk(L, "getLightColor", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			pushFloatArray(L, light->getColor(), 3);
			return 1;
		}
	}

	return 0;
}

int getLightRadius(lua_State * L)
{
	if(! isFunctionOk(L, "getLightRadius", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, (lua_Number)light->getRadius());
			return 1;
		}
	}

	return 0;
}

int getLightIntensity(lua_State * L)
{
	if(! isFunctionOk(L, "getLightIntensity", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, (lua_Number)light->getIntensity());
			return 1;
		}
	}

	return 0;
}

int setLightColor(lua_State * L)
{
	if(! isFunctionOk(L, "setLightColor", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_LIGHT && getVector3(L, 2, &color))
		{
			MOLight * light = (MOLight*)object;
			light->setColor(color);
			return 0;
		}
	}

	return 0;
}

int setLightRadius(lua_State * L)
{
	if(! isFunctionOk(L, "setLightRadius", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float radius = (float)lua_tonumber(L, 2);

			MOLight * light = (MOLight*)object;
			light->setRadius(radius);
			return 0;
		}
	}

	return 0;
}

int setLightIntensity(lua_State * L)
{
	if(! isFunctionOk(L, "setLightIntensity", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float intensity = (float)lua_tonumber(L, 2);

			MOLight * light = (MOLight*)object;
			light->setIntensity(intensity);
			return 0;
		}
	}

	return 0;
}

int enableShadow(lua_State * L)
{
	if(! isFunctionOk(L, "enableShadow", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			bool shadow = lua_toboolean(L, 2) == 1;
			MOLight * light = (MOLight*)object;
			light->castShadow(shadow);
			return 0;
		}
	}
	
	return 0;
}

int isCastingShadow(lua_State * L)
{
	if(! isFunctionOk(L, "isCastingShadow", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushboolean(L, light->isCastingShadow());
			return 1;
		}
	}
	
	return 0;
}

int setlightShadowQuality(lua_State * L)
{
	if(! isFunctionOk(L, "setlightShadowQuality", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			int quality = lua_tointeger(L, 2);
			MOLight * light = (MOLight*)object;
			light->setShadowQuality(quality);
			return 0;
		}
	}
	
	return 0;
}

int setlightShadowBias(lua_State * L)
{
	if(! isFunctionOk(L, "setlightShadowBias", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float bias = lua_tonumber(L, 2);
			MOLight * light = (MOLight*)object;
			light->setShadowBias(bias);
			return 0;
		}
	}
	
	return 0;
}

int setlightShadowBlur(lua_State * L)
{
	if(! isFunctionOk(L, "setlightShadowBlur", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float blur = lua_tonumber(L, 2);
			MOLight * light = (MOLight*)object;
			light->setShadowBlur(blur);
			return 0;
		}
	}
	
	return 0;
}

int getlightShadowQuality(lua_State * L)
{
	if(! isFunctionOk(L, "getlightShadowQuality", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushinteger(L, light->getShadowQuality());
			return 1;
		}
	}
	
	return 0;
}

int getlightShadowBias(lua_State * L)
{
	if(! isFunctionOk(L, "getlightShadowBias", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, light->getShadowBias());
			return 1;
		}
	}
	
	return 0;
}

int getlightShadowBlur(lua_State * L)
{
	if(! isFunctionOk(L, "getlightShadowBlur", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, light->getShadowBlur());
			return 1;
		}
	}
	
	return 0;
}

int setlightSpotAngle(lua_State * L)
{
	if(! isFunctionOk(L, "setlightSpotAngle", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float angle = lua_tonumber(L, 2);
			MOLight * light = (MOLight*)object;
			light->setSpotAngle(angle);
			return 0;
		}
	}
	
	return 0;
}

int setlightSpotExponent(lua_State * L)
{
	if(! isFunctionOk(L, "setlightSpotExponent", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float exponent = lua_tonumber(L, 2);
			MOLight * light = (MOLight*)object;
			light->setSpotExponent(exponent);
			return 0;
		}
	}
	
	return 0;
}

int getlightSpotAngle(lua_State * L)
{
	if(! isFunctionOk(L, "getlightSpotAngle", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, light->getSpotAngle());
			return 1;
		}
	}
	
	return 0;
}

int getlightSpotExponent(lua_State * L)
{
	if(! isFunctionOk(L, "getlightSpotExponent", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, light->getSpotExponent());
			return 1;
		}
	}
	
	return 0;
}


int getSoundGain(lua_State * L)
{
	if(! isFunctionOk(L, "getSoundGain", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			lua_pushnumber(L, (lua_Number)sound->getGain());
			return 1;
		}
	}

	return 0;
}

int setSoundGain(lua_State * L)
{
	if(! isFunctionOk(L, "setSoundGain", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			float gain = (float)lua_tonumber(L, 2);
			MOSound * sound = (MOSound*)object;
			sound->setGain(gain);
			return 0;
		}
	}

	return 0;
}

int setCameraClearColor(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraClearColor", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_CAMERA && getVector3(L, 2, &color))
		{
			MOCamera * camera = (MOCamera*)object;
			camera->setClearColor(color);
			return 0;
		}
	}

	return 0;
}

int getCameraClearColor(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraClearColor", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			pushFloatArray(L, camera->getClearColor(), 3);
			return 1;
		}
	}

	return 0;
}

int setCameraNear(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraNear", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipNear = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setClippingNear(clipNear);
			return 0;
		}
	}

	return 0;
}

int getCameraNear(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraNear", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getClippingNear());
			return 1;
		}
	}

	return 0;
}

int setCameraFar(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFar", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipFar = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setClippingFar(clipFar);
			return 0;
		}
	}

	return 0;
}

int getCameraFar(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFar", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getClippingFar());
			return 1;
		}
	}

	return 0;
}

int setCameraFov(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFov", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fov = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setFov(fov);
			return 0;
		}
	}

	return 0;
}

int getCameraFov(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFov", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getFov());
			return 1;
		}
	}

	return 0;
}

int setCameraFogDistance(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFogDistance", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fogDist = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setFogDistance(fogDist);
			return 0;
		}
	}

	return 0;
}

int getCameraFogDistance(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFogDistance", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getFogDistance());
			return 1;
		}
	}

	return 0;
}

int enableCameraOrtho(lua_State * L)
{
	if(! isFunctionOk(L, "enableCameraOrtho", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool ortho = lua_toboolean(L, 2) == 1;
			MOCamera * camera = (MOCamera*)object;
			camera->enableOrtho(ortho);
			return 0;
		}
	}

	return 0;
}

int isCameraOrtho(lua_State * L)
{
	if(! isFunctionOk(L, "isCameraOrtho", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushboolean(L, camera->isOrtho());
			return 1;
		}
	}

	return 0;
}

int enableCameraFog(lua_State * L)
{
	if(! isFunctionOk(L, "enableCameraFog", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool fog = lua_toboolean(L, 2) == 1;
			MOCamera * camera = (MOCamera*)object;
			camera->enableFog(fog);
			return 0;
		}
	}

	return 0;
}

int isCameraFogEnabled(lua_State * L)
{
	if(! isFunctionOk(L, "isCameraFogEnabled", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushboolean(L, camera->hasFog());
			return 1;
		}
	}

	return 0;
}

int enableCameraLayer(lua_State * L)
{
	if(! isFunctionOk(L, "enableCameraLayer", 2))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			unsigned int sceneId = (unsigned int)lua_tointeger(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setSceneLayer(sceneId+1);
			return 0;
		}
	}
	
	return 0;
}

int disableCameraLayer(lua_State * L)
{
	if(! isFunctionOk(L, "disableCameraLayer", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			camera->setSceneLayer(0);
			return 0;
		}
	}
	
	return 0;
}

int enableRenderToTexture(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MRenderingContext * render = engine->getRenderingContext();
	MLevel * level = engine->getLevel();
	
	if(! isFunctionOk(L, "enableRenderToTexture", 4))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			const char * name = lua_tostring(L, 2);
			if(name)
			{
				MOCamera * camera = (MOCamera *)object;
				
				unsigned int width = (unsigned int)lua_tointeger(L, 3);
				unsigned int height = (unsigned int)lua_tointeger(L, 4);
				
				char globalFilename[256];
				getGlobalFilename(globalFilename, system->getWorkingDirectory(), name);
				
				MTextureRef * colorTexture = level->loadTexture(globalFilename, 0, 0);
				MTextureRef * depthTexture = level->loadTexture(object->getName(), 0, 0);
				
				colorTexture->clear();
				depthTexture->clear();
				
				camera->setRenderColorTexture(colorTexture);
				camera->setRenderDepthTexture(depthTexture);
				
				unsigned int m_colorTextureId, m_depthTextureId;
				
				render->createTexture(&m_colorTextureId);
				render->bindTexture(m_colorTextureId);
				render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
				render->setTextureUWrapMode(M_WRAP_CLAMP);
				render->setTextureVWrapMode(M_WRAP_CLAMP);
				render->texImage(0, width, height, M_UBYTE, M_RGBA, 0);
				
				render->createTexture(&m_depthTextureId);
				render->bindTexture(m_depthTextureId);
				render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
				render->setTextureUWrapMode(M_WRAP_CLAMP);
				render->setTextureVWrapMode(M_WRAP_CLAMP);
				render->texImage(0, width, height, M_UINT, M_DEPTH, 0);
				render->bindTexture(0);
				
				colorTexture->setTextureId(m_colorTextureId);
				colorTexture->setWidth(width);
				colorTexture->setHeight(height);
				depthTexture->setTextureId(m_depthTextureId);
				depthTexture->setWidth(width);
				depthTexture->setHeight(height);
			}
			
			return 0;
		}
	}
	
	return 0;
}

int disableRenderToTexture(lua_State * L)
{
	if(! isFunctionOk(L, "disableRenderToTexture", 1))
		return 0;
	
	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);
	
	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera *)object;
				
			MTextureRef * depthTexture = camera->getRenderDepthTexture();
			if(depthTexture)
				depthTexture->clear();
				
			camera->setRenderColorTexture(NULL);
			camera->setRenderDepthTexture(NULL);
			
			return 0;
		}
	}
	
	return 0;
}

int getBehaviorVariable(lua_State * L)
{
	if(! isFunctionOk(L, "getBehaviorVariable", 3))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		unsigned int bId = (unsigned int)lua_tointeger(L, 2);
		const char * varName = lua_tostring(L, 3);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				MBehavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					MVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								lua_pushboolean(L, *(bool *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								lua_pushinteger(L, *(int *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_FLOAT:
							{
								lua_pushnumber(L, *(float *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_STRING:
							{
								lua_pushstring(L, ((MString *)variable.getPointer())->getData());
								return 1;
							}
						case M_VARIABLE_VEC2:
							{
								pushFloatArray(L, *(MVector2 *)variable.getPointer(), 2);
								return 1;
							}
						case M_VARIABLE_VEC3:
							{
								pushFloatArray(L, *(MVector3 *)variable.getPointer(), 3);
								return 1;
							}
						case M_VARIABLE_VEC4:
							{
								pushFloatArray(L, *(MVector4 *)variable.getPointer(), 4);
								return 1;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int setBehaviorVariable(lua_State * L)
{
	if(! isFunctionOk(L, "setBehaviorVariable", 4))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		unsigned int bId = (unsigned int)lua_tointeger(L, 2);
		const char * varName = lua_tostring(L, 3);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				MBehavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					MVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								bool val = lua_toboolean(L, 4) == 1;
								*(bool *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								int val = lua_tointeger(L, 4);
								*(int *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_FLOAT:
							{
								float val = (float)lua_tonumber(L, 4);
								*(float *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_STRING:
							{
								const char * str = lua_tostring(L, 4);
								if(str)
									((MString *)variable.getPointer())->set(str);
								return 0;
							}
						case M_VARIABLE_VEC2:
							{
								MVector2 vec;
								if(getVector2(L, 4, &vec))
									*(MVector2 *)variable.getPointer() = vec;
								return 0;
							}
						case M_VARIABLE_VEC3:
							{
								MVector3 vec;
								if(getVector3(L, 4, &vec))
									*(MVector3 *)variable.getPointer() = vec;
								return 0;
							}
						case M_VARIABLE_VEC4:
							{
								MVector4 vec;
								if(getVector4(L, 4, &vec))
									*(MVector4 *)variable.getPointer() = vec;
								return 0;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int centerCursor(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MInputContext * input = engine->getInputContext();

	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);
	int x = width/2;
	int y = height/2;

	system->setCursorPosition(x, y);
	input->setAxis("MOUSE_X", (float)(x / (float)width));
	input->setAxis("MOUSE_Y", (float)(y / (float)height));

	return 0;
}

int hideCursor(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	system->hideCursor();
	
	return 0;
}

int showCursor(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	system->showCursor();
	
	return 0;
}

int getText(lua_State * L)
{
	if(! isFunctionOk(L, "getText", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			const char * text = ((MOText *)object)->getText();
			if(text){
				lua_pushstring(L, text);
				return 1;
			}
		}
	}

	return 0;
}

int setText(lua_State * L)
{
	if(! isFunctionOk(L, "setText", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		const char * text = lua_tostring(L, 2);
		if((text) && (object->getType() == M_OBJECT3D_TEXT))
		{
			((MOText *)object)->setText(text);
		}
	}

	return 0;
}

int getTextColor(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	if(! isFunctionOk(L, "getTextColor", 1))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 color = ((MOText *)object)->getColor();
			script->pushFloatArray(color, 4);
			return 1;
		}
	}

	return 0;
}

int setTextColor(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	if(! isFunctionOk(L, "setTextColor", 2))
		return 0;

	MObject3d * object;
	lua_Integer id = lua_tointeger(L, 1);

	if((object = getObject3d(id)))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 color;
			script->getFloatArray(1, color, 4);
			((MOText *)object)->setColor(color);
		}
	}

	return 0;
}

int getWindowScale(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MScript * script = (MScript *)engine->getScriptContext();

	unsigned int width, height;
	system->getScreenSize(&width, &height);
	
	float scale[2] = {width, height};
	script->pushFloatArray(scale, 2);
	
	return 1;
}

int getSystemTick(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	lua_pushinteger(L, (lua_Integer)(system->getSystemTick() - g_startTick));
	return 1;
}

int doFile(lua_State * L)
{
	if(! isFunctionOk(L, "doFile", 1))
		return 0;

	const char * filename = lua_tostring(L, 1);

	// save current directory
	char current[256];
	strcpy(current, g_currentDirectory);
	
	// make global filename
	char globalFilename[256];
	getGlobalFilename(globalFilename, g_currentDirectory, filename);

	// read text
	char * text = readTextFile(globalFilename);
	if(! text)
		return 0;
	
	// update current directory
	getRepertory(g_currentDirectory, globalFilename);
	
	// do string
	luaL_dostring(L, text);
	
	// set back to current
	strcpy(g_currentDirectory, current);
	
	SAFE_FREE(text);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MScript::MScript(void):
m_state(NULL),
m_isRunning(false)
{}

MScript::~MScript(void)
{
	clear();
}

void MScript::init(void)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	
	g_startTick = system->getSystemTick();
	
	
	// create context
	m_state = lua_open();
	luaL_openlibs(m_state);

	
	// vec3
	registerVec3(m_state);
	lua_register(m_state, "vec3", vec3);
	lua_register(m_state, "length", length);
	lua_register(m_state, "normalize", normalize);
	lua_register(m_state, "dot", dot);
	lua_register(m_state, "cross", cross);
	
	// object/scene init
	lua_register(m_state, "getScene",	 getScene);
	lua_register(m_state, "getObject",	 getObject);
	lua_register(m_state, "getClone",	 getClone);
	lua_register(m_state, "getParent",	 getParent);
	lua_register(m_state, "getChilds",	 getChilds);
	lua_register(m_state, "getCurrentCamera",    getCurrentCamera);
	
	// object
	lua_register(m_state, "rotate",					rotate);
	lua_register(m_state, "translate",				translate);
	lua_register(m_state, "getPosition",			getPosition);
	lua_register(m_state, "getRotation",			getRotation);
	lua_register(m_state, "getScale",				getScale);
	lua_register(m_state, "setPosition",			setPosition);
	lua_register(m_state, "setRotation",			setRotation);
	lua_register(m_state, "setScale",				setScale);
	lua_register(m_state, "isVisible",				isVisible);
	lua_register(m_state, "activate",				activate);
	lua_register(m_state, "deactivate",				deactivate);
	lua_register(m_state, "isActive",				isActive);
	lua_register(m_state, "getName",				getName);
	lua_register(m_state, "setParent",				setParent);
	
	lua_register(m_state, "enableShadow",			enableShadow);
	lua_register(m_state, "isCastingShadow",		isCastingShadow);
	
	lua_register(m_state, "getTransformedPosition", getTransformedPosition);
	lua_register(m_state, "getTransformedRotation", getTransformedRotation);
	lua_register(m_state, "getTransformedScale",	getTransformedScale);
	lua_register(m_state, "getInverseRotatedVector",getInverseRotatedVector);
	lua_register(m_state, "getRotatedVector",		getRotatedVector);
	lua_register(m_state, "getInverseVector",		getInverseVector);
	lua_register(m_state, "getTransformedVector",	getTransformedVector);
	lua_register(m_state, "updateMatrix",			updateMatrix);
	lua_register(m_state, "getMatrix",				getMatrix);
	
	// behavior
	lua_register(m_state, "getBehaviorVariable",	getBehaviorVariable);
	lua_register(m_state, "setBehaviorVariable",	setBehaviorVariable);

	// animation
	lua_register(m_state, "getCurrentAnimation",	getCurrentAnimation);
	lua_register(m_state, "changeAnimation",		changeAnimation);
	lua_register(m_state, "isAnimationOver",		isAnimationOver);
	lua_register(m_state, "getAnimationSpeed",		getAnimationSpeed);
	lua_register(m_state, "setAnimationSpeed",		setAnimationSpeed);
	lua_register(m_state, "getCurrentFrame",		getCurrentFrame);
	lua_register(m_state, "setCurrentFrame",		setCurrentFrame);

	// physics
	lua_register(m_state, "setGravity",			setGravity);
	lua_register(m_state, "getGravity",			getGravity);
	lua_register(m_state, "addCentralForce",	addCentralForce);
	lua_register(m_state, "addTorque",			addTorque);
	lua_register(m_state, "getLinearDamping",	getLinearDamping);
	lua_register(m_state, "setLinearDamping",	setLinearDamping);
	lua_register(m_state, "getAngularDamping",	getAngularDamping);
	lua_register(m_state, "setAngularDamping",	setAngularDamping);
	lua_register(m_state, "getMass",			getMass);
	lua_register(m_state, "setMass",			setMass);
	lua_register(m_state, "getFriction",		getFriction);
	lua_register(m_state, "setFriction",		setFriction);
	lua_register(m_state, "getRestitution",		getRestitution);
	lua_register(m_state, "setRestitution",		setRestitution);
	lua_register(m_state, "getAngularFactor",	getAngularFactor);
	lua_register(m_state, "setAngularFactor",	setAngularFactor);
	lua_register(m_state, "getLinearFactor",	getLinearFactor);
	lua_register(m_state, "setLinearFactor",	setLinearFactor);
	lua_register(m_state, "getCentralForce",	getCentralForce);
	lua_register(m_state, "getTorque",			getTorque);

	lua_register(m_state, "isCollisionTest",	isCollisionTest);
	lua_register(m_state, "isCollisionBetween", isCollisionBetween);
	lua_register(m_state, "clearForces",		clearForces);
	lua_register(m_state, "getNumCollisions",	getNumCollisions);
	lua_register(m_state, "rayHit",				rayHit);
	
	// input
	lua_register(m_state, "isKeyPressed", isKeyPressed);
	lua_register(m_state, "onKeyDown",	  onKeyDown);
	lua_register(m_state, "onKeyUp",	  onKeyUp);
	lua_register(m_state, "getAxis",	  getAxis);
	lua_register(m_state, "getProperty",  getProperty);
    
    // multitouch
    lua_register(m_state, "getTouchPosition", getTouchPosition);
    lua_register(m_state, "getLastTouchPosition", getLastTouchPosition);
    lua_register(m_state, "getTouchPhase", getTouchPhase);

	// sound
	lua_register(m_state, "playSound",	  playSound);
	lua_register(m_state, "pauseSound",	  pauseSound);
	lua_register(m_state, "stopSound",	  stopSound);
	lua_register(m_state, "getSoundGain", getSoundGain);
	lua_register(m_state, "setSoundGain", setSoundGain);
	
	// scene/level
	lua_register(m_state, "changeScene",			changeScene);
	lua_register(m_state, "getCurrentSceneId",		getCurrentSceneId);
	lua_register(m_state, "getScenesNumber",		getScenesNumber);
	lua_register(m_state, "doesLevelExist",			doesLevelExist);
	lua_register(m_state, "loadLevel",				loadLevel);

	// light
	lua_register(m_state, "getLightColor",	   getLightColor);
	lua_register(m_state, "getLightRadius",	   getLightRadius);
	lua_register(m_state, "getLightIntensity", getLightIntensity);
	lua_register(m_state, "setLightColor",	   setLightColor);
	lua_register(m_state, "setLightRadius",	   setLightRadius);
	lua_register(m_state, "setLightIntensity", setLightIntensity);
	lua_register(m_state, "setLightShadowQuality",	setlightShadowQuality);
	lua_register(m_state, "setLightShadowBias",		setlightShadowBias);
	lua_register(m_state, "setLightShadowBlur",		setlightShadowBlur);
	lua_register(m_state, "setLightSpotAngle",		setlightSpotAngle);
	lua_register(m_state, "setLightSpotExponent",	setlightSpotExponent);	
	lua_register(m_state, "getLightShadowQuality",	getlightShadowQuality);
	lua_register(m_state, "getLightShadowBias",		getlightShadowBias);
	lua_register(m_state, "getLightShadowBlur",		getlightShadowBlur);
	lua_register(m_state, "getLightSpotAngle",		getlightSpotAngle);
	lua_register(m_state, "getLightSpotExponent",	getlightSpotExponent);
	
	// camera
	lua_register(m_state, "changeCurrentCamera",    changeCurrentCamera);
	lua_register(m_state, "getCameraClearColor",    getCameraClearColor);
	lua_register(m_state, "getCameraFov",		    getCameraFov);
	lua_register(m_state, "getCameraNear",		    getCameraNear);
	lua_register(m_state, "getCameraFar",		    getCameraFar);
	lua_register(m_state, "getCameraFogDistance",   getCameraFogDistance);
	lua_register(m_state, "isCameraOrtho",		    isCameraOrtho);
	lua_register(m_state, "isCameraFogEnabled",	    isCameraFogEnabled);
	lua_register(m_state, "setCameraClearColor",    setCameraClearColor);
	lua_register(m_state, "setCameraFov",		    setCameraFov);
	lua_register(m_state, "setCameraNear",	        setCameraNear);
	lua_register(m_state, "setCameraFar",	        setCameraFar);
	lua_register(m_state, "setCameraFogDistance",   setCameraFogDistance);
	lua_register(m_state, "enableCameraOrtho",      enableCameraOrtho);
	lua_register(m_state, "enableCameraFog",	    enableCameraFog);
	lua_register(m_state, "enableCameraLayer",      enableCameraLayer);
	lua_register(m_state, "disableCameraLayer",	    disableCameraLayer);
	lua_register(m_state, "enableRenderToTexture",  enableRenderToTexture);
	lua_register(m_state, "disableRenderToTexture", disableRenderToTexture);
	lua_register(m_state, "getProjectedPoint",		getProjectedPoint);
	lua_register(m_state, "getUnProjectedPoint",	getUnProjectedPoint);

	// text
	lua_register(m_state, "getText", getText);
	lua_register(m_state, "setText", setText);
	lua_register(m_state, "getTextColor", getTextColor);
	lua_register(m_state, "setTextColor", setTextColor);

	// do file
	lua_register(m_state, "dofile", doFile);
	
	// global
	lua_register(m_state, "centerCursor",	centerCursor);
	lua_register(m_state, "hideCursor",		hideCursor);
	lua_register(m_state, "showCursor",		showCursor);
	lua_register(m_state, "getWindowScale", getWindowScale);
	lua_register(m_state, "getSystemTick",	getSystemTick);
	lua_register(m_state, "quit",			quit);
	

	// register custom functions
	map<string, int (*)(void)>::iterator
		mit (m_functions.begin()),
		mend(m_functions.end());

	for(;mit!=mend;++mit)
		lua_register(m_state, mit->first.c_str(), function);
}

void MScript::clear(void)
{
	if(m_state)
	{
		lua_close(m_state);
		m_state = NULL;
	}
	m_isRunning = false;
}

int MScript::function(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);

	map<string, int (*)(void)>::iterator iter = script->m_functions.find(ar.name);
	if(iter != script->m_functions.end())
		return iter->second();

	return 0;
}

void MScript::runScript(const char * filename)
{
	clear();

	if(! filename)
	{
		m_isRunning = false;
		return;
	}

	if(strlen(filename) == 0)
	{
		m_isRunning = false;
		return;
	}

	// current directory
	getRepertory(g_currentDirectory, filename);
	
	// read file
	char * text = readTextFile(filename);
	if(! text)
	{
		printf("ERROR lua script : unable to read file %s\n", filename);
		m_isRunning = false;
		return;
	}
	
	init();
	
	// do string
	if(luaL_dostring(m_state, text) != 0)
	{
		printf("ERROR lua script :\n %s\n", lua_tostring(m_state, -1));
		m_isRunning = false;
		SAFE_FREE(text);
		return;
	}
	
	// finish
	SAFE_FREE(text);
	m_isRunning = true;
}

bool MScript::startCallFunction(const char* name)
{
	if(m_isRunning)
	{
		lua_getglobal(m_state, name);
		if(!lua_isfunction(m_state, -1))
		{
			lua_pop(m_state, 1);
			return false;
		}
		return true;
	}
	return false;
}

bool MScript::endCallFunction(int numArgs)
{
	if(lua_pcall(m_state, numArgs, 0, 0) != 0)
	{
		printf("ERROR lua script :\n %s\n", lua_tostring(m_state, -1));
		m_isRunning = false;
		return false;
	}
	return true;
}

void MScript::callFunction(const char * name)
{
	if(startCallFunction(name))
		endCallFunction();
}

void MScript::addFunction(const char * name, int (*function)(void)){
	m_functions[name] = function;
}

unsigned int MScript::getArgsNumber(void){
	return lua_gettop(m_state);
}

void MScript::getIntArray(unsigned int arg, int * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (int)lua_tointeger(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

void MScript::getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (float)lua_tonumber(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

const char * MScript::getString(unsigned int arg){
	return lua_tostring(m_state, arg+1);
}

int MScript::getInteger(unsigned int arg){
	return (int)lua_tointeger(m_state, arg+1);
}

float MScript::getFloat(unsigned int arg){
	return (float)lua_tonumber(m_state, arg+1);
}

void* MScript::getPointer(unsigned int arg){
	return (void*)lua_tointeger(m_state, arg+1);
}

void MScript::pushIntArray(const int * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushinteger(m_state, (lua_Integer)values[i]);
		lua_rawset(m_state, -3);
	}
}

void MScript::pushFloatArray(const float * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushnumber(m_state, (lua_Number)values[i]);
		lua_rawset(m_state, -3);
	}
}

void MScript::pushBoolean(bool value){
	lua_pushboolean(m_state, (int)value);
}

void MScript::pushString(const char * string){
	lua_pushstring(m_state, string);
}

void MScript::pushInteger(int value){
	lua_pushinteger(m_state, (lua_Integer)value);
}

void MScript::pushFloat(float value){
	lua_pushnumber(m_state, (lua_Number)value);
}

void MScript::pushPointer(void* value){
	lua_pushinteger(m_state, (lua_Integer)value);
}