/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MStandardRenderer.cpp
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


#include <tinyutf8.h>
#include <MEngine.h>
#include <MLog.h>
#include "MStandardShaders.h"
#include "MStandardRenderer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// LIGHT SPACE PERSPECTIVE SHADOW MAPS
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// This programn is a demonstration of the Light Space Perspective Shadow Mapping
// algorithm described in the respective paper. Additional information and sample
// images are available at
//
// http://www.cg.tuwien.ac.at/research/vr/lispsm/
//
// Copyright and Disclaimer:
//
// This code is copyright Vienna University of Technology, 2004.
//
//
// Please feel FREE to COPY and USE the code to include it in your own work,
// provided you include this copyright notice.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors:
//
// Daniel Scherzer (scherzer@cg.tuwien.ac.at)
// Michael Wimmer (wimmer@cg.tuwien.ac.at)
//
// Date: October 14, 2004
// 2014: adapted for Maratis


static void createOrthoView(MMatrix4x4 * matrix, float left, float right, float bottom, float top, float zNear, float zFar)
{
	if(right == left || top == bottom || zFar == zNear)
		return;
	
	float tx = - (right + left)/(right - left);
	float ty = - (top + bottom)/(top - bottom);
	float tz = - (zFar + zNear)/(zFar - zNear);
	matrix->entries[0] = 2.0f/(right-left);
	matrix->entries[1] = 0.0f;
	matrix->entries[2] = 0.0f;
	matrix->entries[3] = 0.0f;
	matrix->entries[4] = 0.0f;
	matrix->entries[5] = 2.0f/(top-bottom);
	matrix->entries[6] = 0.0f;
	matrix->entries[7] = 0.0f;
	matrix->entries[8] = 0.0f;
	matrix->entries[9] = 0.0f;
	matrix->entries[10] = -2.0f/(zFar-zNear);
	matrix->entries[11] = 0.0f;
	matrix->entries[12] = tx;
	matrix->entries[13] = ty;
	matrix->entries[14] = tz;
	matrix->entries[15] = 1.0f;
}

static void scaleTranslateToFit(MMatrix4x4 * output, const float * vMin, const float * vMax)
{
	//createOrthoView(output, vMin[0], vMax[0], vMin[1], vMax[1], -vMax[2], -vMin[2]);
	
	
	output->entries[ 0] = 2/(vMax[0]-vMin[0]);
	output->entries[ 4] = 0;
	output->entries[ 8] = 0;
	output->entries[12] = -(vMax[0]+vMin[0])/(vMax[0]-vMin[0]);

	output->entries[ 1] = 0;
	output->entries[ 5] = 2/(vMax[1]-vMin[1]);
	output->entries[ 9] = 0;
	output->entries[13] = -(vMax[1]+vMin[1])/(vMax[1]-vMin[1]);

	output->entries[ 2] = 0;
	output->entries[ 6] = 0;
	output->entries[10] = 2/(vMax[2]-vMin[2]);
	output->entries[14] = -(vMax[2]+vMin[2])/(vMax[2]-vMin[2]);

	output->entries[ 3] = 0;
	output->entries[ 7] = 0;
	output->entries[11] = 0;
	output->entries[15] = 1;
	
}

static void linCombVector3(float * result, const float * pos, const float * dir, const double t)
{
	for(int i=0; i<3; i++)
		result[i] = pos[i]+t*dir[i];
}

static MVector3 calcNewDir(const MVector3 * B_points, const int B_size, const MVector3 & eyePos)
{
	MVector3 dir;
	for(int i=0; i<B_size; i++)
	{
		MVector3 p = B_points[i] - eyePos;
		dir+=p;
	}
	return dir.getNormalized();
}

static MVector3 calcUpVec(const MVector3 & viewDir, const MVector3 & lightDir)
{
	MVector3 left = lightDir.crossProduct(viewDir);
	return (left.crossProduct(lightDir)).getNormalized();
}

static void appendToCubicHull(float * min, float * max, const float * v)
{
	int j;
	for(j=0; j<3; j++)
	{
		if(v[j] < min[j])
		{
			min[j] = v[j];
		}
		else if(v[j] > max[j])
		{
			max[j] = v[j];
		}
	}
}

static void calcCubicHull(MVector3 * min, MVector3 * max, const MVector3 * ps, const int size)
{
	if(size > 0)
	{
		*min = *max = ps[0];
		for(int i=1; i<size; i++)
			appendToCubicHull(*min, *max, ps[i]);
	}
}

void mulHomogenPoint(float * output, float * m, const float * v)
{
	//if v == output -> overwriting problems -> so store in temp
	double x = m[0]*v[0] + m[4]*v[1] + m[ 8]*v[2] + m[12];
	double y = m[1]*v[0] + m[5]*v[1] + m[ 9]*v[2] + m[13];
	double z = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14];
	double w = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15];

	output[0] = x/w;
	output[1] = y/w;
	output[2] = z/w;
}

static void calcLispSMMtx(const MVector3 * B_points, const int B_size,
						  const MVector3 & eyePos, const MVector3 & viewDir, const MVector3 & lightDir, float nearDist,
						  MMatrix4x4 * lightView, MMatrix4x4 * lightProj)
{
	MVector3 * B_copy = new MVector3[B_size];
	double dotProd = viewDir.dotProduct(lightDir);
	double sinGamma = sqrt(1.0 - dotProd*dotProd);
	
	
	// uniform test
	/*if(dotProd > 0.75f)
	{
		lightView->lookAt(eyePos, lightDir, viewDir);
	
		for(int i=0; i<B_size; i++)
			B_copy[i] = (*lightView) * B_points[i];
		
		MVector3 min, max;
		calcCubicHull(&min, &max, B_copy, B_size);

		scaleTranslateToFit(lightProj, min, max);
		
		//transform from right handed into left handed coordinate system
		MMatrix4x4 rh2lf(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1);
		(*lightProj) = rh2lf * (*lightProj);
		
		delete[] B_copy;
		return;
	}*/
	
	
	

	//MVector3 up = calcUpVec(viewDir, lightDir);
	MVector3 up = calcUpVec(calcNewDir(B_points, B_size, eyePos), lightDir);
	
	//temporal light View
	//look from position(eyePos)
	//into direction(lightDir) 
	//with up vector(up)
	lightView->lookAt(eyePos, lightDir, up);

	//transform the light volume points from world into light space
	for(int i=0; i<B_size; i++)
		mulHomogenPoint(B_copy[i], lightView->entries, B_points[i]);
		
	//calculate the cubic hull (an AABB) 
	//of the light space extents of the intersection body B
	//and save the two extreme points min and max
	MVector3 min, max;
	calcCubicHull(&min, &max, B_copy, B_size);

	MMatrix4x4 lispMtx;
	{
		//use the formulas of the paper to get n (and f)
		double factor = 1.0/sinGamma;
		double z_n = factor*nearDist; //often 1
		double d = ABS(max.y-min.y); //perspective transform depth //light space y extents
		double z_f = z_n + d*sinGamma;
		double n = (z_n+sqrt(z_f*z_n))/sinGamma;
		double f = n+d;
		MVector3 pos;

		//new observer point n-1 behind eye position
		//pos = eyePos-up*(n-nearDist)
		linCombVector3(pos,eyePos,up*(1+MAX(0, dotProd)*nearDist*4),-(n-nearDist)); // hack

		lightView->lookAt(pos,lightDir,up);

		//one possibility for a simple perspective transformation matrix
		//with the two parameters n(near) and f(far) in y direction
		// a = (f+n)/(f-n); b = -2*f*n/(f-n);
		lispMtx.entries[ 5] = (f+n)/(f-n);		// [ 1 0 0 0]
		lispMtx.entries[13] = -2*f*n/(f-n);		// [ 0 a 0 b]
		lispMtx.entries[ 7] = 1;				// [ 0 0 1 0]
		lispMtx.entries[15] = 0;				// [ 0 1 0 0]

		//temporal arrangement for the transformation of the points to post-perspective space
		(*lightProj) = lispMtx * (*lightView);
		
		//transform the light volume points from world into the distorted light space
		for(int i=0; i<B_size; i++)
			mulHomogenPoint(B_copy[i], lightProj->entries, B_points[i]);
		
		//calculate the cubic hull (an AABB) 
		//of the light space extents of the intersection body B
		//and save the two extreme points min and max
		calcCubicHull(&min, &max, B_copy, B_size);
	}

	//refit to unit cube
	//this operation calculates a scale translate matrix that
	//maps the two extreme points min and max into (-1,-1,-1) and (1,1,1)
	scaleTranslateToFit(lightProj, min, max);

	//together
	(*lightProj) = (*lightProj) * lispMtx; // ligthProjection = scaleTranslate*lispMtx
	
	
	//transform from right handed into left handed coordinate system
	MMatrix4x4 rh2lf(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1);
	(*lightProj) = rh2lf * (*lightProj);
	
	delete[] B_copy;
}










