/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MFixedRenderer.cpp
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


#include <tinyutf8.h>
#include <MEngine.h>
#include "MFixedRenderer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MFixedRenderer::MFixedRenderer(void)
{}

MFixedRenderer::~MFixedRenderer(void)
{}

void MFixedRenderer::destroy(void)
{
	delete this;
}

MRenderer * MFixedRenderer::getNew(void)
{
	return new MFixedRenderer();
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

void MFixedRenderer::drawDisplay(MOEntity * entity, MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MColor * colors)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();


	render->setColor4(MVector4(1, 1, 1, 1));


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
		if(cullMode == M_CULL_NONE){
			render->disableCullFace();
		}
		else{
			render->enableCullFace();
			render->setCullMode(cullMode);
		}

		// texture passes
		unsigned int texturesPassNumber = MIN(8, material->getTexturesPassNumber());

		// set blending mode
		render->setBlendingMode(blendMode);

		// alpha test
		if(blendMode != M_BLENDING_ALPHA && texturesPassNumber > 0)
		{
			MTexturePass * texturePass = material->getTexturePass(0);
			MTexture * texture = texturePass->getTexture();
			if(texture)
			{
				if(texture->getTextureRef()->getComponents() > 3)
					render->setAlphaTest(0.5f);
			}
		}

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

		// fixed pipeline
		{
			// no FX
			render->bindFX(0);

			// Vertex
			render->enableVertexArray();
			render->setVertexPointer(M_FLOAT, 3, vertices);

			// Normal
			if(normals)
			{
				render->enableNormalArray();
				render->setNormalPointer(M_FLOAT, normals);
			}
			else
			{
				render->disableNormalArray();
			}

			// Color
			if(colors)
			{
				render->disableLighting();
				render->enableColorArray();
				render->setColorPointer(M_UBYTE, 4, colors);
			}
			else
			{
				render->disableColorArray();
			}

			// Material
			render->setMaterialDiffuse(MVector4(diffuse.x, diffuse.y, diffuse.z, opacity));
			render->setMaterialSpecular(MVector4(specular));
			render->setMaterialAmbient(MVector4());
			render->setMaterialEmit(MVector4(emit));
			render->setMaterialShininess(shininess);

			// switch to texture matrix mode
			if(texturesPassNumber > 0)
				render->setMatrixMode(M_MATRIX_TEXTURE);
			else
			{
				render->bindTexture(0);
				render->disableTexture();
				render->disableTexCoordArray();
			}

			// Textures
			int id = texturesPassNumber;
			for(unsigned int t=0; t<texturesPassNumber; t++)
			{
				MTexturePass * texturePass = material->getTexturePass(t);

				MTexture * texture = texturePass->getTexture();
				if((! texture) || (! texCoords))
				{
					render->bindTexture(0, t);
					render->disableTexture();
					render->disableTexCoordArray();
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
				render->enableTexture();
				render->setTextureCombineMode(texturePass->getCombineMode());
				render->setTextureUWrapMode(texture->getUWrapMode());
				render->setTextureVWrapMode(texture->getVWrapMode());

				// texture matrix
				render->loadIdentity();
				render->translate(MVector2(0.5f, 0.5f));
				render->scale(texture->getTexScale());
				render->rotate(MVector3(0, 0, -1), texture->getTexRotate());
				render->translate(MVector2(-0.5f, -0.5f));
				render->translate(texture->getTexTranslate());

				// texture coords
				render->enableTexCoordArray();
				render->setTexCoordPointer(M_FLOAT, 2, texCoords + offset);
			}

			// switch back to modelview matrix mode
			if(texturesPassNumber > 0)
				render->setMatrixMode(M_MATRIX_MODELVIEW);

			// draw
			if(indices)
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
			else{
				render->drawArray(primitiveType, begin, size);
			}


			// disable arrays
			render->disableVertexArray();
			render->disableNormalArray();
			render->disableColorArray();

			// restore textures
			for(int t=(int)(id-1); t>=0; t--)
			{
				render->bindTexture(0, t);
				render->disableTexture();
				render->disableTexCoordArray();
				render->setTextureCombineMode(M_TEX_COMBINE_MODULATE);

				render->setMatrixMode(M_MATRIX_TEXTURE);
				render->loadIdentity();
				render->setMatrixMode(M_MATRIX_MODELVIEW);
			}
		}

		// restore fog and alpha test
		render->setFogColor(currentFogColor);
		if(blendMode != M_BLENDING_ALPHA)
			render->setAlphaTest(0.0f);

		// restore lighting
		if(colors)
			render->enableLighting();
	}
}

