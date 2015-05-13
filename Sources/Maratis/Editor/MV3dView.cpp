/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MV3dView.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#include "MEditor.h"
#include "MRenderArray.h"


static MVector4 getObjectColor(MObject3d * object, bool isSelected)
{
	MEditor * editor = MEditor::getInstance();
	MPreferences * prefs = editor->getPreferences();
	
	if(isSelected)
	{
		return prefs->getColor("3d Object Highlight");
	}
	else
	{
		if(object->isActive())
			return prefs->getColor("3d Object On");
		else
			return prefs->getColor("3d Object Off");
	}
}

MV3dView::MV3dView(void):
m_window(NULL),
m_unitSize(0)
{}

MV3dView::~MV3dView(void)
{}

void MV3dView::drawCallback(MGuiWindow * window)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MWindow * rootWindow = window->getRootWindow();
	
	MRenderingContext * render = engine->getRenderingContext();
	MPreferences * prefs = editor->getPreferences();
	MSelectionManager * selection = editor->getSelectionManager();
	
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();
	MLevel * guiData = editor->getGuiData();
	
	MV3dView * viewport = (MV3dView *)window->getUserPointer();
	MOCamera * camera = &viewport->m_view.m_camera;
	
	
	// viewport
	render->setViewport(
		window->getPosition().x, (float)rootWindow->getHeight() - window->getScale().y - window->getPosition().y,
		window->getScale().x, window->getScale().y
	);
	
	// clear
	camera->setClearColor((MVector3)prefs->getColor("3d Background"));
	render->setClearColor(camera->getClearColor());
	render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

	// camera
	camera->enable();
	camera->updateListener();
	
	// draw scene
	viewport->drawGrid();
	scene->draw(camera);
	scene->drawObjectsBehaviors();
	
	// unit size
	viewport->m_unitSize = 10.0f / (float)viewport->m_view.m_camera.getCurrentViewport()[3];
	
	// draw extra
	MMeshRef * lightMeshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("light.mesh");
	MMeshRef * cameraMeshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("camera.mesh");
	MMeshRef * soundMeshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("sound.mesh");
	
	render->disableLighting();
	render->disableTexture();
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);
	
	// lights
	if(lightMeshRef)
	{
		unsigned int i, size = scene->getLightsNumber();
		for(i=0; i<size; i++)
		{
			MOLight * light = scene->getLightByIndex(i);
			viewport->drawLight(light, lightMeshRef, viewport->m_unitSize);
		}
	}
	
	// cameras
	if(cameraMeshRef)
	{
		unsigned int i, size = scene->getCamerasNumber();
		for(i=0; i<size; i++)
		{
			MOCamera * camera = scene->getCameraByIndex(i);
			viewport->drawCamera(camera, cameraMeshRef, viewport->m_unitSize);
		}
	}
	
	// sounds
	if(soundMeshRef)
	{
		unsigned int i, size = scene->getSoundsNumber();
		for(i=0; i<size; i++)
		{
			MOSound * sound = scene->getSoundByIndex(i);
			viewport->drawBillboardObject(sound, soundMeshRef, viewport->m_unitSize);
		}
	}
	
	// entities
	{
		//render->disableDepthTest();
	
		unsigned int i, size = scene->getEntitiesNumber();
		for(i=0; i<size; i++)
		{
			MOEntity * entity = scene->getEntityByIndex(i);
			if(selection->isObjectSelected(entity))
			{
				//if(entity->isActive() && !entity->isInvisible())
					//viewport->drawArmature(entity);
					
				MVector4 color = getObjectColor(entity, 1);
				render->setColor4(color);
				
				render->pushMatrix();
				render->multMatrix(entity->getMatrix());
				viewport->drawBoundingBox(entity->getBoundingBox());
				render->popMatrix();
			}
		}
		
		//render->enableDepthTest();
	}
}