/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MStandardRenderer::MStandardRenderer(void):
m_forceNoFX(false),
m_fboId(0),
m_currentCamera(NULL),
m_FXsNumber(0)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// default FXs
	addFX(vertShader0.c_str(), fragShader0.c_str());
	addFX(vertShader1.c_str(), fragShader1.c_str());
	addFX(vertShader2.c_str(), fragShader2.c_str());
	addFX(vertShader3.c_str(), fragShader3.c_str());
	addFX(vertShader4.c_str(), fragShader4.c_str());
	addFX(vertShader5.c_str(), fragShader5.c_str());
	addFX(vertShader6.c_str(), fragShader6.c_str());
	addFX(vertShader7.c_str(), fragShader7.c_str());
	addFX(vertShader8.c_str(), fragShader8.c_str());

	// rand texture
	MImage image;
	image.create(M_UBYTE, 64, 64, 4);
	unsigned char * pixel = (unsigned char *)image.getData();
	for(unsigned int i=0; i<image.getSize(); i++)
	{
		(*pixel) = (unsigned char)(rand()%256);
		pixel++;
	}

	render->createTexture(&m_randTexture);
	render->bindTexture(m_randTexture);
	render->setTextureFilterMode(M_TEX_FILTER_LINEAR_MIPMAP_LINEAR, M_TEX_FILTER_LINEAR);
	render->setTextureUWrapMode(M_WRAP_REPEAT);
	render->setTextureVWrapMode(M_WRAP_REPEAT);

	render->sendTextureImage(&image, 1, 1, 0);
}

MStandardRenderer::~MStandardRenderer(void)
{
	unsigned int i;
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// delete default FXs
	for(i=0; i<m_FXsNumber; i++)
	{
		render->deleteFX(&m_FXs[i]);
		render->deleteShader(&m_fragShaders[i]);
		render->deleteShader(&m_vertShaders[i]);
	}

	// delete shadowLights
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());
	for(;mit!=mend;++mit)
	{
		render->deleteTexture(&mit->second.shadowTexture);
	}

	// delete occlusion queries
	/*for(i=0; i<MAX_TRANSP; i++)
		render->deleteQuery(&m_transpList[i].occlusionQuery);
	for(i=0; i<MAX_OPAQUE; i++)
		render->deleteQuery(&m_opaqueList[i].occlusionQuery);
		*/

	// delete rand texture
	render->deleteTexture(&m_randTexture);

	// delete FBO
	render->deleteFrameBuffer(&m_fboId);
}

void MStandardRenderer::destroy(void)
{
	delete this;
}

MRenderer * MStandardRenderer::getNew(void)
{
	return new MStandardRenderer();
}

void MStandardRenderer::addFX(const char * vert, const char * frag)
{
	if(m_FXsNumber < MAX_DEFAULT_FXS)
	{
		MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

		render->createVertexShader(&m_vertShaders[m_FXsNumber]);
		render->sendShaderSource(m_vertShaders[m_FXsNumber], vert);
		render->createPixelShader(&m_fragShaders[m_FXsNumber]);
		render->sendShaderSource(m_fragShaders[m_FXsNumber], frag);
		render->createFX(&m_FXs[m_FXsNumber], m_vertShaders[m_FXsNumber], m_fragShaders[m_FXsNumber]);
		m_FXsNumber++;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Drawing
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool hasSubMeshTransparency(MOEntity * entity, MSubMesh * subMesh)
{
	unsigned int i, displayNumber = subMesh->getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		MMaterial * material = entity->getMaterial(display->getMaterialId());
		
		if((! display->isVisible()) || (! material))
			continue;

		if(material->getBlendMode() != M_BLENDING_NONE)
			return true;
	}

	return false;
}

void MStandardRenderer::initVBO(MSubMesh * subMesh, MSubMeshCache * subMeshCahe, MVector3 * vertices, MVector3 * normals, MVector3 * tangents)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	unsigned int * vboId1 = subMesh->getVBOid1();
	unsigned int * vboId2 = subMesh->getVBOid2();
	
	// indices
	M_TYPES indicesType = subMesh->getIndicesType();
	void * indices = subMesh->getIndices();
		
	M_VBO_MODES mode = M_VBO_STATIC;
	if(subMeshCahe)
	{
		mode = M_VBO_DYNAMIC;
		vboId1 = subMeshCahe->getVBOid();
	}
	
	if(*vboId1 == 0 || mode != M_VBO_STATIC)
	{
		// data
		MColor * colors = subMesh->getColors();
		MVector2 * texCoords = subMesh->getTexCoords();
		
		unsigned int totalSize = sizeof(MVector3)*subMesh->getVerticesSize();
		if(normals)
			totalSize += sizeof(MVector3)*subMesh->getNormalsSize();
		if(tangents)
			totalSize += sizeof(MVector3)*subMesh->getTangentsSize();
		if(texCoords)
			totalSize += sizeof(MVector2)*subMesh->getTexCoordsSize();
		if(colors)
			totalSize += sizeof(MColor)*subMesh->getColorsSize();

		// data VBO
		bool init = (*vboId1 == 0);
		if(init)
			render->createVBO(vboId1);
			
		render->bindVBO(M_VBO_ARRAY, *vboId1);
		
		if(init)
			render->setVBO(M_VBO_ARRAY, 0, totalSize, mode);
		
		unsigned int offset = 0;
		render->setVBOSubData(M_VBO_ARRAY, offset, vertices, sizeof(MVector3)*subMesh->getVerticesSize());
		offset += sizeof(MVector3)*subMesh->getVerticesSize();
		
		if(normals)
		{
			render->setVBOSubData(M_VBO_ARRAY, offset, normals, sizeof(MVector3)*subMesh->getNormalsSize());
			offset += sizeof(MVector3)*subMesh->getNormalsSize();
		}
		
		if(tangents)
		{
			render->setVBOSubData(M_VBO_ARRAY, offset, tangents, sizeof(MVector3)*subMesh->getTangentsSize());
			offset += sizeof(MVector3)*subMesh->getTangentsSize();
		}
		
		if(*vboId1 == 0)
		{
			if(texCoords)
			{
				render->setVBOSubData(M_VBO_ARRAY, offset, texCoords, sizeof(MVector2)*subMesh->getTexCoordsSize());
				offset += sizeof(MVector2)*subMesh->getTexCoordsSize();
			}
		
			if(colors)
			{
				render->setVBOSubData(M_VBO_ARRAY, offset, colors, sizeof(MColor)*subMesh->getColorsSize());
				offset += sizeof(MColor)*subMesh->getColorsSize();
			}
		}
		
		render->bindVBO(M_VBO_ARRAY, 0);
	}
	
	// indices VBO
	if(*vboId2 == 0 && indices)
	{
		unsigned int typeSize = indicesType == M_USHORT ? sizeof(short) : sizeof(int);
			
		render->createVBO(vboId2);
		render->bindVBO(M_VBO_ELEMENT_ARRAY, *vboId2);
		render->setVBO(M_VBO_ELEMENT_ARRAY, indices, subMesh->getIndicesSize()*typeSize, mode);
		render->bindVBO(M_VBO_ELEMENT_ARRAY, 0);
	}
}

