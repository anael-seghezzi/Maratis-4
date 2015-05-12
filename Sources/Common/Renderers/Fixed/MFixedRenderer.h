/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MFixedRenderer.h
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


#ifndef _M_FIXED_RENDERER_H
#define _M_FIXED_RENDERER_H


// Fixed Renderer
class MFixedRenderer : public MRenderer
{
public:
	
	MFixedRenderer(void);
	~MFixedRenderer(void);
	
private:
	
	void drawDisplay(MOEntity * entity, MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MColor * colors);
	void drawDisplayTriangles(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices);
	void drawOpaques(MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh, MArmature * armature);
	void drawTransparents(MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh, MArmature * armature);
	void updateVisibility(MScene * scene, MOCamera * camera);
	void enableFog(MOCamera * camera);
	
	float getDistanceToCam(MOCamera * camera, const MVector3 & pos);
	
public:
	
	// destroy
	void destroy(void);
	
	// get new
	static MRenderer * getNew(void);

	// name
	static const char * getStaticName(void){ return "FixedRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	// draw
	void drawScene(MScene * scene, MOCamera * camera);
	
	// extra
	void drawText(MOText * textObj);
};

#endif