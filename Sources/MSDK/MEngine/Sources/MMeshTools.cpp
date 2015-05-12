/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMeshTools.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
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


// blend matrices optim
#ifdef __SSE2__
#include <mmintrin.h>
#include <xmmintrin.h>
static void blendMatrices(MMatrix4x4 * matrix, const MMatrix4x4 * skinMatrix, const float weight)
{
	__m128 w = _mm_set1_ps(weight);
				
	for(int i=0; i<16; i+=4)
	{
		__m128 a = _mm_loadu_ps(matrix->entries + i);
		__m128 b = _mm_loadu_ps(skinMatrix->entries + i);
		__m128 c = _mm_mul_ps(b, w);
		__m128 d = _mm_add_ps(a, c);
		_mm_storeu_ps(matrix->entries + i, d);
	}
}
#else
static void blendMatrices(MMatrix4x4 * matrix, const MMatrix4x4 * skinMatrix, const float weight)
{
	for(int i=0; i<16; i++)
		matrix->entries[i] += skinMatrix->entries[i] * weight;
}
#endif


// animation
bool animateFloat(MKey * keys, unsigned int keysNumber, float t, float * value)
{
	// no keys
	if(keysNumber == 0)
		return false;

	// one key
	if(keysNumber == 1){
		(*value) = *((float *)keys->getData());
		return true;
	}

	// out of range
	MKey * keyMin = keys;
	MKey * keyMax = keys + (keysNumber - 1);

	int tMin = keyMin->getT();
	int tMax = keyMax->getT();

	if (t <= tMin)
    {
		(*value) = *((float *)keyMin->getData());
		return true;
	}

	if (t >= tMax)
    {
		(*value) = *((float *)keyMax->getData());
		return true;
	}

	// interpolation
	for (unsigned int i = 1; i < keysNumber; i++)
	{
		MKey * key0 = keys;
		MKey * key1 = keys+1;

		int t0 = key0->getT();
		int t1 = key1->getT();

		if ((t >= t0) && (t <= t1))
		{
			float factor = (t - t0) / (float)(t1 - t0);
			float * data0 = (float *)key0->getData();
			float * data1 = (float *)key1->getData();

			(*value) = (*data0) + ((*data1) - (*data0))*factor;
			return true;
		}

		keys++;
	}

	return false;
}

bool animateVector2(MKey * keys, unsigned int keysNumber, float t, MVector2 * vector2)
{
	// no keys
	if (keysNumber == 0)
		return false;

	// one key
	if (keysNumber == 1)
    {
		(*vector2) = *((MVector2 *)keys->getData());
		return true;
	}

	// out of range
	MKey * keyMin = keys;
	MKey * keyMax = keys + (keysNumber - 1);

	int tMin = keyMin->getT();
	int tMax = keyMax->getT();

	if (t <= tMin)
    {
		(*vector2) = *((MVector2 *)keyMin->getData());
		return true;
	}

	if (t >= tMax)
    {
		(*vector2) = *((MVector2 *)keyMax->getData());
		return true;
	}

	// interpolation
	for (unsigned int i = 1; i < keysNumber; i++)
	{
		MKey * key0 = keys;
		MKey * key1 = keys+1;

		int t0 = key0->getT();
		int t1 = key1->getT();

		if ((t >= t0) && (t <= t1))
		{
			float factor = (t - t0) / (float)(t1 - t0);
			MVector2 * data0 = (MVector2 *)key0->getData();
			MVector2 * data1 = (MVector2 *)key1->getData();

			(*vector2) = (*data0) + ((*data1) - (*data0))*factor;
			return true;
		}

		keys++;
	}

	return false;
}

bool animateVector3(MKey * keys, unsigned int keysNumber, float t, MVector3 * vector3)
{
	// no keys
	if (keysNumber == 0)
		return false;

	// one key
	if (keysNumber == 1)
    {
		(*vector3) = *((MVector3 *)keys->getData());
		return true;
	}

	// out of range
	MKey * keyMin = keys;
	MKey * keyMax = keys + (keysNumber - 1);

	int tMin = keyMin->getT();
	int tMax = keyMax->getT();

	if (t <= tMin)
    {
		(*vector3) = *((MVector3 *)keyMin->getData());
		return true;
	}

	if (t >= tMax)
    {
		(*vector3) = *((MVector3 *)keyMax->getData());
		return true;
	}

	// interpolation
	for (unsigned int i = 1; i < keysNumber; i++)
	{
		MKey * key0 = keys;
		MKey * key1 = keys+1;

		int t0 = key0->getT();
		int t1 = key1->getT();

		if ((t >= t0) && (t <= t1))
		{
			float factor = (t - t0) / (float)(t1 - t0);
			MVector3 * data0 = (MVector3 *)key0->getData();
			MVector3 * data1 = (MVector3 *)key1->getData();

			(*vector3) = (*data0) + ((*data1) - (*data0))*factor;
			return true;
		}

		keys++;
	}

	return false;
}