void MStandardRenderer::drawDisplay(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MVector3 * tangents, MColor * colors)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();


	// VBO
	unsigned int * vboId1 = subMesh->getVBOid1();
	if(subMeshCahe)
		vboId1 = subMeshCahe->getVBOid();
	
	unsigned int * vboId2 = subMesh->getVBOid2();
	
	
	// get material
	MMaterial * material = entity->getMaterial(display->getMaterialId());
	{
		float opacity = material->getOpacity();
		if(opacity <= 0.0f)
			return;

		// data
		M_TYPES indicesType = subMesh->getIndicesType();
		void * indices = subMesh->getIndices();
		MVector2 * texCoords = subMesh->getTexCoords();

		// begin / size
		unsigned int begin = display->getBegin();
		unsigned int size = display->getSize();

		// get properties
		M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
		M_BLENDING_MODES blendMode = material->getBlendMode();
		M_CULL_MODES cullMode = display->getCullMode();
		MVector3 diffuse = material->getDiffuse();
		MVector3 specular = material->getSpecular();
		MVector3 emit = material->getEmit();
		float shininess = material->getShininess();

		// get current fog color
		MVector3 currentFogColor;
		render->getFogColor(&currentFogColor);

		// set cull mode
		if(cullMode == M_CULL_NONE)
        {
			render->disableCullFace();
		}
		else{
			render->enableCullFace();
			render->setCullMode(cullMode);
		}

		// set blending mode
		render->setBlendingMode(blendMode);

		// set fog color depending on blending
		switch(blendMode)
		{
			case M_BLENDING_ADD:
			case M_BLENDING_LIGHT:
				render->setFogColor(MVector3(0, 0, 0));
				break;

			case M_BLENDING_PRODUCT:
				render->setFogColor(MVector3(1, 1, 1));
				break;

            default:
                break;
		}

		// texture passes
		unsigned int texturesPassNumber = MIN(8, material->getTexturesPassNumber());

		// FX
		unsigned int fxId = 0;
		MFXRef * FXRef = material->getFXRef();
		MFXRef * ZFXRef = material->getZFXRef();

		if(FXRef)
			fxId = FXRef->getFXId();

		bool basicFX = false;

		// force NoFX
		if(m_forceNoFX)
		{
			// optimize only for standard shader (for custom shader we don't know how geometry and alpha test is done)
			if(fxId == 0)
			{
				fxId = m_FXs[0]; // basic FX
				texturesPassNumber = 0;

				// alpha test
				if(material->getTexturesPassNumber() > 0)
				{
					MTexture * texture = material->getTexturePass(0)->getTexture();
					if(texture)
					{
						if(texture->getTextureRef())
						{
							if(texture->getTextureRef()->getComponents() > 3)
							{
								fxId = m_FXs[7]; // basic FX with texture
								texturesPassNumber = 1;
							}
						}
					}
				}

				basicFX = true;
			}
			else if(ZFXRef) // if custom shader, use the Z FX is any
			{
				fxId = ZFXRef->getFXId();
			}
		}

		// standard shader
		else if(fxId == 0)
		{
			if(material->getTexturesPassNumber() == 0) // simple
				fxId = m_FXs[1];
			else if(material->getTexturesPassNumber() == 1) // diffuse
				fxId = m_FXs[2];
			else if(material->getTexturesPassNumber() == 2) // diffuse+specular
				fxId = m_FXs[3];
			else if(material->getTexturesPassNumber() == 3) // diffuse+specular+normal
				fxId = m_FXs[4];
			else{
				if(material->getTexturePass(2)->getTexture()) // diffuse+specular+emit+normal
					fxId = m_FXs[6];
				else
					fxId = m_FXs[5]; // diffuse+specular+emit
			}
		}

		// FX pipeline
		{
			unsigned int attribList[64];
			unsigned int attribListNb = 0;
		
			int attribIndex;
			MMatrix4x4 * cameraViewMatrix = m_currentCamera->getCurrentViewMatrix();
			MMatrix4x4 * cameraProjMatrix = m_currentCamera->getCurrentProjMatrix();

			// properties
			int AlphaTest;
			MVector3 FogColor;
			float FogEnd;
			float FogScale;

			MVector4 LightPosition[4];
			MVector3 LightDiffuseProduct[4];
			MVector3 LightSpecularProduct[4];
			MVector3 LightSpotDirection[4];
			float LightConstantAttenuation[4];
			float LightQuadraticAttenuation[4];
			float LightSpotCosCutoff[4];
			float LightSpotExponent[4];
			int LightActive[4];

			int ShadowMaps[4];
			int Texture[8] = {0, 1, 2, 3, 4, 5, 6, 7};
			MMatrix4x4 TextureMatrix[8];
			MMatrix4x4 ProjModelViewMatrix;
			MMatrix4x4 NormalMatrix;


			// Alpha test
			AlphaTest = (blendMode != M_BLENDING_ALPHA);

			// Matrix
			ProjModelViewMatrix = (*cameraProjMatrix) * m_currModelViewMatrix;

			if(! basicFX)
			{
				// Fog
				float min, max;
				render->getFogColor(&FogColor);
				render->getFogDistance(&min, &max);
				FogEnd = max;
				FogScale = 1.0f / (max - min);

				// Lights
				for(int i=0; i<4; i++)
				{
					float spotAngle;
					float linearAttenuation;
					MVector4 lightDiffuse;
					render->getLightDiffuse(i, &lightDiffuse);
					render->getLightPosition(i, &LightPosition[i]);
					render->getLightSpotDirection(i, &LightSpotDirection[i]);
					render->getLightAttenuation(i, &LightConstantAttenuation[i], &linearAttenuation, &LightQuadraticAttenuation[i]);
					render->getLightSpotAngle(i, &spotAngle);
					render->getLightSpotExponent(i, &LightSpotExponent[i]);

					LightActive[i] = (lightDiffuse.w > 0.0f);
					LightSpotCosCutoff[i] = cosf(spotAngle*DEG_TO_RAD);
					LightDiffuseProduct[i] = (diffuse) * MVector3(lightDiffuse);
					LightSpecularProduct[i] = (specular) * MVector3(lightDiffuse);
					LightPosition[i] = (*cameraViewMatrix) * MVector4(LightPosition[i]);
					LightSpotDirection[i] = (cameraViewMatrix->getRotatedVector3(LightSpotDirection[i])).getNormalized();
				}

				// Normal Matrix
				NormalMatrix = m_currModelViewMatrix.getInversetranspose();
			}


			// bind FX
			render->bindFX(fxId);

			
			// bind VBO is any
			if(*vboId1 > 0)
				render->bindVBO(M_VBO_ARRAY, *vboId1);
			
			
			// Vertex
			render->getAttribLocation(fxId, "Vertex", &attribIndex);
			if(attribIndex != -1)
			{
				if(*vboId1 > 0)	render->setAttribPointer(attribIndex, M_FLOAT, 3, 0);
				else			render->setAttribPointer(attribIndex, M_FLOAT, 3, vertices);
				render->enableAttribArray(attribIndex);
				attribList[attribListNb] = attribIndex; attribListNb++;
			}

			if(! basicFX)
			{
				unsigned long offset = sizeof(MVector3)*subMesh->getVerticesSize();
				
				// Normal
				if(normals)
				{
					render->getAttribLocation(fxId, "Normal", &attribIndex);
					if(attribIndex != -1)
					{
						if(*vboId1 > 0)	render->setAttribPointer(attribIndex, M_FLOAT, 3, (void*)offset);
						else			render->setAttribPointer(attribIndex, M_FLOAT, 3, normals);
						render->enableAttribArray(attribIndex);
						attribList[attribListNb] = attribIndex; attribListNb++;
					}
					
					offset += sizeof(MVector3)*subMesh->getNormalsSize();
				}

				// Tangent
				if(tangents)
				{
					render->getAttribLocation(fxId, "Tangent", &attribIndex);
					if(attribIndex != -1)
					{
						if(*vboId1 > 0)	render->setAttribPointer(attribIndex, M_FLOAT, 3, (void*)offset);
						else			render->setAttribPointer(attribIndex, M_FLOAT, 3, tangents);
						render->enableAttribArray(attribIndex);
						attribList[attribListNb] = attribIndex; attribListNb++;
					}
					
					offset += sizeof(MVector3)*subMesh->getTangentsSize();
				}

				// Texcoords
				if(texCoords)
				{
					offset += sizeof(MVector2)*subMesh->getTexCoordsSize();
				}
				
				// Color
				if(colors)
				{
					render->getAttribLocation(fxId, "Color", &attribIndex);
					if(attribIndex != -1)
					{
						if(*vboId1 > 0)	render->setAttribPointer(attribIndex, M_UBYTE, 3, (void*)offset, true);
						else			render->setAttribPointer(attribIndex, M_UBYTE, 3, colors, true);
						render->enableAttribArray(attribIndex);
						attribList[attribListNb] = attribIndex; attribListNb++;
					}
				}
			}


			// Textures
			unsigned int textureArrayOffset = sizeof(MVector3)*subMesh->getVerticesSize();
			{
				if(normals) textureArrayOffset += sizeof(MVector3)*subMesh->getNormalsSize();
				if(tangents) textureArrayOffset += sizeof(MVector3)*subMesh->getTangentsSize();
			}
			
			int id = texturesPassNumber;
			for(unsigned int t=0; t<texturesPassNumber; t++)
			{
				MTexturePass * texturePass = material->getTexturePass(t);

				MTexture * texture = texturePass->getTexture();
				if((! texture) || (! texCoords))
				{
					render->bindTexture(0, t);
					continue;
				}

				// texCoords
				unsigned int offset = 0;
				if(subMesh->isMapChannelExist(texturePass->getMapChannel()))
					offset = subMesh->getMapChannelOffset(texturePass->getMapChannel());

				// texture id
				unsigned int textureId = 0;
				MTextureRef * texRef = texture->getTextureRef();
				if(texRef)
					textureId = texRef->getTextureId();

				// bind texture
				render->bindTexture(textureId, t);
				render->setTextureUWrapMode(texture->getUWrapMode());
				render->setTextureVWrapMode(texture->getVWrapMode());

				// texture matrix
				MMatrix4x4 * texMatrix = &TextureMatrix[t];
				texMatrix->loadIdentity();
				texMatrix->translate(MVector2(0.5f, 0.5f));
				texMatrix->scale(texture->getTexScale());
				texMatrix->rotate(MVector3(0, 0, -1), texture->getTexRotate());
				texMatrix->translate(MVector2(-0.5f, -0.5f));
				texMatrix->translate(texture->getTexTranslate());

				// texture coords
				char name[16];
				sprintf(name, "TexCoord%d", t);
				render->getAttribLocation(fxId, name, &attribIndex);
				if(attribIndex != -1)
				{
					if(*vboId1 > 0)	render->setAttribPointer(attribIndex, M_FLOAT, 2, (void*)(textureArrayOffset + sizeof(MVector2)*offset));
					else			render->setAttribPointer(attribIndex, M_FLOAT, 2, texCoords + offset);
					render->enableAttribArray(attribIndex);
					attribList[attribListNb] = attribIndex; attribListNb++;
				}
			}

			if(! basicFX)
			{
				// Shadows
				for(int i=0; i<4; i++)
				{
					if(m_lightShadow[i] == 1)
					{
						render->bindTexture(m_lightShadowTexture[i], id);
						ShadowMaps[i] = id;
						id++;
					}
					else {
						render->bindTexture(0, id);
						ShadowMaps[i] = id;
						id++;
					}
				}

				// rand texture
				int randTextureId = id;
				{
					render->bindTexture(m_randTexture, id);
					id++;
				}

				// uniforms
				render->sendUniformVec4(fxId, "FogColor", MVector4(FogColor));
				render->sendUniformFloat(fxId, "FogEnd", &FogEnd);
				render->sendUniformFloat(fxId, "FogScale", &FogScale);

				render->sendUniformVec3(fxId, "MaterialEmit", emit);
				render->sendUniformFloat(fxId, "MaterialShininess", &shininess);

				render->sendUniformVec4(fxId, "LightPosition", LightPosition[0], 4);
				render->sendUniformVec3(fxId, "LightDiffuseProduct", LightDiffuseProduct[0], 4);
				render->sendUniformVec3(fxId, "LightSpecularProduct", LightSpecularProduct[0], 4);
				render->sendUniformVec3(fxId, "LightSpotDirection", LightSpotDirection[0], 4);
				render->sendUniformFloat(fxId, "LightConstantAttenuation", LightConstantAttenuation, 4);
				render->sendUniformFloat(fxId, "LightQuadraticAttenuation", LightQuadraticAttenuation, 4);
				render->sendUniformFloat(fxId, "LightSpotCosCutoff", LightSpotCosCutoff, 4);
				render->sendUniformFloat(fxId, "LightSpotExponent", LightSpotExponent, 4);
				render->sendUniformInt(fxId, "LightActive", LightActive, 4);

				render->sendUniformInt(fxId, "LightShadowMap", ShadowMaps, 4);
				render->sendUniformInt(fxId, "LightShadow", m_lightShadow, 4);
				render->sendUniformFloat(fxId, "LightShadowBias", m_lightShadowBias, 4);
				render->sendUniformFloat(fxId, "LightShadowBlur", m_lightShadowBlur, 4);
				render->sendUniformMatrix(fxId, "LightShadowMatrix", m_lightShadowMatrix, 4);

				render->sendUniformInt(fxId, "RandTexture", &randTextureId);

				render->sendUniformMatrix(fxId, "ModelViewMatrix", &m_currModelViewMatrix);
				render->sendUniformMatrix(fxId, "NormalMatrix", &NormalMatrix);
				render->sendUniformMatrix(fxId, "ProjectionMatrix", cameraProjMatrix);
			}

			if(texturesPassNumber > 0)
			{
				render->sendUniformInt(fxId, "AlphaTest", &AlphaTest);
				render->sendUniformInt(fxId, "Texture", Texture, texturesPassNumber);
				render->sendUniformMatrix(fxId, "TextureMatrix", TextureMatrix, texturesPassNumber);
			}

			render->sendUniformFloat(fxId, "MaterialOpacity", &opacity);
			render->sendUniformMatrix(fxId, "ProjModelViewMatrix", &ProjModelViewMatrix);


			// draw
			if(indices)
			{
				if(*vboId2 > 0)
				{
					render->bindVBO(M_VBO_ELEMENT_ARRAY, *vboId2);
					
					switch(indicesType)
					{
						case M_USHORT:
							render->drawElement(primitiveType, size, indicesType, (void*)(begin*sizeof(short)));
							break;
						case M_UINT:
							render->drawElement(primitiveType, size, indicesType, (void*)(begin*sizeof(int)));
							break;
						default:
							break;
					}
				}
				else
				{
					switch(indicesType)
					{
						case M_USHORT:
							render->drawElement(primitiveType, size, indicesType, (unsigned short*)indices + begin);
							break;
						case M_UINT:
							render->drawElement(primitiveType, size, indicesType, (unsigned int*)indices + begin);
							break;
						default:
							break;
					}
				}
			}
			else{
				render->drawArray(primitiveType, begin, size);
			}


			// disable attribs
			for(int i=0; i<attribListNb; i++)
				render->disableAttribArray(attribList[i]);

			// restore textures
			for(int t=(int)(id-1); t>=0; t--)
			{
				render->bindTexture(0, t);
				render->disableTexture();
			}

			// restore FX
			render->bindFX(0);
			
			// restore VBO
			render->bindVBO(M_VBO_ARRAY, 0);
			render->bindVBO(M_VBO_ELEMENT_ARRAY, 0);
		}

		// restore fog and alpha test
		render->setFogColor(currentFogColor);
	}
}