void MV3dView::drawGrid(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	render->disableFog();
	render->enableDepthTest();
	render->disableTexture();
	render->disableBlending();

	// grid
	unsigned int i;
	unsigned int gridSpace = 10;
	unsigned int gridNumber = 4;
	unsigned int gSize = gridNumber*10;
	float gLength = (float)(gSize*gridSpace);

	// colors
	MVector3 clearColor = m_view.m_camera.getClearColor();
	MVector3 color1 = (clearColor * 0.8f);
	MVector3 color2 = (clearColor * 0.9f);
	MVector3 red =   (clearColor * 0.7f) + (MVector3(1, 0, 0) * 0.3f);
	MVector3 green = (clearColor * 0.7f) + (MVector3(0, 1, 0) * 0.3f);

	// draw lines
	render->setColor3(color2);
	beginDraw(M_PRIMITIVE_LINES);
	for(i=0; i<=gSize; i++)
	{
		float length = (float)(i*gridSpace);

		pushVertex(MVector3(-length, -gLength, 0));
		pushVertex(MVector3(-length,  gLength, 0));

		pushVertex(MVector3( length, -gLength, 0));
		pushVertex(MVector3( length,  gLength, 0));

		pushVertex(MVector3(-gLength, -length, 0));
		pushVertex(MVector3( gLength, -length, 0));

		pushVertex(MVector3(-gLength, length, 0));
		pushVertex(MVector3( gLength, length, 0));
	}
	endDraw(render);

	render->disableDepthTest();

	render->setColor3(color1);
	beginDraw(M_PRIMITIVE_LINES);
	for(i=0; i<=gSize; i+=gridSpace)
	{
		float length = (float)(i*gridSpace);

		pushVertex(MVector3(-length, -gLength, 0));
		pushVertex(MVector3(-length,  gLength, 0));

		pushVertex(MVector3( length, -gLength, 0));
		pushVertex(MVector3( length,  gLength, 0));

		pushVertex(MVector3(-gLength, -length, 0));
		pushVertex(MVector3( gLength, -length, 0));

		pushVertex(MVector3(-gLength, length, 0));
		pushVertex(MVector3( gLength, length, 0));
	}
	endDraw(render);

	// axis
	render->setColor3(red);
	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(-gLength, 0, 0));
	pushVertex(MVector3( gLength, 0, 0));
	endDraw(render);

	render->setColor3(green);
	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(0, -gLength, 0));
	pushVertex(MVector3(0,  gLength, 0));
	endDraw(render);
	
	render->enableDepthTest();
}