bool animateQuaternion(MKey * keys, unsigned int keysNumber, float t, MQuaternion * quaternion)
{
	// no keys
	if (keysNumber == 0)
		return false;

	// one key
	if (keysNumber == 1)
    {
		(*quaternion) = *((MQuaternion *)keys->getData());
		return true;
	}

	// out of range
	MKey * keyMin = keys;
	MKey * keyMax = keys + (keysNumber - 1);

	int tMin = keyMin->getT();
	int tMax = keyMax->getT();

	if (t <= tMin)
    {
		(*quaternion) = *((MQuaternion *)keyMin->getData());
		return true;
	}

	if (t >= tMax)
    {
		(*quaternion) = *((MQuaternion *)keyMax->getData());
		return true;
	}

	// interpolation
	for (unsigned int i = 1; i < keysNumber; i++)
	{
		MKey * key0 = keys;
		MKey * key1 = keys+1;

		int t0 = key0->getT();
		int t1 = key1->getT();

		if (t == t0)
		{
			(*quaternion) = *(MQuaternion *)key0->getData();
			return true;
		}

		if (t == t1)
		{
			(*quaternion) = *(MQuaternion *)key1->getData();
			return true;
		}

		if ((t > t0) && (t < t1))
		{
			float factor = (t - t0) / (float)(t1 - t0);
			MQuaternion * data0 = (MQuaternion *)key0->getData();
			MQuaternion * data1 = (MQuaternion *)key1->getData();

			(*quaternion) = MQuaternion(*data0, *data1, factor);
			return true;
		}

		keys++;
	}

	return false;
}

void animateArmature(MArmature * armature, MArmatureAnim * armatureAnim, float t)
{
	MObject3dAnim * bonesAnim = armatureAnim->getBonesAnim();

	MVector3 position;
	MVector3 scale;
	MQuaternion rotation;

	unsigned int b;
	unsigned int bSize = armatureAnim->getBonesAnimNumber();
	for (b = 0; b < bSize; b++)
	{
		MOBone * bone = armature->getBone(b);
		
		// position
		if(animateVector3(bonesAnim->getPositionKeys(), bonesAnim->getPositionKeysNumber(), t, &position))
			bone->setPosition(position);

		// scale
		if(animateVector3(bonesAnim->getScaleKeys(), bonesAnim->getScaleKeysNumber(), t, &scale))
			bone->setScale(scale);

		// rotation
		if(animateQuaternion(bonesAnim->getRotationKeys(), bonesAnim->getRotationKeysNumber(), t, &rotation))
			bone->setRotation(rotation);

		bonesAnim++;
	}

	armature->processBonesLinking();
	armature->updateBonesSkinMatrix();
}

void animateTextures(MMesh * mesh, MTexturesAnim * texturesAnim, float t)
{
	MTextureAnim * texAnim = texturesAnim->getTexturesAnim();

	float rotation;
	MVector2 translate;
	MVector2 scale;

	unsigned int tex;
	unsigned int texSize = texturesAnim->getTexturesAnimNumber();
	for (tex = 0; tex < texSize; tex++)
	{
		MTexture * texture = mesh->getTexture(tex);
		
		// rotation
		if(animateFloat(texAnim->getRotationKeys(), texAnim->getRotationKeysNumber(), t, &rotation))
			texture->setTexRotate(rotation);

		// translate
		if(animateVector2(texAnim->getTranslateKeys(), texAnim->getTranslateKeysNumber(), t, &translate))
			texture->setTexTranslate(translate);

		// scale
		if(animateVector2(texAnim->getScaleKeys(), texAnim->getScaleKeysNumber(), t, &scale))
			texture->setTexScale(scale);

		texAnim++;
	}
}