void MStandardRenderer::drawOpaques(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MArmature * armature)
{
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MVector3 * tangents = subMesh->getTangents();
	MColor * colors = subMesh->getColors();

	if(! vertices)
		return;

	if(subMeshCahe)
	{
		vertices = subMeshCahe->getVertices();
		normals = subMeshCahe->getNormals();
		tangents = subMeshCahe->getTangents();
	}

	initVBO(subMesh, subMeshCahe, vertices, normals, tangents);

	unsigned int i;
	unsigned int displayNumber = subMesh->getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		if(! display->isVisible())
			continue;

		MMaterial * material = entity->getMaterial(display->getMaterialId());
		if(material)
		{
			if(material->getBlendMode() == M_BLENDING_NONE)
				drawDisplay(entity, subMeshCahe, subMesh, display, vertices, normals, tangents, colors);
		}
	}
}

void MStandardRenderer::drawTransparents(MOEntity * entity, MSubMeshCache * subMeshCahe, MSubMesh * subMesh, MArmature * armature)
{
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MVector3 * tangents = subMesh->getTangents();
	MColor * colors = subMesh->getColors();

	if(! vertices)
		return;

	if(subMeshCahe)
	{
		vertices = subMeshCahe->getVertices();
		normals = subMeshCahe->getNormals();
		tangents = subMeshCahe->getTangents();
	}
	
	initVBO(subMesh, subMeshCahe, vertices, normals, tangents);
		
	unsigned int i;
	unsigned int displayNumber = subMesh->getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		if(! display->isVisible())
			continue;

		MMaterial * material = entity->getMaterial(display->getMaterialId());
		if(material)
		{
			if(material->getBlendMode() != M_BLENDING_NONE)
				drawDisplay(entity, subMeshCahe, subMesh, display, vertices, normals, tangents, colors);
		}
	}
}

float MStandardRenderer::getDistanceToCam(MOCamera * camera, const MVector3 & pos)
{
	if(! camera->isOrtho())
	{
		return (pos - camera->getTransformedPosition()).getSquaredLength();
	}

	MVector3 axis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	float dist = (pos - camera->getTransformedPosition()).dotProduct(axis);
	return dist*dist;
}

