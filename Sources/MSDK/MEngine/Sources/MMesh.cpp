/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMesh.cpp
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


// MDisplay
MDisplay::MDisplay(M_PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size):
m_primitiveType(primitiveType),
m_begin(begin),
m_size(size),
m_cullMode(M_CULL_BACK),
m_materialId(0),
m_visibility(true)
{}

MDisplay::MDisplay(const MDisplay & display):
m_primitiveType(display.m_primitiveType),
m_begin(display.m_begin),
m_size(display.m_size),
m_cullMode(display.m_cullMode),
m_materialId(display.m_materialId),
m_visibility(display.m_visibility)
{}

MDisplay::~MDisplay(void)
{}


// MSubMeshCache
MSubMeshCache::MSubMeshCache(void):
m_verticesSize(0),
m_normalsSize(0),
m_tangentsSize(0),
m_vertices(NULL),
m_normals(NULL),
m_tangents(NULL),
m_vboId(0)
{}

MSubMeshCache::~MSubMeshCache(void)
{
	clearVertices();
	clearNormals();
	clearTangents();
}

void MSubMeshCache::clearVertices(void)
{
	m_verticesSize = 0;
	SAFE_DELETE_ARRAY(m_vertices);
}

void MSubMeshCache::clearNormals(void)
{
	m_normalsSize = 0;
	SAFE_DELETE_ARRAY(m_normals);
}

void MSubMeshCache::clearTangents(void)
{
	m_tangentsSize = 0;
	SAFE_DELETE_ARRAY(m_tangents);
}

MVector3 * MSubMeshCache::allocVertices(unsigned int size)
{
	if(size == m_verticesSize)
		return m_vertices;

	clearVertices();
	if(size == 0)
		return NULL;

	m_verticesSize = size;
	m_vertices = new MVector3[size];

	return m_vertices;
}

MVector3 * MSubMeshCache::allocNormals(unsigned int size)
{
	if(size == m_normalsSize)
		return m_normals;
		
	clearNormals();
	if(size == 0)
		return NULL;

	m_normalsSize = size;
	m_normals = new MVector3[size];

	return m_normals;
}

MVector3 * MSubMeshCache::allocTangents(unsigned int size)
{
	if(size == m_tangentsSize)
		return m_tangents;
		
	clearTangents();
	if(size == 0)
		return NULL;
	
	m_tangentsSize = size;
	m_tangents = new MVector3[size];
	
	return m_tangents;
}

void MSubMeshCache::clearVBO(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	if(render && m_vboId>0)
		render->deleteVBO(&m_vboId);
}


// MSubMesh
MSubMesh::MSubMesh(void):

m_morphingData(NULL),
m_skinData(NULL),

m_indicesSize(0),
m_verticesSize(0),
m_normalsSize(0),
m_tangentsSize(0),
m_texCoordsSize(0),
m_colorsSize(0),

m_indicesType(M_UINT),
m_indices(NULL),

m_colors(NULL),
m_vertices(NULL),
m_normals(NULL),
m_tangents(NULL),
m_texCoords(NULL),

m_vboId1(0),
m_vboId2(0),

m_displaysNumber(0),
m_displays(NULL)
{}

MSubMesh::~MSubMesh(void)
{
	clearVBO();
	
	clearIndices();
	clearVertices();
	clearNormals();
	clearTangents();
	clearTexCoords();
	clearColors();

	clearDisplays();

	clearMorphingData();
	clearSkinData();
}

void MSubMesh::clearMapChannelOffset(void)
{
	m_mapChannelsOffset.clear();
}

void MSubMesh::setMapChannelOffset(unsigned int channel, unsigned int offset)
{
	m_mapChannelsOffset[channel] = offset;
}

bool MSubMesh::isMapChannelExist(unsigned int channel)
{
	map<unsigned int, unsigned int>::iterator iter = m_mapChannelsOffset.find(channel);
	return (iter != m_mapChannelsOffset.end());
}

unsigned int MSubMesh::getMapChannelOffset(unsigned int channel)
{
	map<unsigned int, unsigned int>::iterator iter = m_mapChannelsOffset.find(channel);
	if(iter != m_mapChannelsOffset.end())
		return iter->second;
	return 0;
}

void MSubMesh::clearMorphingData(void)
{
	SAFE_DELETE(m_morphingData);
}