void MFixedRenderer::drawDisplayTriangles(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// begin / size
	unsigned int begin = display->getBegin();
	unsigned int size = display->getSize();

	// display properties
	M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
	M_CULL_MODES cullMode = display->getCullMode();

	// cull mode
	if(cullMode == M_CULL_NONE){
		render->disableCullFace();
	}
	else{
		render->enableCullFace();
		render->setCullMode(cullMode);
	}

	// indices
	M_TYPES indicesType = subMesh->getIndicesType();
	void * indices = subMesh->getIndices();

	// FX
	render->bindFX(0);

	// Vertex
	render->enableVertexArray();
	render->setVertexPointer(M_FLOAT, 3, vertices);

	// draw
	if(indices)
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
	else
		render->drawArray(primitiveType, begin, size);

	// disable vertex array
	render->disableVertexArray();

	// restore FX
	render->bindFX(0);
}

void MFixedRenderer::drawOpaques(MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh, MArmature * armature)
{
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MColor * colors = subMesh->getColors();

	if(! vertices)
		return;

	if(subMeshCache)
	{
		vertices = subMeshCache->getVertices();
		normals = subMeshCache->getNormals();
	}

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
				drawDisplay(entity, subMesh, display, vertices, normals, colors);
		}
	}
}

void MFixedRenderer::drawTransparents(MOEntity * entity, MSubMeshCache * subMeshCache, MSubMesh * subMesh, MArmature * armature)
{
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MColor * colors = subMesh->getColors();

	if(! vertices)
		return;

	if(subMeshCache)
	{
		vertices = subMeshCache->getVertices();
		normals = subMeshCache->getNormals();
	}

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
				drawDisplay(entity, subMesh, display, vertices, normals, colors);
		}
	}
}

float MFixedRenderer::getDistanceToCam(MOCamera * camera, const MVector3 & pos)
{
	if(! camera->isOrtho())
	{
		return (pos - camera->getTransformedPosition()).getSquaredLength();
	}

	MVector3 axis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	float dist = (pos - camera->getTransformedPosition()).dotProduct(axis);
	return dist*dist;
}

void MFixedRenderer::updateVisibility(MScene * scene, MOCamera * camera)
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
}

void MFixedRenderer::enableFog(MOCamera * camera)
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

void MFixedRenderer::drawText(MOText * textObj)
{
	MFont * font = textObj->getFont();
	const char * text = textObj->getText();
	vector <float> * linesOffset = textObj->getLinesOffset();

	if(! (strlen(text) > 0 && font))
		return;

	if(linesOffset->size() == 0)
		return;

	MRenderingContext * render = MEngine().getInstance()->getRenderingContext();


	render->enableBlending();
	render->enableTexture();
	render->disableLighting();
	render->disableCullFace();
	render->setDepthMask(0);

	render->setColor4(textObj->getColor());
	render->setBlendingMode(M_BLENDING_ALPHA);

	static MVector2 vertices[4];
	static MVector2 texCoords[4];

	render->disableNormalArray();
	render->disableColorArray();
	render->enableVertexArray();
	render->enableTexCoordArray();

	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->setTexCoordPointer(M_FLOAT, 2, texCoords);

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
	
	render->setDepthMask(1);
}