void MStandardRenderer::setShadowMatrix(MMatrix4x4 * matrix, MOCamera * camera)
{
	const MMatrix4x4 biasMatrix(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	MMatrix4x4 * modelViewMatrix = camera->getCurrentViewMatrix();
	MMatrix4x4 * projMatrix = camera->getCurrentProjMatrix();

	(*matrix) = biasMatrix;
	(*matrix) = (*matrix) * (*projMatrix);
	(*matrix) = (*matrix) * (*modelViewMatrix);
}


/*
// TEST: software rendering
void tri2(MImage * image, const MVector4 & v0, const MVector4 & v1, const MVector4 & v2, const MVector3 & p0, const MVector3 & p1, const MVector3 & p2)
{
	if(v0.w <= 0 || v1.w <= 0 || v2.w <= 0) // no clip
		return;

	// denominator
	float denom = (v1.x - v2.x)*(v2.y - v0.y) - (v1.y - v2.y)*(v2.x - v0.x);
	if(denom >= 0.0f)
		return;
		
	float idenom = 1.0f/denom;
	int w = image->getWidth();
	int h = image->getHeight();

	// bounding box
	int minx = MIN(v0.x, MIN(v1.x, v2.x));
	int miny = MIN(v0.y, MIN(v1.y, v2.y));
	int maxx = MAX(v0.x, MAX(v1.x, v2.x));
	int maxy = MAX(v0.y, MAX(v1.y, v2.y));
	minx = MAX(minx, 0);
	miny = MAX(miny, 0);
	maxx = MIN(maxx, w-1);
	maxy = MIN(maxy, h-1);

	// scanline
    float t0_row = ((v2.x-v1.x)*(miny-v1.y) - (v2.y-v1.y)*(minx-v1.x))*idenom;
    float t1_row = ((v0.x-v2.x)*(miny-v2.y) - (v0.y-v2.y)*(minx-v2.x))*idenom;
	
	float deltaX0 = (v0.x - v2.x)*idenom;
	float deltaX2 = (v2.x - v1.x)*idenom;
	float deltaY1 = (v1.y - v2.y)*idenom;
	float deltaY2 = (v2.y - v0.y)*idenom;

	MVector3 up0 = p0*v0.w;
	MVector3 up1 = p1*v1.w;
	MVector3 up2 = p2*v2.w;

	int w3 = w*3;
	int mx3 = minx*3;
	
	unsigned char * yline = (unsigned char *)image->getData() + miny*w3;
	for(int y=miny; y<=maxy; y++)
	{
		float t0 = t0_row;
		float t1 = t1_row;

		unsigned char * pixel = yline + mx3;
		for(int x=minx; x<=maxx; x++)
		{
			float t2 = t0+t1;
			if(t0>=0 && t1>=0 && t2<=1)
			{
				t2 = 1-t2;
				
				float w = t0*v0.w + t1*v1.w + t2*v2.w;
				MVector3 pos = (up0*t0 + up1*t1 + up2*t2)/w;
				
				//float z = CLAMP((t0*v0.z + t1*v1.z + t2*v2.z), 0, 1);
				pixel[0] = pos.x*10+128;
				pixel[1] = pos.y*10+128;
				pixel[2] = pos.z*10+128;
			}

			t0 += deltaY1;
			t1 += deltaY2;
			pixel += 3;
		}

		t0_row += deltaX2;
		t1_row += deltaX0;
		yline += w3;
    }
}

bool rayBoxIntersection2(const MVector4 origin, const MVector4 direction, const MVector4 min, const MVector4 max, float * enter, float * exit)
{
	MVector4 idir(
		1.0f / direction.x,
		1.0f / direction.y,
		1.0f / direction.z,
		1.0f / direction.w
	);
	
	MVector4 tmin = (min - origin)*idir;
    MVector4 tmax = (max - origin)*idir;
    
	MVector4 tnear(
		MIN(tmin.x, tmax.x),
		MIN(tmin.y, tmax.y),
		MIN(tmin.z, tmax.z),
		MIN(tmin.w, tmax.w)
	);
	
	MVector4 tfar(
		MAX(tmin.x, tmax.x),
		MAX(tmin.y, tmax.y),
		MAX(tmin.z, tmax.z),
		MAX(tmin.w, tmax.w)
	);
	
    float ent = MAX(MAX(tnear.x, 0.0f), MAX(tnear.y, tnear.z));
    float ex = MIN(tfar.x, MIN(tfar.y, tfar.z));
    
    if(ex > 0.0f && ent < ex)
	{
		*enter = ent;
		*exit = ex;
		return 1;
	}

	return 0;
}
*/


void MStandardRenderer::updateVisibility(MScene * scene, MOCamera * camera)
{
	// make frustum
	camera->getFrustum()->makeVolume(camera);

	// compute object visibility
	unsigned int i;
	unsigned int oSize = scene->getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);
		if(object->isActive())
			object->updateVisibility(camera);
	}

	/*
	// TEST: software rendering
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	MOEntity * entity = scene->getEntityByIndex(0);
	MBox3d * ebox = entity->getBoundingBox();
	
	static MImage renderFrame;
	
	int width = 128;
	int height = 64;
	renderFrame.create(M_UBYTE, width, height, 3);
	memset(renderFrame.getData(), 0, renderFrame.getSize()*sizeof(char));
	
	MSystemContext * system = MEngine::getInstance()->getSystemContext();
	unsigned int tick = system->getSystemTick();
	
	// test ray trace, occlusion culling ?
	MMatrix4x4 invViewProjMatrix = ((*camera->getCurrentProjMatrix()) * (*camera->getCurrentViewMatrix())).getInverse();
	MVector3 origin = camera->getTransformedPosition();
	float divx = 1.0f/(float)width;
	float divy = 1.0f/(float)height;
	
	
	// test 1
	{
		MVector3 A, B, C;
		{
			MVector4 point(-1, -1, 0.01f, 1);
			MVector4 v = invViewProjMatrix * point;
			A = MVector3(v.x, v.y, v.z)/v.w;
			
			point.x = 2*divx - 1;
			v = invViewProjMatrix * point;
			B = MVector3(v.x, v.y, v.z)/v.w;
			
			point.x = -1;
			point.y = 2*divy - 1;
			v = invViewProjMatrix * point;
			C = MVector3(v.x, v.y, v.z)/v.w;
		}
	
		//for(int i=0; i<100; i++)
		{
			MSubMesh * subMesh = &entity->getMesh()->getSubMeshs()[0];
			
			MMatrix4x4 * matrix = entity->getMatrix();
			MVector3 localOrigin = matrix->getInverseTransformedVector3(origin);
	
			MVector3 startPt = matrix->getInverseTransformedVector3(A);
			MVector3 dx = matrix->getInverseTransformedVector3(B) - startPt;
			MVector3 dy = matrix->getInverseTransformedVector3(C) - startPt;
		
			MVector3 ypos = startPt + dy*(height-1);
			
			unsigned char * pixel = (unsigned char *)renderFrame.getData();
			for(int y=0; y<height; y++)
			{
				MVector3 pos = ypos;
				for(int x=0; x<width; x++)
				{
					MVector3 localDir = (pos - localOrigin);
				
					float enter = 0, exit = 0;
					if(! isPointInBox(localOrigin, ebox->min, ebox->max))
					{
						if(! rayBoxIntersection2(localOrigin, localDir, ebox->min, ebox->max, &enter, &exit))
						{
							pos+=dx;
							pixel+=3;
							continue;
						}
					}
					
					MVector3 localOrigin2 = localOrigin + localDir*enter;
					MVector3 localExit = localOrigin + localDir*exit;
					MVector3 localDir2 = localDir.getNormalized();
					
					float dist = rayMeshIntersectionAccel(
						subMesh->getAccelMap(), subMesh->getBoundingBox(),
						localOrigin2, localDir2,
						subMesh->getIndices(), subMesh->getIndicesType(),
						subMesh->getVertices(), subMesh->getIndicesSize()/3
					);
						
					if(dist > 0)
					{
						MVector3 pos = matrix->getTransformedVector3(localOrigin2 + localDir2*dist);
						
						unsigned char d = CLAMP((pos - origin).getLength()*4, 0, 255);
						pixel[0] = d;
						pixel[1] = d;
						pixel[2] = d;
					}
					
					pos+=dx;
					pixel+=3;
				}
			
				ypos-=dy;
			}
		}
	}

	// test 2 // 2 X faster (without clipping)
	{
		float hw = width*0.5f;
		float hh = height*0.5f;
		
		MMatrix4x4 modelViewProjMatrix = (*camera->getCurrentProjMatrix()) * (*camera->getCurrentViewMatrix());
	
		//for(int i=0; i<100; i++)
		{
			MMatrix4x4 * matrix = entity->getMatrix();
		
			MVector3 points[8] = {
				MVector3(ebox->min.x, ebox->min.y, ebox->min.z),
				MVector3(ebox->max.x, ebox->min.y, ebox->min.z),
				MVector3(ebox->max.x, ebox->max.y, ebox->min.z),
				MVector3(ebox->min.x, ebox->max.y, ebox->min.z),
				MVector3(ebox->min.x, ebox->min.y, ebox->max.z),
				MVector3(ebox->max.x, ebox->min.y, ebox->max.z),
				MVector3(ebox->max.x, ebox->max.y, ebox->max.z),
				MVector3(ebox->min.x, ebox->max.y, ebox->max.z)
			};
		
			MVector4 points2d[8];
			for(int i=0; i<8; i++)
			{
				points2d[i] = (*matrix)*points[i];
				points2d[i] = modelViewProjMatrix*points2d[i];
			
				float iw = 1.0f/points2d[i].w;
				points2d[i].x = (points2d[i].x*iw + 1)*hw;
				points2d[i].y = height-1 - (points2d[i].y*iw + 1)*hh;
				points2d[i].z *= iw;
				points2d[i].w = iw;
			}

			tri2(&renderFrame, points2d[2], points2d[1], points2d[0], points[2], points[1], points[0]);
			tri2(&renderFrame, points2d[0], points2d[3], points2d[2], points[0], points[3], points[2]);
			tri2(&renderFrame, points2d[4], points2d[5], points2d[6], points[4], points[5], points[6]);
			tri2(&renderFrame, points2d[6], points2d[7], points2d[4], points[6], points[7], points[4]);
			tri2(&renderFrame, points2d[7], points2d[6], points2d[2], points[7], points[6], points[2]);
			tri2(&renderFrame, points2d[2], points2d[3], points2d[7], points[2], points[3], points[7]);
			tri2(&renderFrame, points2d[4], points2d[0], points2d[1], points[4], points[0], points[1]);
			tri2(&renderFrame, points2d[1], points2d[5], points2d[4], points[1], points[5], points[4]);
			tri2(&renderFrame, points2d[3], points2d[0], points2d[4], points[3], points[0], points[4]);
			tri2(&renderFrame, points2d[4], points2d[7], points2d[3], points[4], points[7], points[3]);
			tri2(&renderFrame, points2d[1], points2d[2], points2d[6], points[1], points[2], points[6]);
			tri2(&renderFrame, points2d[6], points2d[5], points2d[1], points[6], points[5], points[1]);
		}
	}
	
	unsigned int elapsed = system->getSystemTick() - tick;
	printf("elapsed %d ms\n", elapsed);
	
	MDataManager * texMan = MEngine::getInstance()->getLevel()->getTextureManager();
	MTextureRef * texRef = (MTextureRef*)texMan->getRefFromFilename("RAY");
	if(texRef)
	{
		texRef->updateFromImage(&renderFrame);
		render->setTextureUWrapMode(M_WRAP_CLAMP);
		render->setTextureVWrapMode(M_WRAP_CLAMP);
		render->setTextureFilterMode(M_TEX_FILTER_NEAREST, M_TEX_FILTER_NEAREST);
	}
	*/
}

void MStandardRenderer::enableFog(MOCamera * camera)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	float fogMin = camera->getClippingFar()*0.9999f;
	if(camera->hasFog())
	{
		render->enableFog();
		float camFogMin = camera->getClippingFar() - camera->getFogDistance();
		if(camFogMin < fogMin)
			fogMin = camFogMin;
	}
	else {
		render->disableFog();
	}

	render->setFogColor(camera->getClearColor());
	render->setFogDistance(fogMin, camera->getClippingFar());
}