void MV3dView::drawBoundingBox(MBox3d * box)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	MVector3 * min = &box->min;
	MVector3 * max = &box->max;

	beginDraw(M_PRIMITIVE_LINES);

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(min->x, max->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(max->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(max->x, max->y, min->z));

	pushVertex(MVector3(max->x, max->y, min->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(max->x, min->y, max->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, min->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	pushVertex(MVector3(min->x, max->y, max->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(min->x, max->y, max->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(max->x, min->y, max->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	endDraw(render);
}

void MV3dView::drawArmature(MOEntity * entity)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MMesh * mesh = entity->getMesh();
	if(mesh)
	{
		MArmature * armature = mesh->getArmature();
		if(armature)
		{
			MVector3 iVue = m_view.m_camera.getPosition();
			
			mesh->updateArmature(entity->getCurrentFrame());
			beginDraw(M_PRIMITIVE_LINES);
			
			unsigned int b, bSize = armature->getBonesNumber();
			for(b=0; b<bSize; b++)
			{
				MOBone * bone = armature->getBone(b);
				
				MVector3 pos = entity->getTransformedVector(bone->getTransformedPosition());
				float size = (pos - iVue).getLength() * 0.01f;
				
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(-size, 0, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3( size, 0, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, -size, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0,  size, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, 0, -size)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, 0,  size)));
			}
			
			endDraw(render);
		}
	}
}

void MV3dView::drawWireframe(MMesh * mesh)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	
	unsigned int s, sSize = mesh->getSubMeshsNumber();
	for(s=0; s<sSize; s++)
	{
		MSubMesh * subMesh = &mesh->getSubMeshs()[s];
		MVector3 * vertices = subMesh->getVertices();

		if(subMesh->getIndices())
		{
			switch(subMesh->getIndicesType())
			{
				case M_USHORT:
				{
					unsigned short * indices = (unsigned short *)subMesh->getIndices();
				
					beginDraw(M_PRIMITIVE_LINES);
					
					unsigned int i;
					for(i=0; i<subMesh->getIndicesSize(); i+=3)
					{
						pushVertex(vertices[indices[i+0]]);
						pushVertex(vertices[indices[i+1]]);

						pushVertex(vertices[indices[i+1]]);
						pushVertex(vertices[indices[i+2]]);
					}
					
					endDraw(render);
					
					break;
				}
				case M_UINT:
				{
					unsigned int * indices = (unsigned int *)subMesh->getIndices();
				
					beginDraw(M_PRIMITIVE_LINES);
					
					unsigned int i;
					for(i=0; i<subMesh->getIndicesSize(); i+=3)
					{
						pushVertex(vertices[indices[i+0]]);
						pushVertex(vertices[indices[i+1]]);

						pushVertex(vertices[indices[i+1]]);
						pushVertex(vertices[indices[i+2]]);
					}
					
					endDraw(render);
					
					break;
				}
				default:
					break;
			}
		}
	}
}

void MV3dView::drawBillboardObject(MObject3d * object, MMeshRef * meshRef, float unitSize)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	MSelectionManager * selection = editor->getSelectionManager();
	MOCamera * camera = &m_view.m_camera;
	

	bool selected = selection->isObjectSelected(object);
	MVector4 color = getObjectColor(object, selected);
		
	if(! object->isActive())
		object->updateMatrix();

	float size = getBillboardObjectSize(object, unitSize);

	MMatrix4x4 matrix;
	matrix.setScale(object->getTransformedScale() * MVector3(size));
	matrix.setTranslationPart(object->getTransformedPosition());

	MMatrix4x4 camMatrix(*camera->getMatrix());
	camMatrix.setTranslationPart(MVector3(0, 0, 0));
	
	render->setColor4(color);
	render->pushMatrix();

	render->multMatrix(&matrix);
	render->multMatrix(&camMatrix);
	drawWireframe(meshRef->getMesh());
	
	render->popMatrix();
}

void MV3dView::drawLight(MOLight * light, MMeshRef * meshRef, float unitSize)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	MSelectionManager * selection = editor->getSelectionManager();
	MOCamera * camera = &m_view.m_camera;
	

	bool selected = selection->isObjectSelected(light);
	MVector4 color = getObjectColor(light, selected);
	
	// billboard
	drawBillboardObject(light, meshRef, unitSize);

	float spotAngle = light->getSpotAngle();
	
	// directional
	if(light->getLightType() == M_LIGHT_DIRECTIONAL)
	{
		float size = getBillboardObjectSize(light, unitSize);
		
		MVector3 pos = light->getTransformedPosition();
		MVector3 dir = light->getRotatedVector(MVector3(0, 0, -1))*size;
		
		MVector3 cx = camera->getRotatedVector(MVector3(1, 0, 0)).getNormalized();
		MVector3 cy = camera->getRotatedVector(MVector3(0, 1, 0)).getNormalized();
		
		MVector3 ndir = dir.getNormalized();
		float dx = ndir.dotProduct(cx);
		float dy = ndir.dotProduct(cy);
		
		MVector3 cameraAxis = camera->getRotatedVector(MVector3(dy, -dx, 0)).getNormalized()*size;
		
		render->setColor4(color);
		beginDraw(M_PRIMITIVE_LINES);
		pushVertex(pos + dir);
		pushVertex(pos + dir*10);
		pushVertex(pos + dir*9 + cameraAxis*0.5f);
		pushVertex(pos + dir*10);
		pushVertex(pos + dir*9 - cameraAxis*0.5f);
		pushVertex(pos + dir*10);
		endDraw(render);
	}
	// spot
	else if(light->getLightType() == M_LIGHT_SPOT)
	{
		float size = getBillboardObjectSize(light, unitSize)*10;
		
		MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
		
		if(size > light->getRadius()*0.25f)
			size = light->getRadius()*0.25f;

		MVector3 pos = light->getTransformedPosition();
		MVector3 dir1 = light->getRotatedVector(MVector3(0, 0, -1));
		MVector3 dir2 = dir1;

		MVector3 axis = light->getInverseRotatedVector(cameraAxis);
		axis.z = 0;
		axis = light->getRotatedVector(axis);

		MMatrix4x4 matrix;
		matrix.setRotationAxis(spotAngle, axis);
		dir1 = matrix * dir1;

		matrix.loadIdentity();
		matrix.setRotationAxis(-spotAngle, axis);
		dir2 = matrix * dir2;

		dir1.normalize();
		dir2.normalize();

		render->setColor4(color);
		beginDraw(M_PRIMITIVE_LINES);
		pushVertex(pos);
		pushVertex(pos + dir1*size);
		pushVertex(pos);
		pushVertex(pos + dir2*size);
		endDraw(render);

		beginDraw(M_PRIMITIVE_LINE_LOOP);
		for(int x=0; x<360; x+=10)
		{
			MVector3 dir = MVector3(0, 0, -1).getRotatedX(spotAngle);
			dir.rotateZ(x);
			dir = light->getRotatedVector(dir).getNormalized();
			pushVertex(pos + dir*size);
		}
		endDraw(render);
	}
}

void MV3dView::drawCamera(MOCamera * camera, MMeshRef * meshRef, float unitSize)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	MSelectionManager * selection = editor->getSelectionManager();
	
	if(! camera->isActive())
		camera->updateMatrix();

	float size = getBillboardObjectSize(camera, unitSize);
	MMatrix4x4 matrix;
	MMatrix4x4 scaleMatrix;
	scaleMatrix.setScale(MVector3(size));
	matrix = (*camera->getMatrix()) * scaleMatrix;

	bool selected = selection->isObjectSelected(camera);
	MVector4 color = getObjectColor(camera, selected);

	render->setColor4(color);
	render->pushMatrix();
	render->multMatrix(&matrix);

	drawWireframe(meshRef->getMesh());

	render->popMatrix();
}

