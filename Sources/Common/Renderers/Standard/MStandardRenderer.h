/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MStandardRenderer.h
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


#ifndef _M_STANDARD_RENDERER_H
#define _M_STANDARD_RENDERER_H


#define MAX_ENTITY_LIGHTS 256
#define MAX_SHADOW_LIGHTS 64
#define MAX_DEFAULT_FXS 16
#define MAX_OPAQUE 4096
#define MAX_TRANSP 2048


// Entity light
struct MEntityLight
{
	MBox3d lightBox;
	MOLight * light;
};


// SubMesh pass
class MSubMeshPass
{
public:
	MSubMeshPass(void):occlusionQuery(0){}
	
	unsigned int occlusionQuery;
	unsigned int subMeshId;
	unsigned int lightsNumber;
	MObject3d * object;
	MOLight * lights[4];
};


// Shadow Light
class MShadowLight
{
public:
	MShadowLight(void):score(0), currentFrame(-1), shadowTexture(0){}
	
	int score;
	int currentFrame;
	unsigned int shadowTexture;
	unsigned int shadowQuality;
	float biasUnity;
	MMatrix4x4 shadowMatrix;
};


// Standard Renderer
class MStandardRenderer : public MRenderer
{
public:
	
	MStandardRenderer(void);
	~MStandardRenderer(void);
	
private:
	
	// globals
	bool m_forceNoFX;
	unsigned int m_fboId;
	int m_lightShadow[4];
	int m_lightShadowTexture[4];
	float m_lightShadowBias[4];
	float m_lightShadowBlur[4];
	
	MMatrix4x4 m_currModelViewMatrix;
	MMatrix4x4 m_lightShadowMatrix[4];
	MOCamera * m_currentCamera;
	
	// shadow lights
	unsigned int m_randTexture;
	map<unsigned long, MShadowLight> m_shadowLights;
	
	// default FXs
	unsigned int m_FXsNumber;
	unsigned int m_vertShaders[MAX_DEFAULT_FXS];
	unsigned int m_fragShaders[MAX_DEFAULT_FXS];
	unsigned int m_FXs[MAX_DEFAULT_FXS];
	
	// lists
	int m_transpSortList[MAX_TRANSP];
	int m_opaqueSortList[MAX_OPAQUE];
	float m_transpSortZList[MAX_TRANSP];
	float m_opaqueSortZList[MAX_OPAQUE];
	MSubMeshPass m_transpList[MAX_TRANSP];	
	MSubMeshPass m_opaqueList[MAX_OPAQUE];
	
	// lights list
	int m_entityLightsList[MAX_ENTITY_LIGHTS];
	float m_entityLightsZList[MAX_ENTITY_LIGHTS];
	MEntityLight m_entityLights[MAX_ENTITY_LIGHTS];
	
private:
	
	void addFX(const char * vert, const char * frag);
	void drawDisplay(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MVector3 * tangents, MColor * colors);
	void drawOpaques(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MArmature * armature);
	void drawTransparents(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MArmature * armature);
	void setShadowMatrix(MMatrix4x4 * matrix, MOCamera * camera);
	void updateVisibility(MScene * scene, MOCamera * camera);
	void enableFog(MOCamera * camera);
	
	void initVBO(MSubMesh * subMesh, MSubMeshCache * subMeshCahe, MVector3 * vertices, MVector3 * normals, MVector3 * tangents);
	
	float getDistanceToCam(MOCamera * camera, const MVector3 & pos);
	
	// shadow lights
	MShadowLight * createShadowLight(MOLight * light);
	void destroyUnusedShadowLights(void);
	void decreaseShadowLights(void);
	
	// subMesh
	void prepareSubMesh(MScene * scene, MOCamera * camera, MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh);
	
public:
	
	// destroy
	void destroy(void);
	
	// get new
	static MRenderer * getNew(void);

	// name
	static const char * getStaticName(void){ return "StandardRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	// draw
	void drawScene(MScene * scene, MOCamera * camera);
	
	// extra
	void drawText(MOText * textObj);
};

#endif