void animateMaterials(MMesh * mesh, MMaterialsAnim * materialsAnim, float t)
{
	MMaterialAnim * matAnim = materialsAnim->getMaterialsAnim();

	float opacity;
	float shininess;
	float customValue;
	MVector3 diffuse;
	MVector3 specular;
	MVector3 emit;
	MVector3 customColor;

	unsigned int m;
	unsigned int mSize = materialsAnim->getMaterialsAnimNumber();
	for (m = 0; m < mSize; m++)
	{
		MMaterial * material = mesh->getMaterial(m);
		
		// opacity
		if(animateFloat(matAnim->getOpacityKeys(), matAnim->getOpacityKeysNumber(), t, &opacity))
			material->setOpacity(opacity);

		// shininess
		if(animateFloat(matAnim->getShininessKeys(), matAnim->getShininessKeysNumber(), t, &shininess))
			material->setShininess(shininess);

		// customValue
		if(animateFloat(matAnim->getCustomValueKeys(), matAnim->getCustomValueKeysNumber(), t, &customValue))
			material->setCustomValue(customValue);

		// diffuse
		if(animateVector3(matAnim->getDiffuseKeys(), matAnim->getDiffuseKeysNumber(), t, &diffuse))
			material->setDiffuse(diffuse);

		// specular
		if(animateVector3(matAnim->getSpecularKeys(), matAnim->getSpecularKeysNumber(), t, &specular))
			material->setSpecular(specular);

		// emit
		if(animateVector3(matAnim->getEmitKeys(), matAnim->getEmitKeysNumber(), t, &emit))
			material->setEmit(emit);

		// customColor
		if(animateVector3(matAnim->getCustomColorKeys(), matAnim->getCustomColorKeysNumber(), t, &customColor))
			material->setCustomColor(customColor);

		matAnim++;
	}
}


// skinning
void computeSkinning (
	MArmature * armature, MSkinData * skinData,
	const MVector3 * baseVertices, const MVector3 * baseNormals, const MVector3 * baseTangents,
	MVector3 * vertices, MVector3 * normals, MVector3 * tangents)
{
	MMatrix4x4 matrix;

	unsigned int p;
	unsigned int pSize = skinData->getPointsNumber();
	if(baseTangents && baseNormals)
	{
		for(p = 0; p < pSize; p++)
		{
			MSkinPoint * point = skinData->getPoint(p);
			const unsigned short * bonesIds = point->getBonesIds();
			const float * bonesWeights = point->getBonesWeights();
			
			unsigned int vertexId = point->getVertexId();
			unsigned int b, bSize = point->getBonesNumber();
			
			memset(matrix.entries, 0, sizeof(float)*16);
			
			for(b=0; b<bSize; b++)
			{
				MOBone * bone = armature->getBone(bonesIds[b]);
				blendMatrices(&matrix, bone->getSkinMatrix(), bonesWeights[b]);
			}
			
			vertices[vertexId] = matrix * baseVertices[vertexId];
			normals[vertexId] = matrix.getRotatedVector3(baseNormals[vertexId]);
			tangents[vertexId] = matrix.getRotatedVector3(baseTangents[vertexId]);
		}
	}
	else if(baseNormals)
	{
		for(p = 0; p < pSize; p++)
		{
			MSkinPoint * point = skinData->getPoint(p);
			const unsigned short * bonesIds = point->getBonesIds();
			const float * bonesWeights = point->getBonesWeights();
			
			unsigned int vertexId = point->getVertexId();
			unsigned int b, bSize = point->getBonesNumber();
			
			memset(matrix.entries, 0, sizeof(float)*16);
			
			for(b=0; b<bSize; b++)
			{
				MOBone * bone = armature->getBone(bonesIds[b]);
				blendMatrices(&matrix, bone->getSkinMatrix(), bonesWeights[b]);
			}
			
			vertices[vertexId] = matrix * baseVertices[vertexId];
			normals[vertexId] = matrix.getRotatedVector3(baseNormals[vertexId]);
		}
	}
	else
	{
		for(p = 0; p < pSize; p++)
		{
			MSkinPoint * point = skinData->getPoint(p);
			const unsigned short * bonesIds = point->getBonesIds();
			const float * bonesWeights = point->getBonesWeights();
			
			unsigned int vertexId = point->getVertexId();
			unsigned int b, bSize = point->getBonesNumber();
			
			memset(matrix.entries, 0, sizeof(float)*16);
			
			for(b=0; b<bSize; b++)
			{
				MOBone * bone = armature->getBone(bonesIds[b]);
				blendMatrices(&matrix, bone->getSkinMatrix(), bonesWeights[b]);
			}
			
			vertices[vertexId] = matrix * baseVertices[vertexId];
		}
	}
}