MShadowLight * MStandardRenderer::createShadowLight(MOLight * light)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	unsigned int shadowQuality = light->getShadowQuality();

	map<unsigned long, MShadowLight>::iterator iter = m_shadowLights.find((unsigned long)light);
	if(iter != m_shadowLights.end())
	{
		MShadowLight * shadowLight = &iter->second;
		shadowLight->score = 1;

		if(shadowLight->shadowQuality != shadowQuality)
		{
			shadowLight->shadowQuality = shadowQuality;

			render->bindTexture(shadowLight->shadowTexture);
			render->texImage(0, shadowQuality, shadowQuality, M_UBYTE, M_DEPTH, 0);
			render->bindTexture(0);
		}

		return shadowLight;
	}
	else
	{
		m_shadowLights[(unsigned long)(light)] = MShadowLight();
		MShadowLight * shadowLight = &m_shadowLights[(unsigned long)(light)];
		shadowLight->score = 1;
		shadowLight->shadowQuality = shadowQuality;

		render->createTexture(&shadowLight->shadowTexture);
		render->bindTexture(shadowLight->shadowTexture);
		render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
		render->setTextureUWrapMode(M_WRAP_CLAMP);
		render->setTextureVWrapMode(M_WRAP_CLAMP);
		render->texImage(0, shadowQuality, shadowQuality, M_UBYTE, M_DEPTH, 0);
		render->bindTexture(0);

		return shadowLight;
	}
}

void MStandardRenderer::destroyUnusedShadowLights(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// keys
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());

	for(;mit!=mend;++mit)
	{
		MShadowLight * shadowLight = &mit->second;
		if(shadowLight->score < 1)
		{
			render->deleteTexture(&shadowLight->shadowTexture);
			m_shadowLights.erase(mit);
			mit = m_shadowLights.begin();
			mend = m_shadowLights.end();
			if(mit == mend)
				return;
		}
	}
}

void MStandardRenderer::decreaseShadowLights(void)
{
	// keys
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());

	for(;mit!=mend;++mit)
	{
		MShadowLight * shadowLight = &mit->second;
		shadowLight->score--;
	}
}

void MStandardRenderer::drawText(MOText * textObj)
{
	MRenderingContext * render = MEngine().getInstance()->getRenderingContext();


	MFont * font = textObj->getFont();
	const char * text = textObj->getText();
	vector <float> * linesOffset = textObj->getLinesOffset();

	if(! (strlen(text) > 0 && font))
		return;

	if(linesOffset->size() == 0)
		return;


	int id = 0;
	int vertAttribIndex;
	int texAttribIndex;
	unsigned int fxId;
	MVector2 vertices[4];
	MVector2 texCoords[4];
	MMatrix4x4 ProjModelViewMatrix;
	

	// Matrix
	if(m_currentCamera)
	{
		MMatrix4x4 * cameraProjMatrix = m_currentCamera->getCurrentProjMatrix();
		ProjModelViewMatrix = (*cameraProjMatrix) * m_currModelViewMatrix;
	}
	else
	{
		MMatrix4x4 cameraProjMatrix, modelViewMatrix;
		render->getProjectionMatrix(&cameraProjMatrix);
		render->getModelViewMatrix(&modelViewMatrix);
		ProjModelViewMatrix = cameraProjMatrix * modelViewMatrix;
	}


	// cull face
	render->disableCullFace();
	render->setDepthMask(0);
	
	// blending
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);


	// bind FX
	fxId = m_FXs[8];
	render->bindFX(fxId);


	// ProjModelViewMatrix
	render->sendUniformMatrix(fxId, "ProjModelViewMatrix", &ProjModelViewMatrix);

	// Texture0
	render->sendUniformInt(fxId, "Texture0", &id);

	// Color
	render->sendUniformVec4(fxId, "Color", textObj->getColor());

	// Vertex
	render->getAttribLocation(fxId, "Vertex", &vertAttribIndex);
	if(vertAttribIndex != -1)
	{
		render->setAttribPointer(vertAttribIndex, M_FLOAT, 2, vertices);
		render->enableAttribArray(vertAttribIndex);
	}

	// TexCoord
	render->getAttribLocation(fxId, "TexCoord", &texAttribIndex);
	if(texAttribIndex != -1)
	{
		render->setAttribPointer(texAttribIndex, M_FLOAT, 2, texCoords);
		render->enableAttribArray(texAttribIndex);
	}


	// bind texture
	render->bindTexture(font->getTextureId());

	unsigned int lineId = 0;
	float lineOffset = (*linesOffset)[0];

	float size = textObj->getSize();
	float tabSize = size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;
	float xc = 0, yc = 0;

	unsigned int charCode;
	unsigned int state = 0;
	unsigned char* s = (unsigned char*)text;	
	for(; *s; ++s)
	{
		if(utf8_decode(&state, &charCode, *s) != UTF8_ACCEPT)
			continue;
	
		if(charCode == '\n') // return
		{
			lineId++;
			lineOffset = (*linesOffset)[lineId];

			yc += size;
			xc = 0;
			continue;
		}

		if(charCode == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			continue;
		}

		// get character
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
			continue;

		MVector2 pos = character->getPos();
		MVector2 scale = character->getScale();
		MVector2 offset = character->getOffset() * size + MVector2(lineOffset, 0);

		float width = scale.x * widthFactor * size;
		float height = scale.y * heightFactor * size;

		// construct quad
		texCoords[0] = MVector2(pos.x, (pos.y + scale.y));
		vertices[0] = MVector2(xc, (yc + height)) + offset;

		texCoords[1] = MVector2((pos.x + scale.x), (pos.y + scale.y));
		vertices[1] = MVector2((xc + width), (yc + height)) + offset;

		texCoords[3] = MVector2((pos.x + scale.x), pos.y);
		vertices[3] = MVector2((xc + width), yc) + offset;

		texCoords[2] = MVector2(pos.x, pos.y);
		vertices[2] = MVector2(xc, yc) + offset;

		// draw quad
		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

		//move to next character
		xc += character->getXAdvance() * size;
	}

	// disable attribs
	if(vertAttribIndex != -1)
		render->disableAttribArray(vertAttribIndex);
	if(texAttribIndex != -1)
		render->disableAttribArray(texAttribIndex);

	// release FX
	render->bindFX(0);
	render->setDepthMask(1);
}

void MStandardRenderer::prepareSubMesh(MScene * scene, MOCamera * camera, MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MMesh * mesh = entity->getMesh();
	MVector3 scale = entity->getTransformedScale();
	MBox3d * box = subMesh->getBoundingBox();
	if(subMeshCache)
		box = subMeshCache->getBoundingBox();

	// subMesh center
	MVector3 center = box->min + (box->max - box->min)*0.5f;
	center = entity->getTransformedVector(center);

	// entity min scale
	float minScale = scale.x;
	minScale = MIN(minScale, scale.y);
	minScale = MIN(minScale, scale.z);
	minScale = 1.0f / minScale;

	// lights
	unsigned int l;
	unsigned int lSize = scene->getLightsNumber();
	unsigned int lightsNumber = 0;
	for(l=0; l<lSize; l++)
	{
		MOLight * light = scene->getLightByIndex(l);

		if(! light->isActive())
			continue;

		if(! light->isVisible())
			continue;

		if(light->getRadius() <= 0.0f)
			continue;

		// light box
		MVector3 lightPos = light->getTransformedPosition();
		MVector3 localPos = entity->getInversePosition(lightPos);

		float localRadius = light->getRadius() * minScale;

		MBox3d lightBox(
			MVector3(localPos - localRadius),
			MVector3(localPos + localRadius)
		);

		if(! box->isInCollisionWith(lightBox))
			continue;

		MEntityLight * entityLight = &m_entityLights[lightsNumber];
		entityLight->lightBox = lightBox;
		entityLight->light = light;

		m_entityLightsList[lightsNumber] = lightsNumber;

		float z = (center - light->getTransformedPosition()).getLength();
		m_entityLightsZList[lightsNumber] = (1.0f/z)*light->getRadius();

		lightsNumber++;
		if(lightsNumber == MAX_ENTITY_LIGHTS)
			break;
	}

	// sort lights
	if(lightsNumber > 1)
		sortFloatIndexList(m_entityLightsList, m_entityLightsZList, 0, (int)lightsNumber-1);


	// animate textures
	if(mesh->getTexturesAnim())
		animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

	// animate materials
	if(mesh->getMaterialsAnim())
		animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());


	// local lights
	if(lightsNumber > 4)
		lightsNumber = 4;

	for(l=0; l<lightsNumber; l++)
	{
		MEntityLight * entityLight = &m_entityLights[m_entityLightsList[l]];
		MOLight * light = entityLight->light;

		// attenuation
		float quadraticAttenuation = (8.0f / light->getRadius());
		quadraticAttenuation = (quadraticAttenuation*quadraticAttenuation)*light->getIntensity();

		// color
		MVector3 color = light->getFinalColor();

		// set light
		MVector4 lightPos;
		if(light->getLightType() == M_LIGHT_DIRECTIONAL)
		{
			MVector3 lightDir = light->getRotatedVector(MVector3(0, 0, 1)).getNormalized();
			lightPos.x = lightDir.x;
			lightPos.y = lightDir.y;
			lightPos.z = lightDir.z;
			lightPos.w = 0;
		}
		else
		{
			lightPos = light->getTransformedPosition();
		}
		
		render->enableLight(l);
		render->setLightPosition(l, lightPos);
		render->setLightDiffuse(l, MVector4(color));
		render->setLightSpecular(l, MVector4(color));
		render->setLightAmbient(l, MVector3(0, 0, 0));
		render->setLightAttenuation(l, 1, 0, quadraticAttenuation);

		// spot
		render->setLightSpotAngle(l, light->getSpotAngle());
		if(light->getSpotAngle() < 90){
			render->setLightSpotDirection(l, light->getRotatedVector(MVector3(0, 0, -1)).getNormalized());
			render->setLightSpotExponent(l, light->getSpotExponent());
		}
		else {
			render->setLightSpotExponent(l, 0.0f);
		}

		// shadow
		if(light->isCastingShadow())
		{
			MShadowLight * shadowLight = &m_shadowLights[(unsigned long)(light)];
			m_lightShadow[l] = 1;
			m_lightShadowBias[l] = light->getShadowBias()*shadowLight->biasUnity;
			m_lightShadowBlur[l] = light->getShadowBlur();
			m_lightShadowTexture[l] = (int)shadowLight->shadowTexture;
			m_lightShadowMatrix[l] = shadowLight->shadowMatrix * (*entity->getMatrix());
		}
		else{
			m_lightShadow[l] = 0;
		}
	}

	for(l=lightsNumber; l<4; l++){
		render->setLightDiffuse(l, MVector4(0, 0, 0, 0));
		render->disableLight(l);
		m_lightShadow[l] = 0;
	}
}