MMorphingData * MSubMesh::createMorphingData(void)
{
	MMorphingData * morphingData = new MMorphingData();
	clearMorphingData();
	m_morphingData = morphingData;
	return morphingData;
}

void MSubMesh::clearSkinData(void)
{
	SAFE_DELETE(m_skinData);
}

MSkinData * MSubMesh::createSkinData(void)
{
	MSkinData * skinData = new MSkinData();
	clearSkinData();
	m_skinData = skinData;
	return skinData;
}

void MSubMesh::clearIndices(void)
{
	m_indicesSize = 0;
	SAFE_FREE(m_indices);
}

void MSubMesh::clearVertices(void)
{
	m_verticesSize = 0;
	SAFE_DELETE_ARRAY(m_vertices);
}

void MSubMesh::clearNormals(void)
{
	m_normalsSize = 0;
	SAFE_DELETE_ARRAY(m_normals);
}

void MSubMesh::clearTangents(void)
{
	m_tangentsSize = 0;
	SAFE_DELETE_ARRAY(m_tangents);
}

void MSubMesh::clearTexCoords(void)
{
	m_texCoordsSize = 0;
	SAFE_DELETE_ARRAY(m_texCoords);
}

void MSubMesh::clearColors(void)
{
	m_colorsSize = 0;
	SAFE_DELETE_ARRAY(m_colors);
}

void * MSubMesh::allocIndices(unsigned int size, M_TYPES type)
{
	clearIndices();
	if(size == 0)
		return NULL;

	m_indicesSize = size;

	switch(type)
	{
	default:
	case M_UINT:
		m_indicesType = M_UINT;
		m_indices = malloc(sizeof(unsigned int)*size);
		break;
	case M_USHORT:
		m_indicesType = M_USHORT;
		m_indices = malloc(sizeof(unsigned short)*size);
		break;
	}
	
	return m_indices;
}

MVector3 * MSubMesh::allocVertices(unsigned int size)
{
	clearVertices();
	if(size == 0)
		return NULL;

	m_verticesSize = size;
	m_vertices = new MVector3[size];

	return m_vertices;
}

MVector3 * MSubMesh::allocNormals(unsigned int size)
{
	clearNormals();
	if(size == 0)
		return NULL;

	m_normalsSize = size;
	m_normals = new MVector3[size];

	return m_normals;
}

MVector3 * MSubMesh::allocTangents(unsigned int size)
{
	clearTangents();
	if(size == 0)
		return NULL;
	
	m_tangentsSize = size;
	m_tangents = new MVector3[size];
	
	return m_tangents;
}

MVector2 * MSubMesh::allocTexCoords(unsigned int size)
{
	clearTexCoords();
	if(size == 0)
		return NULL;

	m_texCoordsSize = size;
	m_texCoords = new MVector2[size];

	return m_texCoords;
}

MColor * MSubMesh::allocColors(unsigned int size)
{
	clearColors();
	if(size == 0)
		return NULL;

	m_colorsSize = size;
	m_colors = new MColor[size];

	return m_colors;
}

void MSubMesh::clearVBO(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	if(render && m_vboId1>0)
	{
		render->deleteVBO(&m_vboId1);
		render->deleteVBO(&m_vboId2);
	}
}

void MSubMesh::allocDisplays(unsigned int size)
{
	clearDisplays();
	if(size == 0)
		return;
	m_displays = (MDisplay **) malloc(sizeof(void *)*size);
}

void MSubMesh::clearDisplays(void)
{
	unsigned int i;
	for(i=0; i<m_displaysNumber; i++)
		SAFE_DELETE(m_displays[i]);

	m_displaysNumber = 0;
	SAFE_FREE(m_displays);
}

MDisplay * MSubMesh::addNewDisplay(M_PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size)
{
	MDisplay * display = new MDisplay(primitiveType, begin, size);
	m_displays[m_displaysNumber] = display;
	m_displaysNumber++;
	return display;
}

