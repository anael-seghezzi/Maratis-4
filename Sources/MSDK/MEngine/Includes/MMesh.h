/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMesh.h
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


#ifndef _M_MESH_H
#define _M_MESH_H

#include "MMorphingData.h"
#include "MSkinData.h"


// Display
class M_ENGINE_EXPORT MDisplay
{
public:

	MDisplay(M_PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size);
	MDisplay(const MDisplay & display);
	~MDisplay(void);

private:

	// draw array infos
	M_PRIMITIVE_TYPES m_primitiveType;
	unsigned int m_begin;
	unsigned int m_size;

	// cull mode
	M_CULL_MODES m_cullMode;

	// material
	unsigned int m_materialId;

	// visibility
	bool m_visibility;

public:

	// array infos
	inline M_PRIMITIVE_TYPES getPrimitiveType(void){ return m_primitiveType; }
	inline unsigned int getBegin(void){ return m_begin; }
	inline unsigned int getSize(void){ return m_size; }

	// cull mode
	inline void setCullMode(M_CULL_MODES cullMode){ m_cullMode = cullMode; }
	inline M_CULL_MODES getCullMode(void){ return m_cullMode; }

	// material
	inline void setMaterialId(unsigned int materialId){ m_materialId = materialId; }
	inline unsigned int getMaterialId(void){ return m_materialId; }
	
	// visibility
	inline void setVisibility(bool visibility){ m_visibility = visibility; }
	inline bool isVisible(void){ return m_visibility; }
};


// SubMeshCache
class M_ENGINE_EXPORT MSubMeshCache
{
public:

	MSubMeshCache(void);
	~MSubMeshCache(void);

private:

	// data
	unsigned int m_verticesSize;
	unsigned int m_normalsSize;
	unsigned int m_tangentsSize;

	MVector3 * m_vertices;
	MVector3 * m_normals;
	MVector3 * m_tangents;

	// bounding box
	MBox3d m_boundingBox;

	// VBO (optional)
	unsigned int m_vboId;
	
public:

	// data
	void clearVertices(void);
	void clearNormals(void);
	void clearTangents(void);

	MVector3 * allocVertices(unsigned int size);
	MVector3 * allocNormals(unsigned int size);
	MVector3 * allocTangents(unsigned int size);

	inline unsigned int getVerticesSize(void){ return m_verticesSize; }
	inline unsigned int getNormalsSize(void){ return m_normalsSize; }
	inline unsigned int getTangentsSize(void){ return m_tangentsSize; }

	inline MVector3 * getVertices(void){ return m_vertices; }
	inline MVector3 * getNormals(void){ return m_normals; }
	inline MVector3 * getTangents(void){ return m_tangents; }

	// bounding box
	inline MBox3d * getBoundingBox(void){ return &m_boundingBox; }
	
	// VBO
	void clearVBO(void);
	unsigned int * getVBOid(void){ return &m_vboId; }
};


// TEST: accel map (voxel)
class M_ENGINE_EXPORT MAccelMap
{
public:

	MAccelMap(void):data(NULL), map(NULL), bitMap(NULL), sx(0), sy(0), sz(0){}
	~MAccelMap(void){ SAFE_DELETE_ARRAY(data); SAFE_DELETE_ARRAY(map); SAFE_DELETE_ARRAY(bitMap); }
	
	unsigned int * data;
	unsigned int * map;
	char * bitMap;
	unsigned int sx;
	unsigned int sy;
	unsigned int sz;
	unsigned int mapSize;
	unsigned int dataSize;
	MVector3 unit;
	MVector3 invUnit;
	MVector3 invUnitN;
};


// SubMesh
class M_ENGINE_EXPORT MSubMesh
{
public:

	MSubMesh(void);
	~MSubMesh(void);

private:

	// morphing data
	MMorphingData * m_morphingData;

	// skin data
	MSkinData * m_skinData;

	// data
	unsigned int m_indicesSize;
	unsigned int m_verticesSize;
	unsigned int m_normalsSize;
	unsigned int m_tangentsSize;
	unsigned int m_texCoordsSize;
	unsigned int m_colorsSize;

	M_TYPES m_indicesType;
	void * m_indices;

	MColor * m_colors;
	MVector3 * m_vertices;
	MVector3 * m_normals;
	MVector3 * m_tangents;
	MVector2 * m_texCoords;

	// VBO (optional)
	unsigned int m_vboId1;
	unsigned int m_vboId2; // for indices
	
	// mapChannels offset (map of textureCoords offsets used for channel mapping)
	map<unsigned int, unsigned int> m_mapChannelsOffset;
	
	// displays
	unsigned int m_displaysNumber;
	MDisplay ** m_displays;

	// bounding box
	MBox3d m_boundingBox;
	
	// acceleration structure TEST !!
	MAccelMap m_accelMap;

public:

	// morphing data
	void clearMorphingData(void);
	MMorphingData * createMorphingData(void);
	inline MMorphingData * getMorphingData(void){ return m_morphingData; }

	// skin data
	void clearSkinData(void);
	MSkinData * createSkinData(void);
	inline MSkinData * getSkinData(void){ return m_skinData; }

	// data
	void clearIndices(void);
	void clearVertices(void);
	void clearNormals(void);
	void clearTangents(void);
	void clearTexCoords(void);
	void clearColors(void);

	void * allocIndices(unsigned int size, M_TYPES type);
	MColor * allocColors(unsigned int size);
	MVector3 * allocVertices(unsigned int size);
	MVector3 * allocNormals(unsigned int size);
	MVector3 * allocTangents(unsigned int size);
	MVector2 * allocTexCoords(unsigned int size);

