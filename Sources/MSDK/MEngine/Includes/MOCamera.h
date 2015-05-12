/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOCamera.h
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


#ifndef _M_OCAMERA_H
#define _M_OCAMERA_H


class M_ENGINE_EXPORT MOCamera : public MObject3d
{
private:

	// properties
	bool m_fog;
	bool m_ortho;
	float m_fov;
	float m_fogDistance;
	float m_clippingNear;
	float m_clippingFar;
	MVector3 m_clearColor;

	// current matrices
	MMatrix4x4 m_currentViewMatrix; // current model view matrix
	MMatrix4x4 m_currentProjMatrix; // current projection matrix

	// current viewport
	int m_currentViewport[4]; // 0=x 1=y 2=width 3=height

	// frustum (camera volume for elimination)
	MFrustum m_frustum;
	
	// scene layer (used for GUI etc)
	unsigned int m_sceneLayer;

	// render to texture
	MTextureRef * m_renderColorTexture;
	MTextureRef * m_renderDepthTexture;
	
public:

	// constructor
	MOCamera(void);

	// destructor
	~MOCamera(void);

	// copy constructor
	MOCamera(const MOCamera & camera);

public:

	// type
	int getType(void){ return M_OBJECT3D_CAMERA; }

	// properties
	inline void enableFog(const bool fog){ m_fog = fog; }
	inline void enableOrtho(const bool ortho){ m_ortho = ortho; }
	inline void setFov(const float fov){ m_fov = fov; }
	inline void setFogDistance(const float fogDistance){ m_fogDistance = fogDistance; }
	inline void setClippingNear(const float clippingNear){ m_clippingNear = clippingNear; }
	inline void setClippingFar(const float clippingFar){ m_clippingFar = clippingFar; }
	inline bool isOrtho(void){ return m_ortho; }
	inline bool hasFog(void){ return m_fog; }
	inline float getClippingNear(void){ return m_clippingNear; }
	inline float getClippingFar(void){ return m_clippingFar; }
	inline float getFov(void){ return m_fov; }
	inline float getFogDistance(void){ return m_fogDistance; }
	inline void setClearColor(MVector3 clearColor) { m_clearColor = clearColor; }
	inline MVector3 getClearColor(void) const { return m_clearColor; }

	// projection
	MVector3 getProjectedPoint(const MVector3 & point) const;
	MVector3 getUnProjectedPoint(const MVector3 & point) const;

	// matrices
	void updateViewMatrix(void);
	void updateProjMatrix(void);
	void enableViewProjMatrix(void);
	inline MMatrix4x4 * getCurrentViewMatrix(void){ return &m_currentViewMatrix; }
	inline MMatrix4x4 * getCurrentProjMatrix(void){ return &m_currentProjMatrix; }

	// viewport
	int * getCurrentViewport(void){ return m_currentViewport; }

	// frustum
	inline MFrustum * getFrustum(void){ return &m_frustum; }

	// scene layer
	inline void setSceneLayer(unsigned int sceneLayer){ m_sceneLayer = sceneLayer; }
	inline unsigned int getSceneLayer(void){ return m_sceneLayer; }
	
	// render to texture
	inline void setRenderColorTexture(MTextureRef * renderColorTexture){ m_renderColorTexture = renderColorTexture; }
	inline void setRenderDepthTexture(MTextureRef * renderDepthTexture){ m_renderDepthTexture = renderDepthTexture; }
	inline MTextureRef * getRenderColorTexture(void){ return m_renderColorTexture; }
	inline MTextureRef * getRenderDepthTexture(void){ return m_renderDepthTexture; }
	
	// listener
	void updateListener(void);

	// enable
	void enable(void);
	
	// bounding volumes
	MBox3d createMatrixOrientedBoundingBox(MMatrix4x4 * matrix);
};

#endif