void MSubMesh::buildAccelMap(void)
{
	if(m_indicesType != M_USHORT) // limit to small mesh for now
		return;
	
	if(m_accelMap.map == NULL)
	{
		int triSize = m_indicesSize/3;
		if(triSize == 0)
			return;
		
		float vsize = pow((float)triSize*16, 1.0f/3.0f); // cubic root
	
		MVector3 bscale = m_boundingBox.max - m_boundingBox.min;
		float bsum = bscale.x + bscale.y + bscale.z;
	
		m_accelMap.sx = MAX(4, (bscale.x/bsum)*vsize);
		m_accelMap.sy = MAX(4, (bscale.y/bsum)*vsize);
		m_accelMap.sz = MAX(4, (bscale.z/bsum)*vsize);
		
		while(m_accelMap.sx%4 != 0)
			m_accelMap.sx++;
		while(m_accelMap.sy%4 != 0)
			m_accelMap.sy++;
		while(m_accelMap.sz%4 != 0)
			m_accelMap.sz++;
		
		m_accelMap.mapSize = m_accelMap.sx*m_accelMap.sy*m_accelMap.sz;
		m_accelMap.dataSize = m_accelMap.mapSize*16;
	
		m_accelMap.map = new unsigned int[m_accelMap.mapSize];
		m_accelMap.bitMap = new char[m_accelMap.mapSize];
		m_accelMap.data = new unsigned int[m_accelMap.dataSize];
	}
	
	//MSystemContext * system = MEngine::getInstance()->getSystemContext();
	//unsigned long tick = system->getSystemTick();
	
	// triangles
	unsigned short * indices = (unsigned short *)m_indices;
	
	// init
	int dataId = 1;
	memset(m_accelMap.map, 0, m_accelMap.mapSize*sizeof(int));
	memset(m_accelMap.bitMap, 0, m_accelMap.mapSize*sizeof(char));

	m_accelMap.unit = m_boundingBox.max - m_boundingBox.min;
	m_accelMap.unit.x /= (float)(m_accelMap.sx);
	m_accelMap.unit.y /= (float)(m_accelMap.sy);
	m_accelMap.unit.z /= (float)(m_accelMap.sz);
	
	m_accelMap.invUnit.x = 1.0f/m_accelMap.unit.x;
	m_accelMap.invUnit.y = 1.0f/m_accelMap.unit.y;
	m_accelMap.invUnit.z = 1.0f/m_accelMap.unit.z;
	m_accelMap.invUnitN = m_accelMap.invUnit.getNormalized();
	
	MVector3 bunit_h = m_accelMap.unit*0.5f;
	MVector3 bzero = m_boundingBox.min + bunit_h;
	
	int nb=0;
	for(int z=0; z<m_accelMap.sz; z++)
	for(int y=0; y<m_accelMap.sy; y++)
	for(int x=0; x<m_accelMap.sx; x++)
	{
		int curDataId = dataId;
		dataId++;
		
		MVector3 bcenter = bzero + m_accelMap.unit*MVector3(x, y, z);
		
		for(int i=0; i<m_indicesSize; i+=3)
		{
			if(isTriangleInBox(m_vertices[indices[i]], m_vertices[indices[i+1]], m_vertices[indices[i+2]], bcenter, bunit_h))
			{
				m_accelMap.data[dataId] = i;
				dataId++;
			
				// not enough memory ?
				if(dataId == m_accelMap.dataSize)
				{
					printf("not enough memory : %d / %d\n", (m_indicesSize/3), dataId); // TODO : realloc ?
					return;
				}
			}
		}
		
		int id3d = m_accelMap.sx*m_accelMap.sy*z + m_accelMap.sx*y + x;
		
		if(curDataId+1 == dataId)
		{
			dataId--;
		}
		else
		{
			m_accelMap.bitMap[id3d] = 1;
			m_accelMap.map[id3d] = curDataId;
			m_accelMap.data[curDataId] = dataId-curDataId-1;
			nb++;
		}
	}
	
	//unsigned int elapsed = system->getSystemTick() - tick;
	//printf("ok %d = %d ms\n", nb, elapsed);
}


// MMesh
MMesh::MMesh(void):

m_animsRangesNumber(0),
m_animsRanges(NULL),

m_armatureAnimRef(NULL),
m_texturesAnimRef(NULL),
m_materialsAnimRef(NULL),

m_armature(NULL),

m_texturesNumber(0),
m_textures(NULL),

m_materialsNumber(0),
m_materials(NULL),

m_subMeshsNumber(0),
m_subMeshs(NULL)
{}

MMesh::~MMesh(void)
{
	clear();
}

MMesh * MMesh::getNew(void)
{
	return new MMesh();
}

void MMesh::destroy(void)
{
	delete this;
}

void MMesh::clear(void)
{
	clearArmature();
	clearTextures();
	clearMaterials();
	clearSubMeshs();
	clearAnimsRanges();
	
	m_armatureAnimRef = NULL;
	m_texturesAnimRef = NULL;
	m_materialsAnimRef = NULL;
}