void MV3dView::drawTriangles(MMesh * mesh)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	// subMeshs
	unsigned int s;
	unsigned sSize = mesh->getSubMeshsNumber();
	for(s=0; s<sSize; s++)
	{
		MSubMesh * subMesh = &mesh->getSubMeshs()[s];

		render->disableLighting();
		render->disableNormalArray();
		render->disableColorArray();
		render->disableTexCoordArray();

		// vertices
		render->enableVertexArray();
		render->setVertexPointer(M_FLOAT, 3, subMesh->getVertices());

		unsigned int i;
		unsigned int displayNumber = subMesh->getDisplaysNumber();
		for(i=0; i<displayNumber; i++)
		{
			MDisplay * display = subMesh->getDisplay(i);
			if(! display->isVisible())
				continue;

			// begin / size
			unsigned int begin = display->getBegin();
			unsigned int size = display->getSize();

			// display properties
			M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
			M_CULL_MODES cullMode = display->getCullMode();

			// cull mode
			render->setCullMode(cullMode);

			// indices
			void * indices = subMesh->getIndices();

			// draw
			if(indices)
			{
				switch(subMesh->getIndicesType())
				{
                    case M_USHORT:
                        render->drawElement(primitiveType, size, M_USHORT, (unsigned short*)indices + begin);
                        break;

                    case M_UINT:
                        render->drawElement(primitiveType, size, M_UINT, (unsigned int*)indices + begin);
                        break;

                    default:
                        break;
				}
			}
			else
				render->drawArray(primitiveType, begin, size);
		}
	}
}

float MV3dView::getBillboardObjectSize(MObject3d * object, float unitSize)
{
	if(! m_view.m_camera.isOrtho())
		return (object->getTransformedPosition() - m_view.m_camera.getTransformedPosition()).getLength() * unitSize;
	else
		return m_view.m_camera.getFov() * unitSize;
}

void MV3dView::create(MWindow * rootWindow)
{
	if(! m_window)
	{
		m_window = rootWindow->addNewWindow();
		m_window->setUserPointer(this);
		m_window->setDrawCallback(drawCallback);
	}
}

void MV3dView::resize(MVector2 position, MVector2 scale)
{
	if(m_window)
	{
		m_window->setPosition(position);
		m_window->setScale(scale);
	}
}

void MV3dView::hide(void)
{
	if(m_window)
		m_window->setVisible(false);
}

void MV3dView::show(void)
{
	if(m_window)
		m_window->setVisible(true);
}

void MV3dView::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEditor * editor = MEditor::getInstance();
	MPreferences * prefs = editor->getPreferences();

	if(! m_window)
		return;
		
	if(rootWindow->isSomethingEditing() || (! m_window->isVisible()))
		return;

	switch(event)
	{
        case MWIN_EVENT_MOUSE_MOVE:
		{
			MVector2 mouseDir = rootWindow->getMouseDir();
			
			/*if(mouse->isLeftButtonPushed())
			{
				maratis->transformSelectedObjects();
				break;
			}*/

			if(m_window->isScrolled() && rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_MIDDLE))
			{
				if(prefs->isShortCutEngaged(rootWindow, "Pan View"))
				{
					m_view.pan(mouseDir.x, mouseDir.y);
				}
				else
				{
					m_view.rotate(mouseDir.x, mouseDir.y);
				}
			}
			
			break;
		}
	
        case MWIN_EVENT_MOUSE_SCROLL:
		{
			if(m_window->isHighLight() && !rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_MIDDLE))
				m_view.zoom(rootWindow->getMouseScroll().y*0.25f);
				
			break;
		}

		case MWIN_EVENT_KEY_UP:
		{
			if(prefs->isShortCutEngaged(rootWindow, "Pan View"))
			{
				//maratis->updateViewCenter();
				break;
			}
			
			if(prefs->isShortCutEngaged(rootWindow, "Orthogonal View"))
			{
				m_view.switchProjectionMode();
				break;
            }
			
			if(prefs->isShortCutEngaged(rootWindow, "Face View"))
            {
                m_view.initOrtho(1);
                break;
            }
			
            if(prefs->isShortCutEngaged(rootWindow, "Right View"))
            {
                m_view.initOrtho(3);
                break;
            }
			
            if(prefs->isShortCutEngaged(rootWindow, "Top View"))
            {
                m_view.initOrtho(7);
                break;
            }
			
            if(prefs->isShortCutEngaged(rootWindow, "Bottom View"))
            {
                m_view.initOrtho(9);
                break;
            }
			
			break;
		}
		
		default:
			break;
	}
}