void MStandardRenderer::drawScene(MScene * scene, MOCamera * camera)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();


	// current view
	int * currentViewport = camera->getCurrentViewport();
	MMatrix4x4 currentViewMatrix = *camera->getCurrentViewMatrix();
	MMatrix4x4 currentProjMatrix = *camera->getCurrentProjMatrix();

	// current render buffer
	unsigned int currentFrameBuffer = 0;
	render->getCurrentFrameBuffer(&currentFrameBuffer);

	// init
	render->setAlphaTest(0);
	render->disableVertexArray();
	render->disableTexCoordArray();
	render->disableNormalArray();
	render->disableColorArray();
	render->disableLighting();
	render->disableBlending();
	
	
	// destroy unused shadowLights
	destroyUnusedShadowLights();

	// decrease shadowLights score
	decreaseShadowLights();


	// lights
	unsigned int l;
	unsigned int lSize = scene->getLightsNumber();


	// camera
	camera->getFrustum()->makeVolume(camera);
	
	// compute lights visibility
	for(l=0; l<lSize; l++)
	{
		MOLight * light = scene->getLightByIndex(l);
		if(light->isActive())
			light->updateVisibility(camera);
	}

	// create frame buffer (TODO: only if minimum one shadow light)
	if(m_fboId == 0)
	{
		render->createFrameBuffer(&m_fboId);
		render->bindFrameBuffer(m_fboId);
		render->setDrawingBuffers(NULL, 0);
		render->bindFrameBuffer(currentFrameBuffer);
	}
	

	// update visibility
	updateVisibility(scene, camera); // TODO: don't need to test light vis again

	// get camera frustum
	MFrustum * frustum = camera->getFrustum();

	// opaque/transp number
	unsigned int opaqueNumber = 0;
	unsigned int transpNumber = 0;

	m_currentCamera = camera;

	// entities
	unsigned int i;
	unsigned int eSize = scene->getEntitiesNumber();


	// make opaque and transp list
	for(i=0; i<eSize; i++)
	{
		// get entity
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();

		if(! entity->isActive())
			continue;

		if(! entity->isVisible())
			continue;

		if(mesh)
		{
			unsigned int s;
			unsigned int sSize = mesh->getSubMeshsNumber();
			for(s=0; s<sSize; s++)
			{
				MSubMesh * subMesh = &mesh->getSubMeshs()[s];
				MBox3d * box = subMesh->getBoundingBox();

				// cache
				if(entity->getSubMeshCachesNumber() == sSize)
					box = entity->getSubMeshCaches()[s].getBoundingBox();

				// check if submesh visible
				if(sSize > 1)
				{
					MVector3 * min = &box->min;
					MVector3 * max = &box->max;

					MVector3 points[8] = {
						entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, max->z))
					};

					if(! frustum->isPointCloudVisible(points, 8))
						continue;
				}

				// subMesh center
				MVector3 center = box->min + (box->max - box->min)*0.5f;
				center = entity->getTransformedVector(center);

				// z distance
				float z = getDistanceToCam(camera, center);

				// transparent
				if(hasSubMeshTransparency(entity, subMesh))
				{
					if(transpNumber < MAX_TRANSP)
					{
						// transparent subMesh pass
						MSubMeshPass * subMeshPass = &m_transpList[transpNumber];

						// set values
						m_transpSortList[transpNumber] = transpNumber;
						m_transpSortZList[transpNumber] = z;
						subMeshPass->subMeshId = s;
						subMeshPass->object = entity;
						//if(subMeshPass->occlusionQuery == 0)
						//	render->createQuery(&subMeshPass->occlusionQuery);
						transpNumber++;
					}
				}
				// opaque
				else
				{
					if(opaqueNumber < MAX_OPAQUE)
					{
						// opaque subMesh pass
						MSubMeshPass * subMeshPass = &m_opaqueList[opaqueNumber];

						// set values
						m_opaqueSortList[opaqueNumber] = opaqueNumber;
						m_opaqueSortZList[opaqueNumber] = z;
						subMeshPass->subMeshId = s;
						subMeshPass->object = entity;
						//if(subMeshPass->occlusionQuery == 0)
						//	render->createQuery(&subMeshPass->occlusionQuery);
						opaqueNumber++;
					}
				}

			}
		}
	}


	// Z pre-pass (run this first to let the queries time to process during shadowmaps drawing ?)
	// problem: query latency is too high
	m_forceNoFX = true;
	{
		if(opaqueNumber > 1)
			sortFloatIndexList(m_opaqueSortList, m_opaqueSortZList, 0, (int)opaqueNumber-1);

		render->setDepthMode(M_DEPTH_LEQUAL);
		render->setColorMask(0, 0, 0, 0);

		for(int s=(int)opaqueNumber-1; s>=0; s--)
		{
			MSubMeshPass * subMeshPass = &m_opaqueList[m_opaqueSortList[s]];
			
			MOEntity * entity = (MOEntity *)subMeshPass->object;
			MMesh * mesh = entity->getMesh();
			MSubMesh * subMesh = &mesh->getSubMeshs()[subMeshPass->subMeshId];

			// animate textures
			if(mesh->getTexturesAnim())
				animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

			// animate materials
			if(mesh->getMaterialsAnim())
				animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

			m_currModelViewMatrix = currentViewMatrix * (*entity->getMatrix());
			
			// cache
			MSubMeshCache * subMeshCache = NULL;
			if(entity->getSubMeshCachesNumber() == mesh->getSubMeshsNumber())
				subMeshCache = &entity->getSubMeshCaches()[subMeshPass->subMeshId];
							
			// draw opaques
			//render->beginQuery(subMeshPass->occlusionQuery);
			drawOpaques(entity, subMeshCache, subMesh, mesh->getArmature());
			//render->endQuery();
		}
	}



	// add texts to transp list
	unsigned int tSize = scene->getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MOText * text = scene->getTextByIndex(i);
		if(text->isActive() && text->isVisible() && transpNumber < MAX_TRANSP)
		{
			// transparent pass
			MSubMeshPass * subMeshPass = &m_transpList[transpNumber];

			// center
			MBox3d * box = text->getBoundingBox();
			MVector3 center = box->min + (box->max - box->min)*0.5f;
			center = text->getTransformedVector(center);

			// z distance to camera
			float z = getDistanceToCam(camera, center);

			// set values
			m_transpSortList[transpNumber] = transpNumber;
			m_transpSortZList[transpNumber] = z;
			subMeshPass->object = text;

			transpNumber++;
		}
	}



	bool restoreCamera = false;
	for(l=0; l<lSize; l++)
	{
		MOLight * light = scene->getLightByIndex(l);
		if(! (light->isActive() && light->isVisible() && light->isCastingShadow()))
			continue;

		MOCamera lightCamera;
		unsigned int shadowQuality = light->getShadowQuality();
		MShadowLight * shadowLight = createShadowLight(light);

		// no need to update shadow (spot only)
		if(light->getSpotAngle() < 90.0f && shadowLight->currentFrame == scene->getCurrentFrame())
			continue;

		// bind FBO
		render->bindFrameBuffer(m_fboId);
		render->attachFrameBufferTexture(M_ATTACH_DEPTH, shadowLight->shadowTexture);

		// disable lights
		{
			for(int i=0; i<4; i++){
				render->setLightDiffuse(i, MVector4(0, 0, 0, 0));
				render->disableLight(i);
				m_lightShadow[i] = 0;
			}
		}
		
		// directional
		if(light->getLightType() == M_LIGHT_DIRECTIONAL)
		{
			MOCamera cameraCopy(*camera);
			cameraCopy.setClippingNear(1);
			cameraCopy.setClippingFar(light->getRadius());
			cameraCopy.updateProjMatrix();
			cameraCopy.getFrustum()->makeVolume(&cameraCopy);
	
			
			// camera bounding box
			MMatrix4x4 lightInverseMatrix = light->getMatrix()->getInverse();
			MBox3d box = cameraCopy.createMatrixOrientedBoundingBox(&lightInverseMatrix);
			
			// setup light camera
			lightCamera.enableOrtho(true);
			lightCamera.setClippingNear(1);
			lightCamera.setClippingFar(box.max.z-box.min.z);
			lightCamera.setFov(MAX(box.max.x - box.min.x, box.max.y - box.min.y));
			
			MVector3 lightPos = light->getTransformedVector(MVector3(
				box.min.x + (box.max.x - box.min.x)*0.5f,
				box.min.y + (box.max.y - box.min.y)*0.5f,
				box.max.z + 1
			));
			
			lightCamera.setPosition(lightPos);
			lightCamera.setEulerRotation(light->getTransformedRotation());
			lightCamera.updateMatrix();
			lightCamera.updateViewMatrix();
			lightCamera.updateProjMatrix();
			
			
			/*
			// test LIGHT SPACE PERSPECTIVE SHADOW MAPS
			MVector3 viewDir = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
			MVector3 lightDir = light->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
			
			calcLispSMMtx(
				cameraCopy.getFrustum()->getPoints(), 8,
				cameraCopy.getTransformedPosition(), viewDir, lightDir, cameraCopy.getClippingNear(),
				lightCamera.getCurrentViewMatrix(), lightCamera.getCurrentProjMatrix()
				);
				
			*lightCamera.getMatrix() = lightCamera.getCurrentViewMatrix()->getInverse();
			*/
		}
		// spot
		else
		{
			*lightCamera.getMatrix() = *light->getMatrix();
			lightCamera.setClippingNear(light->getRadius()*0.001f);
			lightCamera.setClippingFar(light->getRadius());
			lightCamera.setFov(light->getSpotAngle()*2.0f);
		}
	
		
		{
			MVector3 cameraPos = lightCamera.getTransformedPosition();
			MVector3 cameraAxis = lightCamera.getRotatedVector(MVector3(0, 0, -1)).getNormalized();

			render->disableScissorTest();
			render->enableDepthTest();
			render->setViewport(0, 0, shadowQuality, shadowQuality);
			render->getViewport(lightCamera.getCurrentViewport());
			
			if(light->getLightType() != M_LIGHT_DIRECTIONAL)
			{
				lightCamera.updateViewMatrix();
				lightCamera.updateProjMatrix();
			}
			
			lightCamera.enableViewProjMatrix();

			// frustum
			MFrustum * frustum = lightCamera.getFrustum();
			frustum->makeVolume(&lightCamera);

			unsigned int i, eSize = scene->getEntitiesNumber();
			

			/**/float distMin = lightCamera.getClippingFar();
			float distMax = lightCamera.getClippingNear();
			for(i=0; i<eSize; i++)
			{
				MOEntity * entity = scene->getEntityByIndex(i);
				if(entity->isActive())
				{
					if(entity->isInvisible()){
						entity->setVisible(false);
						continue;
					}

					// compute entities visibility
					MBox3d * box = entity->getBoundingBox();
					MVector3 * min = &box->min;
					MVector3 * max = &box->max;

					MVector3 points[8] = {
						entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, max->z))
					};

					entity->setVisible(frustum->isPointCloudVisible(points, 8));

					// adapt clipping
					if(entity->isVisible())
					{
						for(int p=0; p<8; p++)
						{
							float dist = (points[p] - cameraPos).dotProduct(cameraAxis);
							distMin = MIN(distMin, dist);
							distMax = MAX(distMax, dist);
						}
					}
				}
			}

			// sort Zlist and set clipping
			if(light->getLightType() != M_LIGHT_DIRECTIONAL)
			{
				lightCamera.setClippingFar(MIN(lightCamera.getClippingFar(), distMax));
				lightCamera.setClippingNear(MAX(lightCamera.getClippingNear(), distMin));
				lightCamera.updateProjMatrix();
				lightCamera.enableViewProjMatrix();
			}/**/

			m_currentCamera = &lightCamera;

			render->clear(M_BUFFER_DEPTH);
			render->setColorMask(0, 0, 0, 0);
			render->setPolygonOffset(light->getShadowBias(), 0);

			// entities
			for(i=0; i<eSize; i++)
			{
				// get entity
				MOEntity * entity = scene->getEntityByIndex(i);
				MMesh * mesh = entity->getMesh();

				// draw mesh
				if(mesh && entity->isActive() && entity->isVisible())
				{
					// animate textures
					if(mesh->getTexturesAnim())
						animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

					// animate materials
					if(mesh->getMaterialsAnim())
						animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

					unsigned int s;
					unsigned int sSize = mesh->getSubMeshsNumber();
					for(s=0; s<sSize; s++)
					{
						MSubMesh * subMesh = &mesh->getSubMeshs()[s];
						MBox3d * box = subMesh->getBoundingBox();
						
						// cache
						MSubMeshCache * subMeshCache = NULL;
						if(entity->getSubMeshCachesNumber() == sSize)
						{
							subMeshCache = &entity->getSubMeshCaches()[s];
							box = subMeshCache->getBoundingBox();
						}
						
						// check if submesh visible
						if(sSize > 1)
						{
							MVector3 * min = &box->min;
							MVector3 * max = &box->max;

							MVector3 points[8] = {
								entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
								entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
								entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
								entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
								entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
								entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
								entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
								entity->getTransformedVector(MVector3(max->x, min->y, max->z))
							};

							if(! frustum->isPointCloudVisible(points, 8))
								continue;
						}

						m_currModelViewMatrix = (*lightCamera.getCurrentViewMatrix()) * (*entity->getMatrix());

						// draw opaques
						drawOpaques(entity, subMeshCache, subMesh, mesh->getArmature());
					}
				}
			}

			setShadowMatrix(&shadowLight->shadowMatrix, &lightCamera);

			// biasUnity
			{
				MVector4 pt1 = shadowLight->shadowMatrix * MVector4(cameraPos + cameraAxis);
				MVector4 pt2 = shadowLight->shadowMatrix * MVector4(cameraPos + cameraAxis*2.0f);

				shadowLight->biasUnity = (- (pt1.z - pt2.z*0.5f))*0.01f;
				
				if(light->getLightType() == M_LIGHT_DIRECTIONAL)
					shadowLight->biasUnity = 0.01f;
			}

			render->setPolygonOffset(0, 0);
			render->setColorMask(1, 1, 1, 1);
		}
		
		// update shadow light currentFrame
		shadowLight->currentFrame = scene->getCurrentFrame();
		
		render->bindFrameBuffer(currentFrameBuffer);
		restoreCamera = true;
	}

	// restore camera after shadow pass
	if(restoreCamera)
	{
		render->setViewport(currentViewport[0], currentViewport[1], currentViewport[2], currentViewport[3]);

		render->setMatrixMode(M_MATRIX_PROJECTION);
		render->loadIdentity();
		render->multMatrix(&currentProjMatrix);

		render->setMatrixMode(M_MATRIX_MODELVIEW);
		render->loadIdentity();
		render->multMatrix(&currentViewMatrix);
		
		m_currentCamera = camera;
	}
	

	// fog
	enableFog(camera);

	// draw opaques
	{
		// render passs
		m_forceNoFX = false;
		render->setColorMask(1, 1, 1, 1);
		render->setDepthMask(0);
		render->setDepthMode(M_DEPTH_EQUAL);

		for(int s=(int)opaqueNumber-1; s>=0; s--)
		{
			MSubMeshPass * subMeshPass = &m_opaqueList[m_opaqueSortList[s]];
			MOEntity * entity = (MOEntity *)subMeshPass->object;
			MMesh * mesh = entity->getMesh();
			MSubMesh * subMesh = &mesh->getSubMeshs()[subMeshPass->subMeshId];

			// cache
			MSubMeshCache * subMeshCache = NULL;
			if(entity->getSubMeshCachesNumber() == mesh->getSubMeshsNumber())
				subMeshCache = &entity->getSubMeshCaches()[subMeshPass->subMeshId];

			// read occlusion result
			/*unsigned int queryResult = 1;
			if(render->isQueryResultAvailable(subMeshPass->occlusionQuery))
				render->getQueryResult(subMeshPass->occlusionQuery, &queryResult);
				
			if(queryResult > 0)*/
			{
				prepareSubMesh(scene, camera, entity, subMeshCache, subMesh);
				m_currModelViewMatrix = currentViewMatrix * (*entity->getMatrix());
				drawOpaques(entity, subMeshCache, subMesh, mesh->getArmature());
			}
		}

		render->setDepthMask(1);
		render->setDepthMode(M_DEPTH_LEQUAL);
	}


	// draw transparent
	{
		render->enableBlending();
		render->setDepthMask(0);
		
		if(transpNumber > 1)
			sortFloatIndexList(m_transpSortList, m_transpSortZList, 0, (int)transpNumber-1);

		for(int s=0; s<transpNumber; s++)
		{
			MSubMeshPass * subMeshPass = &m_transpList[m_transpSortList[s]];
			MObject3d * object = subMeshPass->object;

			// objects
			switch(object->getType())
			{
				case M_OBJECT3D_ENTITY:
				{
					MOEntity * entity = (MOEntity *)object;
					MMesh * mesh = entity->getMesh();
					MSubMesh * subMesh = &mesh->getSubMeshs()[subMeshPass->subMeshId];

					// cache
					MSubMeshCache * subMeshCache = NULL;
					if(entity->getSubMeshCachesNumber() == mesh->getSubMeshsNumber())
						subMeshCache = &entity->getSubMeshCaches()[subMeshPass->subMeshId];
				
					prepareSubMesh(scene, camera, entity, subMeshCache, subMesh);

					m_currModelViewMatrix = currentViewMatrix * (*entity->getMatrix());
					drawTransparents(entity, subMeshCache, subMesh, mesh->getArmature());
					break;
				}

				case M_OBJECT3D_TEXT:
				{
					MOText * text = (MOText *)object;

					m_currModelViewMatrix = currentViewMatrix * (*text->getMatrix());
					drawText(text);
					break;
				}
			}
		}
		
		render->setDepthMask(1);
	}
}