void MFixedRenderer::drawScene(MScene * scene, MOCamera * camera)
{
	struct MEntityLight
	{
		MBox3d lightBox;
		MOLight * light;
	};

	struct MSubMeshPass
	{
		unsigned int subMeshId;
		unsigned int lightsNumber;
		MObject3d * object;
		MOLight * lights[4];
	};

	// sub objects
	#define MAX_TRANSP_SUBOBJ 4096
	static int transpList[MAX_TRANSP_SUBOBJ];
	static float transpZList[MAX_TRANSP_SUBOBJ];
	static MSubMeshPass transpSubObjs[MAX_TRANSP_SUBOBJ];

	// lights list
	#define MAX_ENTITY_LIGHTS 256
	static int entityLightsList[MAX_ENTITY_LIGHTS];
	static float entityLightsZList[MAX_ENTITY_LIGHTS];
	static MEntityLight entityLights[MAX_ENTITY_LIGHTS];


	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	render->enableLighting();
	render->enableBlending();


	// make frustum
	MFrustum * frustum = camera->getFrustum();
	frustum->makeVolume(camera);

	// update visibility
	updateVisibility(scene, camera);

	// fog
	enableFog(camera);


	// transp sub obj number
	unsigned int transpSubObsNumber = 0;

	// lights
	unsigned int l;
	unsigned int lSize = scene->getLightsNumber();

	// entities
	unsigned int i;
	unsigned int eSize = scene->getEntitiesNumber();
	for(i=0; i<eSize; i++)
	{
		// get entity
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();

		if(! entity->isActive())
			continue;

		if(! entity->isVisible())
			continue;

		// draw mesh
		if(mesh)
		{
			MVector3 scale = entity->getTransformedScale();
			MBox3d * entityBox = entity->getBoundingBox();

			float minScale = scale.x;
			minScale = MIN(minScale, scale.y);
			minScale = MIN(minScale, scale.z);
			minScale = 1.0f / minScale;

			unsigned int entityLightsNumber = 0;
			for(l=0; l<lSize; l++)
			{
				// get entity
				MOLight * light = scene->getLightByIndex(l);

				if(! light->isActive())
					continue;

				if(! light->isVisible())
					continue;

				// light box
				MVector3 lightPos = light->getTransformedPosition();
				MVector3 localPos = entity->getInversePosition(lightPos);

				float localRadius = light->getRadius() * minScale;

				MBox3d lightBox(
								MVector3(localPos - localRadius),
								MVector3(localPos + localRadius)
								);

				if(! entityBox->isInCollisionWith(lightBox))
					continue;

				MEntityLight * entityLight = &entityLights[entityLightsNumber];
				entityLight->lightBox = lightBox;
				entityLight->light = light;

				entityLightsNumber++;
				if(entityLightsNumber == MAX_ENTITY_LIGHTS)
					break;
			}

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

				// subMesh center
				MVector3 center = box->min + (box->max - box->min)*0.5f;
				center = entity->getTransformedVector(center);

				// sort entity lights
				unsigned int lightsNumber = 0;
				for(l=0; l<entityLightsNumber; l++)
				{
					MEntityLight * entityLight = &entityLights[l];
					if(! box->isInCollisionWith(entityLight->lightBox))
						continue;

					MOLight * light = entityLight->light;

					float z = (center - light->getTransformedPosition()).getLength();
					entityLightsList[lightsNumber] = l;
					entityLightsZList[l] = (1.0f/z)*light->getRadius();
					lightsNumber++;
				}

				if(lightsNumber > 1)
					sortFloatIndexList(entityLightsList, entityLightsZList, 0, (int)lightsNumber-1);

				// local lights
				if(lightsNumber > 4)
					lightsNumber = 4;

				for(l=0; l<lightsNumber; l++)
				{
					MEntityLight * entityLight = &entityLights[entityLightsList[l]];
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
				}

				for(l=lightsNumber; l<4; l++){
					render->disableLight(l);
				}

				render->pushMatrix();
				render->multMatrix(entity->getMatrix());

				// draw opaques
				drawOpaques(entity, subMeshCache, subMesh, mesh->getArmature());

				if(hasSubMeshTransparency(entity, subMesh))
				{
					if(transpSubObsNumber < MAX_TRANSP_SUBOBJ)
					{
						// transparent subMesh pass
						MSubMeshPass * subMeshPass = &transpSubObjs[transpSubObsNumber];

						// lights
						subMeshPass->lightsNumber = lightsNumber;
						for(l=0; l<lightsNumber; l++)
							subMeshPass->lights[l] = entityLights[entityLightsList[l]].light;

						// z distance to camera
						float z = getDistanceToCam(camera, center);

						// set values
						transpList[transpSubObsNumber] = transpSubObsNumber;
						transpZList[transpSubObsNumber] = z;
						subMeshPass->subMeshId = s;
						subMeshPass->object = entity;

						transpSubObsNumber++;
					}
				}

				render->popMatrix();
			}
		}
	}


	// texts
	unsigned int tSize = scene->getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MOText * text = scene->getTextByIndex(i);
		if(text->isActive() && text->isVisible())
		{
			// transparent pass
			MSubMeshPass * subMeshPass = &transpSubObjs[transpSubObsNumber];

			// center
			MBox3d * box = text->getBoundingBox();
			MVector3 center = box->min + (box->max - box->min)*0.5f;
			center = text->getTransformedVector(center);

			// z distance to camera
			float z = getDistanceToCam(camera, center);

			// set values
			transpList[transpSubObsNumber] = transpSubObsNumber;
			transpZList[transpSubObsNumber] = z;
			subMeshPass->object = text;

			transpSubObsNumber++;
		}
	}


	// sort transparent list
	if(transpSubObsNumber > 1)
		sortFloatIndexList(transpList, transpZList, 0, (int)transpSubObsNumber-1);

	// draw transparents
	render->setDepthMask(0);
	
	for(i=0; i<transpSubObsNumber; i++)
	{
		MSubMeshPass * subMeshPass = &transpSubObjs[transpList[i]];
		MObject3d * object = subMeshPass->object;

		// objects
		switch(object->getType())
		{
			case M_OBJECT3D_ENTITY:
			{
				MOEntity * entity = (MOEntity *)object;
				unsigned int subMeshId = subMeshPass->subMeshId;
				MMesh * mesh = entity->getMesh();
				MSubMesh * subMesh = &mesh->getSubMeshs()[subMeshId];

				// animate textures
				if(mesh->getTexturesAnim())
					animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

				// animate materials
				if(mesh->getMaterialsAnim())
					animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

				// lights
				for(l=0; l<subMeshPass->lightsNumber; l++)
				{
					MOLight * light = subMeshPass->lights[l];

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
				}

				for(l=subMeshPass->lightsNumber; l<4; l++){
					render->disableLight(l);
				}
				
				// cache
				MSubMeshCache * subMeshCache = NULL;
				if(entity->getSubMeshCachesNumber() == mesh->getSubMeshsNumber())
					subMeshCache = &entity->getSubMeshCaches()[subMeshPass->subMeshId];

				render->pushMatrix();
				render->multMatrix(entity->getMatrix());
				drawTransparents(entity, subMeshCache, subMesh, mesh->getArmature());
				render->popMatrix();
			}
				break;

			case M_OBJECT3D_TEXT:
			{
				MOText * text = (MOText *)object;

				render->pushMatrix();
				render->multMatrix(text->getMatrix());
				drawText(text);
				render->popMatrix();
			}
				break;
		}
	}
	
	render->setDepthMask(1);

	render->disableLighting();
	render->disableFog();
}
