/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MEngine.h
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


#ifndef _M_ENGINE_H
#define _M_ENGINE_H

#ifdef WIN32

	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
		#pragma warning(disable: 4100)
		#pragma warning(disable: 4244)
		#pragma warning(disable: 4267)
		#pragma warning(disable: 4083)
	#endif

	// M_ENGINE_EXPORT
	#if defined(MENGINE_DLL)
		#define M_ENGINE_EXPORT __declspec( dllexport )
	#elif defined(MENGINE_STATIC)
		#define M_ENGINE_EXPORT
	#else
		#define M_ENGINE_EXPORT __declspec( dllimport )
	#endif

#else

	// M_ENGINE_EXPORT
	#define M_ENGINE_EXPORT

#endif


// object3d type
#define M_OBJECT3D			0
#define M_OBJECT3D_BONE		1
#define M_OBJECT3D_CAMERA	2
#define M_OBJECT3D_LIGHT	3
#define M_OBJECT3D_ENTITY	4
#define M_OBJECT3D_SOUND	5
#define M_OBJECT3D_TEXT		6

// dataRef type
#define M_REF_MESH				0
#define M_REF_TEXTURE			1
#define M_REF_SOUND				2
#define M_REF_ARMATURE_ANIM		3
#define M_REF_MATERIALS_ANIM	4
#define M_REF_TEXTURES_ANIM		5
#define M_REF_SHADER			6
#define M_REF_FONT				7

// classes
class MScene;
class MLevel;
class MGame;

class MObject3d;
class MOBone;
class MOLight;
class MOCamera;
class MOEntity;
class MOSound;
class MOText;

class MFontRef;
class MShaderRef;
class MSoundRef;
class MMeshRef;
class MTextureRef;
class MArmatureAnimRef;
class MTexturesAnimRef;
class MMaterialsAnimRef;


#include <MCore.h>

#include "MLog.h"

#include "MPackageManager.h"

#include "MKey.h"
#include "MObject3dAnim.h"
#include "MTextureAnim.h"
#include "MMaterialAnim.h"

#include "MArmatureAnim.h"
#include "MTexturesAnim.h"
#include "MMaterialsAnim.h"

#include "MFXManager.h"

#include "MVariable.h"
#include "MBehavior.h"
#include "MBehaviorCreator.h"
#include "MBehaviorManager.h"

#include "MRenderer.h"
#include "MRendererCreator.h"
#include "MRendererManager.h"

#include "MBox3d.h"
#include "MFrustum.h"

#include "MFont.h"
#include "MTexture.h"
#include "MMaterial.h"
#include "MArmature.h"
#include "MMesh.h"
#include "MMeshTools.h"

#include "MFontRef.h"
#include "MShaderRef.h"
#include "MSoundRef.h"
#include "MMeshRef.h"
#include "MTextureRef.h"
#include "MArmatureAnimRef.h"
#include "MTexturesAnimRef.h"
#include "MMaterialsAnimRef.h"

#include "MObject3d.h"
#include "MOBone.h"
#include "MOCamera.h"
#include "MOLight.h"
#include "MOSound.h"
#include "MOEntity.h"
#include "MOText.h"

#include "MScene.h"
#include "MLevel.h"
#include "MGame.h"


class M_ENGINE_EXPORT MEngine
{
public:

	MEngine(void);
	~MEngine(void);

	// instance
	static MEngine * getInstance(void);

private:
	
	// active
	bool m_isActive;
	
	// contexts
	MSoundContext * m_soundContext;
	MRenderingContext * m_renderingContext;
	MPhysicsContext * m_physicsContext;
	MScriptContext * m_scriptContext;
	MInputContext * m_inputContext;
	MSystemContext * m_systemContext;

	// data loaders
	MDataLoader m_fontLoader;
	MDataLoader m_imageLoader;
	MDataLoader m_soundLoader;
	MDataLoader m_meshLoader;
	MDataLoader m_armatureAnimLoader;
	MDataLoader m_texturesAnimLoader;
	MDataLoader m_materialsAnimLoader;
	MDataLoader m_levelLoader;

	// behavior manager
	MBehaviorManager m_behaviorManager;
	
	// renderer manager
	MRendererManager m_rendererManager;

	// package manager
	MPackageManager * m_packageManager;
	
	// level
	MLevel * m_level;

	// game
	MGame * m_game;
	
	// renderer
	MRenderer * m_renderer;

	// stores the name of the level to load when loading is save
	char * m_requestedLevelToLoad;
	
private:
	
	void loadLevelIfRequested(); // Checks if a level was requested to be loaded and loads it
	
public:

	// active
	inline void setActive(bool active){ m_isActive = active; }
	inline bool isActive(void){ return m_isActive; }

	// contexts
	void setSoundContext(MSoundContext * soundContext);
	void setRenderingContext(MRenderingContext * renderingContext);
	void setPhysicsContext(MPhysicsContext * physicsContext);
	void setScriptContext(MScriptContext * scriptContext);
	void setInputContext(MInputContext * inputContext);
	void setSystemContext(MSystemContext * systemContext);

	// get contexts
	inline MSoundContext * getSoundContext(void){ return m_soundContext; }
	inline MRenderingContext * getRenderingContext(void){ return m_renderingContext; }
	inline MPhysicsContext * getPhysicsContext(void){ return m_physicsContext; }
	inline MScriptContext * getScriptContext(void){ return m_scriptContext; }
	inline MInputContext * getInputContext(void){ return m_inputContext; }
	inline MSystemContext * getSystemContext(void){ return m_systemContext; }

	// data loaders
	MDataLoader * getFontLoader(void){ return &m_fontLoader; }
	MDataLoader * getImageLoader(void){ return &m_imageLoader; }
	MDataLoader * getSoundLoader(void){ return &m_soundLoader; }
	MDataLoader * getMeshLoader(void){ return &m_meshLoader; }
	MDataLoader * getArmatureAnimLoader(void){ return &m_armatureAnimLoader; }
	MDataLoader * getTexturesAnimLoader(void){ return &m_texturesAnimLoader; }
	MDataLoader * getMaterialsAnimLoader(void){ return &m_materialsAnimLoader; }
	MDataLoader * getLevelLoader(void){ return &m_levelLoader; }

	// behavior manager
	inline MBehaviorManager * getBehaviorManager(void){ return &m_behaviorManager; }

	// renderer manager
	inline MRendererManager * getRendererManager(void){ return &m_rendererManager; }

	// package manager
	void setPackageManager(MPackageManager * packageManager);
	inline MPackageManager * getPackageManager(void){ return m_packageManager; }

	// update requests
	void updateRequests(void);

	// level
	void setLevel(MLevel * level);
	bool loadLevel(const char * filename); // loads level immediately
	void requestLoadLevel(const char * filename); // send a request to load a level
	inline MLevel * getLevel(void){ return m_level; }

	// game
	void setGame(MGame * game);
	inline MGame * getGame(void){ return m_game; }
	
	// renderer
	void setRenderer(MRenderer * renderer);
	inline MRenderer * getRenderer(void){ return m_renderer; }
};


#endif