void MMesh::clearAnimsRanges(void)
{
	m_animsRangesNumber = 0;
	SAFE_DELETE_ARRAY(m_animsRanges);
}

MAnimRange * MMesh::allocAnimsRanges(unsigned int size)
{
	clearAnimsRanges();
	if(size == 0)
		return NULL;

	m_animsRangesNumber = size;
	m_animsRanges = new MAnimRange[m_animsRangesNumber];
	return m_animsRanges;
}

MArmatureAnim * MMesh::getArmatureAnim(void)
{
	if(! m_armatureAnimRef)
		return NULL;

	return m_armatureAnimRef->getArmatureAnim();
}

MTexturesAnim * MMesh::getTexturesAnim(void)
{
	if(! m_texturesAnimRef)
		return NULL;

	return m_texturesAnimRef->getTexturesAnim();
}

MMaterialsAnim * MMesh::getMaterialsAnim(void)
{
	if(! m_materialsAnimRef)
		return NULL;

	return m_materialsAnimRef->getMaterialsAnim();
}

void MMesh::clearArmature(void)
{
	SAFE_DELETE(m_armature);
}

MArmature * MMesh::createArmature(void)
{
	clearArmature();
	m_armature = new MArmature();
	return m_armature;
}

void MMesh::allocTextures(unsigned int size)
{
	clearTextures();
	if(size == 0)
		return;
	m_textures = (MTexture **) malloc(sizeof(void *)*size);
}

void MMesh::clearTextures(void)
{
	unsigned int i;
	for(i=0; i<m_texturesNumber; i++)
		SAFE_DELETE(m_textures[i]);

	m_texturesNumber = 0;
	SAFE_FREE(m_textures);
}

MTexture * MMesh::addNewTexture(MTextureRef * texRef)
{
	MTexture * texture = new MTexture(texRef);
	m_textures[m_texturesNumber] = texture;
	m_texturesNumber++;
	return texture;
}

void MMesh::allocMaterials(unsigned int size)
{
	clearMaterials();
	if(size == 0)
		return;
	m_materials = (MMaterial **) malloc(sizeof(void *)*size);
}

void MMesh::clearMaterials(void)
{
	unsigned int i;
	for(i=0; i<m_materialsNumber; i++)
		SAFE_DELETE(m_materials[i]);

	m_materialsNumber = 0;
	SAFE_FREE(m_materials);
}

MMaterial * MMesh::addNewMaterial(void)
{
	MMaterial * material = new MMaterial();
	m_materials[m_materialsNumber] = material;
	m_materialsNumber++;
	return material;
}

MSubMesh * MMesh::allocSubMeshs(unsigned int size)
{
	clearSubMeshs();
	if(size == 0)
		return NULL;

	m_subMeshsNumber = size;
	m_subMeshs = new MSubMesh[size];
	return m_subMeshs;
}

void MMesh::clearSubMeshs(void)
{
	m_subMeshsNumber = 0;
	SAFE_DELETE_ARRAY(m_subMeshs);
}

void MMesh::updateBoundingBox(void)
{
	if(m_subMeshsNumber > 0)
	{
		MVector3 * min = &m_boundingBox.min;
		MVector3 * max = &m_boundingBox.max;

		MBox3d * box = m_subMeshs[0].getBoundingBox();
		(*min) = box->min;
		(*max) = box->max;

		unsigned int i;
		for(i=1; i<m_subMeshsNumber; i++)
		{
			box = m_subMeshs[i].getBoundingBox();

			MVector3 * subMin = &box->min;
			MVector3 * subMax = &box->max;

			min->x = MIN(min->x, subMin->x);
			min->y = MIN(min->y, subMin->y);
			min->z = MIN(min->z, subMin->z);

			max->x = MAX(max->x, subMax->x);
			max->y = MAX(max->y, subMax->y);
			max->z = MAX(max->z, subMax->z);
		}
	}
}

void MMesh::updateArmature(float currentFrame)
{
	if(! m_armature)
		return;
		
	MArmatureAnim * armatureAnim = getArmatureAnim();
	if(armatureAnim)
	{
		animateArmature(m_armature, armatureAnim, currentFrame);
	}
	else
	{
		m_armature->processBonesLinking();
		m_armature->updateBonesSkinMatrix();
	}
}