	inline unsigned int getIndicesSize(void){ return m_indicesSize; }
	inline unsigned int getVerticesSize(void){ return m_verticesSize; }
	inline unsigned int getNormalsSize(void){ return m_normalsSize; }
	inline unsigned int getTangentsSize(void){ return m_tangentsSize; }
	inline unsigned int getTexCoordsSize(void){ return m_texCoordsSize; }
	inline unsigned int getColorsSize(void){ return m_colorsSize; }

	M_TYPES getIndicesType(void){ return m_indicesType; }
	inline void	* getIndices(void){ return m_indices; }

	inline MColor * getColors(void){ return m_colors; }
	inline MVector3 * getVertices(void){ return m_vertices; }
	inline MVector3 * getNormals(void){ return m_normals; }
	inline MVector3 * getTangents(void){ return m_tangents; }
	inline MVector2 * getTexCoords(void){ return m_texCoords; }

	// VBO
	void clearVBO1(void);
	void clearVBO2(void);
	void clearVBO(void);
	unsigned int * getVBOid1(void){ return &m_vboId1; }
	unsigned int * getVBOid2(void){ return &m_vboId2; }
	
	// mapChannels offset
	void clearMapChannelOffset(void);
	void setMapChannelOffset(unsigned int channel, unsigned int offset);
	bool isMapChannelExist(unsigned int channel);
	unsigned int getMapChannelOffset(unsigned int channel);
	map<unsigned int, unsigned int> * getMapChannelOffsets(void){ return &m_mapChannelsOffset; }
	
	// displays
	void allocDisplays(unsigned int size);
	void clearDisplays(void);
	MDisplay * addNewDisplay(M_PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size);
	inline unsigned int getDisplaysNumber(void){ return m_displaysNumber; }
	inline MDisplay * getDisplay(unsigned int id){ return m_displays[id]; }

	// bounding box
	inline MBox3d * getBoundingBox(void){ return &m_boundingBox; }
	
	// acceleration structure TEST !!
	void buildAccelMap(void);
	inline MAccelMap * getAccelMap(void){ return &m_accelMap; }
};


// MAnimRange
class M_ENGINE_EXPORT MAnimRange : public MRange
{
public:
	MAnimRange(void):
	  loops(-1)
	{}

	int loops;
};


// Mesh
class M_ENGINE_EXPORT MMesh
{
public:

	MMesh(void);
	~MMesh(void);

	static MMesh * getNew(void);
	void destroy(void);

private:

	// anims ranges
	unsigned int m_animsRangesNumber;
	MAnimRange * m_animsRanges;

	// armature anim
	MArmatureAnimRef * m_armatureAnimRef;

	// textures anim
	MTexturesAnimRef * m_texturesAnimRef;

	// materials anim
	MMaterialsAnimRef * m_materialsAnimRef;

	// armature
	MArmature * m_armature;

	// textures
	unsigned int m_texturesNumber;
	MTexture ** m_textures;

	// materials
	unsigned int m_materialsNumber;
	MMaterial ** m_materials;

	// subMeshs
	unsigned int m_subMeshsNumber;
	MSubMesh * m_subMeshs;

	// bounding box
	MBox3d m_boundingBox;

public:

	// clear
	void clear(void);

	// anims ranges
	void clearAnimsRanges(void);
	MAnimRange * allocAnimsRanges(unsigned int size);
	inline unsigned int getAnimsRangesNumber(void){ return m_animsRangesNumber; }
	inline MAnimRange * getAnimsRanges(void){ return m_animsRanges; }

	// armature anim
	inline void setArmatureAnimRef(MArmatureAnimRef * armatureAnimRef){ m_armatureAnimRef = armatureAnimRef; }
	inline MArmatureAnimRef * getArmatureAnimRef(void){ return m_armatureAnimRef; }
	MArmatureAnim * getArmatureAnim(void);

	// textures anim
	inline void setTexturesAnimRef(MTexturesAnimRef * texturesAnimRef){ m_texturesAnimRef = texturesAnimRef; }
	inline MTexturesAnimRef * getTexturesAnimRef(void){ return m_texturesAnimRef; }
	MTexturesAnim * getTexturesAnim(void);

	// materials anim
	inline void setMaterialsAnimRef(MMaterialsAnimRef * materialsAnimRef){ m_materialsAnimRef = materialsAnimRef; }
	inline MMaterialsAnimRef * getMaterialsAnimRef(void){ return m_materialsAnimRef; }
	MMaterialsAnim * getMaterialsAnim(void);

	// armature
	void clearArmature(void);
	void updateArmature(float currentFrame);
	MArmature * createArmature(void);
	inline MArmature * getArmature(void){ return m_armature; }

	// textures
	void allocTextures(unsigned int size);
	void clearTextures(void);
	MTexture * addNewTexture(MTextureRef * texRef);
	inline unsigned int getTexturesNumber(void){ return m_texturesNumber; }
	inline MTexture * getTexture(unsigned int id){ return m_textures[id]; }

	// materials
	void allocMaterials(unsigned int size);
	void clearMaterials(void);
	MMaterial * addNewMaterial(void);
	inline unsigned int getMaterialsNumber(void){ return m_materialsNumber; }
	inline MMaterial * getMaterial(unsigned int id){ return m_materials[id]; }

	// subMeshs
	void clearSubMeshs(void);
	MSubMesh * allocSubMeshs(unsigned int size);
	inline unsigned int getSubMeshsNumber(void){ return m_subMeshsNumber; }
	inline MSubMesh * getSubMeshs(void){ return m_subMeshs; }

	// bounding box
	void updateBoundingBox(void);
	inline MBox3d * getBoundingBox(void){ return &m_boundingBox; }
};

#endif