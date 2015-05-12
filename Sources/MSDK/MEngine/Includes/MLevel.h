/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MLevel.h
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


#ifndef _M_LEVEL_H
#define _M_LEVEL_H


// level
class M_ENGINE_EXPORT MLevel
{
private:

	// FX manager
	MFXManager m_FXManager;

	// data managers
	MDataManager m_fontManager;
	MDataManager m_textureManager;
	MDataManager m_shaderManager;
	MDataManager m_soundManager;
	MDataManager m_meshManager;
	MDataManager m_armatureAnimManager;
	MDataManager m_texturesAnimManager;
	MDataManager m_materialsAnimManager;

	// data queues
	vector <MDataRef *> m_updateQueue;
	vector <MDataRef *> m_clearQueue;

	// scenes
	unsigned int		m_currentSceneId;
	unsigned int		m_requestedSceneId;
	vector <MScene *>	m_scenes;

public:

	// constructors / destructors
	 MLevel(void);
	 ~MLevel(void);

private:

	friend class MEngine;
	void changeCurrentSceneIfRequested();

public:

	// clear
	void clear(void);
	void clearScenes(void);

	// FX manager
	inline MFXManager * getFXManager(void){ return &m_FXManager; }
	MFXRef * createFX(MShaderRef * vertexShaderRef, MShaderRef * pixelShaderRef);

	// data managers
	inline MDataManager * getFontManager(void){ return &m_fontManager; }
	inline MDataManager * getTextureManager(void){ return &m_textureManager; }
	inline MDataManager * getShaderManager(void){ return &m_shaderManager; }
	inline MDataManager * getSoundManager(void){ return &m_soundManager; }
	inline MDataManager * getMeshManager(void){ return &m_meshManager; }
	inline MDataManager * getArmatureAnimManager(void){ return &m_armatureAnimManager; }
	inline MDataManager * getTexturesAnimManager(void){ return &m_texturesAnimManager; }
	inline MDataManager * getMaterialsAnimManager(void){ return &m_materialsAnimManager; }

	MFontRef * loadFont(const char * filename, const bool preload = true);
	MMeshRef * loadMesh(const char * filename, const bool preload = true);
	MArmatureAnimRef * loadArmatureAnim(const char * filename, const bool preload = true);
	MTexturesAnimRef * loadTexturesAnim(const char * filename, const bool preload = true);
	MMaterialsAnimRef * loadMaterialsAnim(const char * filename, const bool preload = true);
	MTextureRef * loadTexture(const char * filename, const bool mipmap = true, const bool preload = true);
	MSoundRef * loadSound(const char * filename, const bool preload = true);
	MShaderRef * loadShader(const char * filename, M_SHADER_TYPES type, const bool preload = true);

	// queue data updates
	void incrDataRefScore(MDataRef * ref);
	void decrDataRefScore(MDataRef * ref);
	void sendToUpdateQueue(MDataRef * ref);
	void sendToClearQueue(MDataRef * ref);
	void updateQueueDatas(void);
	void clearQueueDatas(void);

	// scenes
	void changeCurrentScene(unsigned int id); // does not change scene straight away, instead it requests to do it
	void deleteScene(unsigned int id);
	bool getSceneIndexByName(const char * name, unsigned int * index);
	MScene * addNewScene(void);
	MScene * getSceneByName(const char * name);
	MScene * getCurrentScene(void);
	void setCurrentSceneId(unsigned int id);
	inline unsigned int getCurrentSceneId(void){ return m_currentSceneId; }
	inline unsigned int getScenesNumber(void){ return m_scenes.size(); }
	inline MScene * getSceneByIndex(unsigned int id){ return m_scenes[id]; }
};

#endif