// raytracing
bool isMeshRaytraced (
	const MVector3 & origin, const MVector3 & direction,
	const void * indices, M_TYPES indicesType, const MVector3 * vertices, unsigned int size)
{
	switch(indicesType)
	{
		case M_USHORT:
		{
			unsigned int v;
			unsigned short * idx = (unsigned short *)indices;
			for(v = 0; v < size; v += 3)
			{
				const MVector3 * v1 = &vertices[idx[v]];
				const MVector3 * v2 = &vertices[idx[v+1]];
				const MVector3 * v3 = &vertices[idx[v+2]];

				float u, v;
				if(rayTriangleIntersection(origin, direction, *v1, *v2, *v3, u, v) > 0)
					return true;
			}
			break;
		}
		
		case M_UINT:
		{
			unsigned int v;
			unsigned int * idx = (unsigned int *)indices;
			for(v = 0; v < size; v += 3)
			{
				const MVector3 * v1 = &vertices[idx[v]];
				const MVector3 * v2 = &vertices[idx[v+1]];
				const MVector3 * v3 = &vertices[idx[v+2]];

				float u, v;
				if(rayTriangleIntersection(origin, direction, *v1, *v2, *v3, u, v) > 0)
					return true;
			}
			break;
		}
            
        default:
            break;
	}

	return false;
}

float rayMeshIntersection (
	const MVector3 & origin, const MVector3 & direction,
	const void * indices, M_TYPES indicesType, const MVector3 * vertices, unsigned int size)
{
	float nearDist = 0;

	switch(indicesType)
	{
		case M_USHORT:
		{
			unsigned int v;
			unsigned short * idx = (unsigned short *)indices;
			for(v = 0; v < size; v += 3)
			{
				const MVector3 * v1 = &vertices[idx[v]];
				const MVector3 * v2 = &vertices[idx[v+1]];
				const MVector3 * v3 = &vertices[idx[v+2]];

				// cull face
				/*if(cullMode != M_CULL_NONE)
				{
					MVector3 normal = ((*v1)-(*v2)).crossProduct((*v1)-(*v3));
					if(direction.dotProduct(normal) < 0)
						continue;
				}*/
				
				float u, v;
				float dist = rayTriangleIntersection(origin, direction, *v1, *v2, *v3, u, v);
				if(dist > 0)
				{
					if(dist < nearDist || nearDist == 0)
						nearDist = dist;
				}
			}
			break;
		}
		
		case M_UINT:
		{
			unsigned int v;
			unsigned int * idx = (unsigned int *)indices;
			for(v = 0; v < size; v += 3)
			{
				const MVector3 * v1 = &vertices[idx[v]];
				const MVector3 * v2 = &vertices[idx[v+1]];
				const MVector3 * v3 = &vertices[idx[v+2]];

				float u, v;
				float dist = rayTriangleIntersection(origin, direction, *v1, *v2, *v3, u, v);
				if(dist > 0)
				{
					if(dist < nearDist || nearDist == 0)
						nearDist = dist;
				}
			}
			break;
		}
            
        default:
            break;
	}
	
	return nearDist;
}

// TEST: Voxel
float rayMeshIntersectionAccel (
	const MAccelMap * accelMap, const MBox3d * box,
	const MVector3 & origin, const MVector3 & direction,
	const void * indices, M_TYPES indicesType, const MVector3 * vertices, const unsigned int size)
{
	MVector3 raypos = (origin - box->min)*accelMap->invUnit;
	MVector3 raydir = direction*accelMap->invUnitN;
	
	int sx = accelMap->sx;
	int sy = accelMap->sy;
	int sz = accelMap->sz;
	int sxy = sx*sy;
	
	raypos += raydir*0.001f;
	while(1)
	{
		if(raypos.x<0 || raypos.x>=sx || raypos.y<0 || raypos.y>=sy || raypos.z<0 || raypos.z>=sz)
			return 0;
			
		int x = (int)raypos.x;
		int y = (int)raypos.y;
		int z = (int)raypos.z;
			
		int id3d = sxy*z + sx*y + x;
		if(accelMap->bitMap[id3d])
			return ((box->min + raypos*accelMap->unit) - origin).getLength();
		raypos += raydir;
	}
	
	return 0;
}
