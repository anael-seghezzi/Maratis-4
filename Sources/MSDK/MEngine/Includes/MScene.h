/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MScene.h
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


#ifndef _M_SCENE_H
#define _M_SCENE_H


// data modes
enum M_DATA_MODES
{
	M_DATA_STATIC = 0,
	M_DATA_DYNAMIC,
	M_DATA_STREAM
};

// MScene
class M_ENGINE_EXPORT MScene
{
public:

	// constructor / destructor
	MScene(void);
	~MScene(void);

private:

	// name
	MString m_name;

	// data mode
	M_DATA_MODES m_dataMode;

	// script
	MString m_scriptFilename;

	// objects pointers
	vector <MObject3d *> m_objects;

	// objects
	vector <MOCamera *> m_cameras;
	vector <MOLight *> m_lights;
	vector <MOEntity *> m_entities;
	vector <MOSound *> m_sounds;
	vector <MOText *> m_texts;

	// current frame
	int m_currentFrame;

	// current camera
	unsigned int m_currentCamera;

	// gravity
	MVector3 m_gravity;

public:

	// name
	void setName(const char * name);
	inline const char * getName(void){ return m_name.getSafeString(); }

	// data mode
	inline void setDataMode(M_DATA_MODES dataMode){ m_dataMode = dataMode; }
	inline M_DATA_MODES getDataMode(void){ return m_dataMode; }

	// script
	void setScriptFilename(const char * scriptFilename);
	inline const char * getScriptFilename(void){ return m_scriptFilename.getData(); }

	// gravity
	inline void setGravity(const MVector3 & gravity){ m_gravity = gravity; }
	inline MVector3 getGravity(void) const { return m_gravity; }

	// sounds
	void playLoopSounds(void);
	void stopAllSounds(void);

	// object name
	MString getNewObjectName(const char * baseName);
	
	// delete object
	void deleteObject(MObject3d * object);

	// add objects
	MOCamera * addNewCamera(void);
	MOCamera * addNewCamera(const MOCamera & camera);
	MOLight * addNewLight(void);
	MOLight * addNewLight(const MOLight & light);
	MOEntity * addNewEntity(MMeshRef * meshRef);
	MOEntity * addNewEntity(const MOEntity & entity);
	MOSound * addNewSound(MSoundRef * soundRef);
	MOSound * addNewSound(const MOSound & sound);
	MOText * addNewText(MFontRef * fontRef);
	MOText * addNewText(const MOText & text);

	// get objects number
	inline unsigned int getObjectsNumber(void){ return m_objects.size(); }
	inline unsigned int getCamerasNumber(void){ return m_cameras.size(); }
	inline unsigned int getLightsNumber(void){ return m_lights.size(); }
	inline unsigned int getEntitiesNumber(void){ return m_entities.size(); }
	inline unsigned int getSoundsNumber(void){ return m_sounds.size(); }
	inline unsigned int getTextsNumber(void){ return m_texts.size(); }

	// current camera
	inline void setCurrentCameraId(unsigned int id){ m_currentCamera = id; }
	inline unsigned int getCurrentCameraId(void){ return m_currentCamera; }
	MOCamera * getCurrentCamera(void);

	// raytracing
	MOEntity * getRayNearestEntityIntersection(const MVector3 & origin, const MVector3 & direction, float * distance);
	
	// current frame
	inline void setCurrentFrame(int currentFrame){ m_currentFrame = currentFrame; }
	inline int getCurrentFrame(void){ return m_currentFrame; }

	// get objects
	MObject3d * getObjectByName(const char * name);
	MOLight * getLightByName(const char * name);
	MOCamera * getCameraByName(const char * name);
	MOEntity * getEntityByName(const char * name);
	MOSound * getSoundByName(const char * name);
	MOText * getTextByName(const char * name);
	bool getObjectIndex(const char * name, unsigned int * id);

	inline MObject3d * getObjectByIndex(unsigned int index){ return m_objects[index]; }
	inline MOLight * getLightByIndex(unsigned int index){ return m_lights[index]; }
	inline MOCamera * getCameraByIndex(unsigned int index){ return m_cameras[index]; }
	inline MOEntity * getEntityByIndex(unsigned int index){ return m_entities[index]; }
	inline MOSound * getSoundByIndex(unsigned int index){ return m_sounds[index]; }
	inline MOText * getTextByIndex(unsigned int index){ return m_texts[index]; }

	// begin / end
	void begin(void);
	void end(void);

	// physics
	void prepareCollisionShape(MOEntity * entity);
	void prepareCollisionObject(MOEntity * entity);
	void prepareConstraints(MOEntity * entity);
	void preparePhysics(void);
	void updatePhysics(void);

	// update
	void update(void);

	// behaviors
	void updateObjectsBehaviors(void);
	void drawObjectsBehaviors(void);
	
	// update objects
	void updateObjectsMatrices(void);

	// draw
	void draw(MOCamera * camera);
};


#endif