/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MV3dEdit.h
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


MV3dEdit::MV3dEdit(void):
MV3dView(),
m_selectionDepth(0),
m_currentAxis(M_AXIS_NONE),
m_tools(NULL)
{}

MV3dEdit::~MV3dEdit(void)
{}

void MV3dEdit::drawCallback(MGuiWindow * window)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MV3dEdit * viewport = (MV3dEdit *)window->getUserPointer();
	
	MSelectionManager * selection = editor->getSelectionManager();
	MRenderingContext * render = engine->getRenderingContext();
	MOCamera * camera = &viewport->m_view.m_camera;
	
	MV3dView::drawCallback(window);
	
	if(selection->getSelectionSize() > 0)
	{
		// init depth
		render->enableDepthTest();
		render->clear(M_BUFFER_DEPTH);

		// draw current transform
		switch(viewport->getTransformMode())
		{
			case M_TRANSFORM_ROTATION:
				selection->updateSelectionCenter();
				viewport->drawEdit(M_TRANSFORM_ROTATION, camera);
				break;
			case M_TRANSFORM_POSITION:
				selection->updateSelectionCenter();
				viewport->drawEdit(M_TRANSFORM_POSITION, camera);
				break;
			case M_TRANSFORM_SCALE:
				selection->updateSelectionCenter();
				viewport->drawEdit(M_TRANSFORM_SCALE, camera, 1);
				break;

            default:
                break;
		}
	}
}

void MV3dEdit::drawAxis(M_TRANSFORM_MODE mode, M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, bool viewTest)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	
	MRenderingContext * render = engine->getRenderingContext();
	MLevel * guiData = editor->getGuiData();

	// position
	MVector3 position = matrix->getTranslationPart();

	// axis
	MVector3 normal;
	MVector3 vector;
	switch(axis)
	{
		case M_AXIS_X:
			normal = MVector3(1, 0, 0);
			vector = MVector3(0, 1, 0);
			break;

		case M_AXIS_Y:
			normal = MVector3(0, 1, 0);
			vector = MVector3(1, 0, 0);
			break;

		case M_AXIS_Z:
			normal = MVector3(0, 0, 1);
			vector = MVector3(1, 0, 0);
			break;

		default:
			break;
	}

	// camera direction
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 cameraDir;
	if(! camera->isOrtho())
	{
		cameraDir = position - cameraPos;
		cameraDir.normalize();
	}
	else {
		cameraDir = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	}

	// view test
	if(viewTest)
	{
		MVector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();
		float axisAlign = ABS(cameraDir.dotProduct(axisNormal));
		
		if(mode == M_TRANSFORM_ROTATION)
		{
			if(axisAlign < 0.98f)
				render->enableDepthTest();
			else
				render->disableDepthTest();
		}
		else
		{
			if(axisAlign > 0.98f)
			{
				if(axis>0 && axis<4)
					m_axisVis[axis-1] = false;
				return;
			}
		}

	}
	else if(mode == M_TRANSFORM_ROTATION)
	{
		render->disableDepthTest();
	}
	
	if(axis>0 && axis<4)
		m_axisVis[axis-1] = true;

	// draw
	if(mode == M_TRANSFORM_ROTATION)
	{
		render->pushMatrix();
		render->translate(- (cameraDir * 4));
		render->multMatrix(matrix);

		beginDraw(M_PRIMITIVE_LINE_STRIP);

		MVector3 vec;
		unsigned int i;
		for(i=0; i<=360; i+=10)
		{
			vec = vector.getRotatedAxis((double)i, normal);
			pushVertex(vec);
		}

		endDraw(render);
		render->popMatrix();
	}
	else
	{
		render->pushMatrix();
		render->multMatrix(matrix);

		switch(axis)
		{
			case M_AXIS_X:
				render->rotate(MVector3(0, 1, 0), 90);
				break;

			case M_AXIS_Y:
				render->rotate(MVector3(1, 0, 0), -90);
				break;

			default:
				break;
		}

		beginDraw(M_PRIMITIVE_LINES);
		pushVertex(MVector3(0, 0, 0.3f));
		pushVertex(MVector3(0, 0, 0.9f));
		endDraw(render);

		render->translate(MVector3(0, 0, 0.9f));
	
		MMeshRef * meshRef = NULL;
		if(mode == M_TRANSFORM_POSITION)
			meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("cone.mesh");
		else if(mode == M_TRANSFORM_SCALE)
			meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("box.mesh");
	
		if(meshRef)
			drawTriangles(meshRef->getMesh());

		render->popMatrix();
	}
}

void MV3dEdit::computeTransformSizeAndPos(MOCamera * camera, MVector3 * position, float * size)
{
	MEditor * editor = MEditor::getInstance();
	MSelectionManager * selection = editor->getSelectionManager();

	int * viewport = camera->getCurrentViewport();
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 selectionCenter = selection->getSelectionCenter();

	if(! camera->isOrtho())
	{
		*size = (camera->getFov() * 400) / (float)viewport[3];
		MVector3 cameraDir = selectionCenter - cameraPos;
		cameraDir.normalize();
		*position = cameraPos + cameraDir*200;
	}
	else
	{
		*size = (camera->getFov() * 120) / (float)viewport[3];
		MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
		
		float dist = rayPlaneIntersection(selectionCenter, -cameraAxis, cameraPos, cameraAxis);
		if(dist > 0)
		{
			MVector3 intersection = selectionCenter - cameraAxis*dist;
			*position = intersection + cameraAxis*200;
		}
	}
}

void MV3dEdit::drawEdit(M_TRANSFORM_MODE mode, MOCamera * camera, bool local)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MWindow * rootWindow = m_window->getRootWindow();

	MSelectionManager * selection = editor->getSelectionManager();
	MRenderingContext * render = engine->getRenderingContext();
	MLevel * guiData = editor->getGuiData();
	
	// selection
	unsigned int sSize = selection->getSelectionSize();

	// view
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 selectionCenter = selection->getSelectionCenter();

	// size and pos
	MVector3 position; float size;
	computeTransformSizeAndPos(camera, &position, &size);

	// rotation
	MVector3 rotation(0, 0, 0);
	if(sSize == 1 && local)
	{
		MObject3d * object = selection->getSelectedObject(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
	}

	// matrix
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(MVector3(size, size, size));

	// draw sphere mask
	if(mode == M_TRANSFORM_ROTATION)
	{
		MMeshRef * meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("sphere.mesh");
		
		render->pushMatrix();
		render->multMatrix(&matrix);
		render->setColorMask(0, 0, 0, 0);
		drawTriangles(meshRef->getMesh());
		render->setColorMask(1, 1, 1, 1);
		render->popMatrix();

		render->enableBlending();
		render->setBlendingMode(M_BLENDING_ALPHA);
		render->setColor4(MVector4(1, 1, 1, 0.2f));

		MMatrix4x4 eyeMatrix;
		if(! camera->isOrtho())
		{
			MQuaternion quat;
			quat.setFromVectors(MVector3(0, 0, 1), selectionCenter - cameraPos);
			eyeMatrix.setRotationAxis(quat.getAngle(), quat.getAxis());
		}
		else
		{
			MQuaternion rotation = camera->getRotation();
			eyeMatrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
		}

		eyeMatrix.setTranslationPart(position);
		eyeMatrix.scale(MVector3(size, size, size));
		drawAxis(mode, M_AXIS_Z, camera, &eyeMatrix, false);
	}

	// draw current axis
	if(rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_LEFT) && (m_currentAxis != M_AXIS_NONE))
	{
		if(mode == M_TRANSFORM_ROTATION)
		{
			switch(m_currentAxis)
			{
				case M_AXIS_X:
					render->setColor4(MVector4(1, 1, 1, 0.2f));
					drawAxis(mode, M_AXIS_X, camera, &matrix, false);
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_X, camera, &matrix);
					return;

				case M_AXIS_Y:
					render->setColor4(MVector4(1, 1, 1, 0.2f));
					drawAxis(mode, M_AXIS_Y, camera, &matrix, false);
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_Y, camera, &matrix);
					return;

				case M_AXIS_Z:
					render->setColor4(MVector4(1, 1, 1, 0.2f));
					drawAxis(mode, M_AXIS_Z, camera, &matrix, false);
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_Z, camera, &matrix);
					return;

				case M_AXIS_VIEW:
					return;
	
				default:
					break;
			}
		}
		else
		{
			switch(m_currentAxis)
			{
				case M_AXIS_X:
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_X, camera, &matrix, false);
					return;

				case M_AXIS_Y:
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_Y, camera, &matrix, false);
					return;

				case M_AXIS_Z:
					render->setColor3(MVector3(1, 1, 0));
					drawAxis(mode, M_AXIS_Z, camera, &matrix, false);
					return;

				case M_AXIS_VIEW:
					return;

				default:
					break;
			}
		}
	}

	// or draw all axis
	render->setColor3(MVector3(1, 0, 0));
	drawAxis(mode, M_AXIS_X, camera, &matrix);
	render->setColor3(MVector3(0, 1, 0));
	drawAxis(mode, M_AXIS_Y, camera, &matrix);
	render->setColor3(MVector3(0, 0, 1));
	drawAxis(mode, M_AXIS_Z, camera, &matrix);
}

void MV3dEdit::computeTransformDirection(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float distance, const MVector3 & axis)
{
	MWindow * rootWindow = m_window->getRootWindow();

	// intersection
	MVector3 intersection = rayO + rayD * distance;

	// normal
	MVector3 normal = getTriangleNormal(position, intersection, position + axis);

	// mouse position
	m_tMousePosition = rootWindow->getMousePosition();
	
	// vector direction
	MVector3 pPoint = camera->getProjectedPoint(intersection - (normal * 100));
	pPoint.y = rootWindow->getHeight() - pPoint.y;

	m_tVectorDirection = (MVector2(pPoint.x, pPoint.y) - m_tMousePosition).getNormalized();
}

void MV3dEdit::computeTransformPlane(MOCamera * camera, const MVector3 & position, const MVector3 & axis)
{
	MWindow * rootWindow = m_window->getRootWindow();
	
	// plane
	MVector3 up = camera->getRotatedVector(MVector3(1, 1, 1));
	m_tPlane = getTriangleNormal(MVector3(0, 0, 0), axis, up);

	// center position
	MVector3 pPoint = camera->getProjectedPoint(position);
	pPoint.y = rootWindow->getHeight() - pPoint.y;

	m_tCenterPosition = MVector2(pPoint.x, pPoint.y);

	// vector direction
	pPoint = camera->getProjectedPoint(position + (axis * 100));
	pPoint.y = rootWindow->getHeight() - pPoint.y;

	m_tVectorDirection = MVector2(pPoint.x, pPoint.y) - m_tCenterPosition;
	m_tVectorDirection.normalize();

	// mouse position
	m_tMousePosition = rootWindow->getMousePosition();

	// offset direction
	m_tOffsetDirection = m_tMousePosition - m_tCenterPosition;
}

M_AXIS MV3dEdit::getSelectedAxis(M_TRANSFORM_MODE mode, MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float radius, bool local)
{
	MEditor * editor = MEditor::getInstance();

	MSelectionManager * selection = editor->getSelectionManager();
	MLevel * guiData = editor->getGuiData();
	
	// axis
	MVector3 radiusScale = MVector3(radius, radius, radius);
	MVector3 axisVec[3] = {
		MVector3(1, 0, 0),
		MVector3(0, 1, 0),
		MVector3(0, 0, 1)
		};

	// objects
	unsigned int oSize = selection->getSelectionSize();

	// rotation
	MVector3 rotation(0, 0, 0);
	if(oSize == 1 && local)
	{
		MObject3d * object = selection->getSelectedObject(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;
		rotation = matrix.getEulerAngles();

		for(int i=0; i<3; i++)
			axisVec[i] = matrix.getRotatedVector3(axisVec[i]);
	}

	// matrice
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// raytrace axis meshs
	float distance = camera->getClippingFar();
	M_AXIS axis = M_AXIS_NONE;
	{
		MMeshRef * meshRefs[3] = {NULL, NULL, NULL};
		
		if(mode == M_TRANSFORM_ROTATION)
		{
			meshRefs[0] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("xcircle.mesh");
			meshRefs[1] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("ycircle.mesh");
			meshRefs[2] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("zcircle.mesh");
		}
		else
		{
			meshRefs[0] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("x.mesh");
			meshRefs[1] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("y.mesh");
			meshRefs[2] = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("z.mesh");
		}
	
		for(int i=0; i<3; i++)
		{
			if(meshRefs[i] && m_axisVis[i])
			{
				MOEntity entity(meshRefs[i]);
				(*entity.getMatrix()) = matrix;
			
				float dist = entity.getRayNearestIntersectionDistance(rayO, rayD);
				if(dist > 0)
				{
					if(axis == M_AXIS_NONE || dist < distance)
					{
						axis = (M_AXIS)(i+1);
						distance = dist;
					}
					
					if(mode == M_TRANSFORM_ROTATION)
						computeTransformDirection(camera, rayO, rayD, position, dist, axisVec[i]);
					else
						computeTransformPlane(camera, position, axisVec[i]);
						
					printf("AXIS %d\n", i);
				}
			}
		}
	}

	// view axis
	if(axis == M_AXIS_NONE)
	{
		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = selection->getSelectedObject(i);
			if(getObjectRayNearestIntersectionDistance(object, rayO, rayD) > 0)
			{
				computeTransformPlane(camera, position, MVector3(0, 0, 0));
				printf("VIEW\n");
				return M_AXIS_VIEW;
			}
		}
	}

	return axis;
}

void MV3dEdit::create(MWindow * rootWindow)
{
	if(! m_window)
	{
		m_window = rootWindow->addNewWindow();
		m_window->setUserPointer(this);
		m_window->setDrawCallback(drawCallback);
	}
	
	MEditor * editor = MEditor::getInstance();
	MLevel * guiData = editor->getGuiData();
	
	if(m_window && !m_tools)
	{
		MTextureRef * ref1_1 = guiData->loadTexture("Tool_Select.png");
		MTextureRef * ref1_2 = guiData->loadTexture("Tool_Select_Pressed.png");
		MTextureRef * ref2_1 = guiData->loadTexture("Tool_Translate.png");
		MTextureRef * ref2_2 = guiData->loadTexture("Tool_Translate_Pressed.png");
		MTextureRef * ref3_1 = guiData->loadTexture("Tool_Rotate.png");
		MTextureRef * ref3_2 = guiData->loadTexture("Tool_Rotate_Pressed.png");
		MTextureRef * ref4_1 = guiData->loadTexture("Tool_Scale.png");
		MTextureRef * ref4_2 = guiData->loadTexture("Tool_Scale_Pressed.png");
		MTextureRef * ref5_1 = guiData->loadTexture("Tool_Magnet.png");
		MTextureRef * ref5_2 = guiData->loadTexture("Tool_Magnet_Pressed.png");
		MTextureRef * ref6_1 = guiData->loadTexture("Tool_Mode.png");
		MTextureRef * ref6_2 = guiData->loadTexture("Tool_Mode_Pressed.png");
		
		m_tools = rootWindow->addNewWindow();
		m_tools->setColor(MVector4(0, 0, 0, 0));
			
		int y = 0;
		MGuiButton * button;
		button = m_tools->addNewButton();
		button->setMode(MGUI_BUTTON_GROUP);
		button->setNormalTexture(ref1_1);
		button->setPressedTexture(ref1_2);
		button->setScale(MVector2(ref1_1->getWidth(), ref1_1->getHeight()));
		button->setYPosition(y);
		button->setPressed(true);
		y+=button->getScale().y;
		
		button = m_tools->addNewButton();
		button->setMode(MGUI_BUTTON_GROUP);
		button->setNormalTexture(ref2_1);
		button->setPressedTexture(ref2_2);
		button->setScale(MVector2(ref2_1->getWidth(), ref2_1->getHeight()));
		button->setYPosition(y);
		y+=button->getScale().y;
		
		button = m_tools->addNewButton();
		button->setMode(MGUI_BUTTON_GROUP);
		button->setNormalTexture(ref3_1);
		button->setPressedTexture(ref3_2);
		button->setScale(MVector2(ref3_1->getWidth(), ref3_1->getHeight()));
		button->setYPosition(y);
		y+=button->getScale().y;
		
		button = m_tools->addNewButton();
		button->setMode(MGUI_BUTTON_GROUP);
		button->setNormalTexture(ref4_1);
		button->setPressedTexture(ref4_2);
		button->setScale(MVector2(ref4_1->getWidth(), ref4_1->getHeight()));
		button->setYPosition(y);
		y+=button->getScale().y;
		
		button = m_tools->addNewButton();
		button->setNormalTexture(ref5_1);
		button->setPressedTexture(ref5_2);
		button->setScale(MVector2(ref5_1->getWidth(), ref5_1->getHeight()));
		button->setYPosition(y);
		
		int x = button->getScale().x;
		button = m_tools->addNewButton();
		button->setNormalTexture(ref6_1);
		button->setPressedTexture(ref6_2);
		button->setScale(MVector2(ref6_1->getWidth(), ref6_1->getHeight()));
		button->setXPosition(x);
		button->setYPosition(y);
		
		m_tools->setMargin(MVector2(0, 0));
		m_tools->autoScale();
	}
}

void MV3dEdit::resize(MVector2 position, MVector2 scale)
{
	MV3dView::resize(position, scale);
	
	if(m_window && m_tools)
	{
		m_tools->setXPosition(m_window->getPosition().x + m_window->getScale().x - m_tools->getScale().x - MUI_TOOLS_X);
		m_tools->setYPosition(m_window->getPosition().y + MUI_TOOLS_Y);
	}
}

void MV3dEdit::hide(void)
{
	MV3dView::hide();
	
	if(m_tools)
		m_tools->setVisible(false);
}

void MV3dEdit::show(void)
{
	MV3dView::show();
	
	if(m_tools)
		m_tools->setVisible(true);
}

void MV3dEdit::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MV3dView::onEvent(rootWindow, event);
	
	if(event == MWIN_EVENT_MOUSE_MOVE)
		m_selectionDepth = 0;
		
	if(m_tools->isMouseInside())
		return;
		
	if(event == MWIN_EVENT_MOUSE_BUTTON_UP && rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
	{
		pointSelect(rootWindow->getMousePosition());
		m_selectionDepth++;
	}
}

float MV3dEdit::getObjectRayNearestIntersectionDistance(MObject3d * object, const MVector3 & origin, const MVector3 & direction)
{
	MEditor * editor = MEditor::getInstance();
	MLevel * guiData = editor->getGuiData();

	switch(object->getType())
	{
		case M_OBJECT3D_ENTITY:
		{
			MOEntity * entity = (MOEntity *)object;
			return entity->getRayNearestIntersectionDistance(origin, direction);
		}
		
		case M_OBJECT3D_CAMERA:
		{
			MMeshRef * meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("camera.mesh");
			MOEntity entity(meshRef);
			
			float size = getBillboardObjectSize(object, m_unitSize);
			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(size));
			
			(*entity.getMatrix()) = ((*object->getMatrix()) * scaleMatrix);
			return entity.getRayNearestIntersectionDistance(origin, direction);
		}
		
		case M_OBJECT3D_LIGHT:
		{
			MMeshRef * meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("sphere.mesh");
			MOEntity entity(meshRef);
			
			float size = getBillboardObjectSize(object, m_unitSize);
			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(size));
			
			(*entity.getMatrix()) = ((*object->getMatrix()) * scaleMatrix);
			return entity.getRayNearestIntersectionDistance(origin, direction);
		}
		
		case M_OBJECT3D_SOUND:
		{
			MMeshRef * meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("sphere.mesh");
			MOEntity entity(meshRef);
			
			float size = getBillboardObjectSize(object, m_unitSize)*1.75f;
			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(size));
			
			(*entity.getMatrix()) = ((*object->getMatrix()) * scaleMatrix);
			return entity.getRayNearestIntersectionDistance(origin, direction);
		}
		
		case M_OBJECT3D_TEXT:
		{
			MOText * text = (MOText *)object;
			MMeshRef * meshRef = (MMeshRef *)guiData->getMeshManager()->getRefFromFilename("plane.mesh");
			MOEntity entity(meshRef);
			
			MVector3 min = text->getBoundingBox()->min;
			MVector3 max = text->getBoundingBox()->max;

			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(max.x - min.x, max.y - min.y, 1));
			scaleMatrix.setTranslationPart(min);

			(*entity.getMatrix()) = (*object->getMatrix()) * scaleMatrix;
			return entity.getRayNearestIntersectionDistance(origin, direction);
		}
	}
	
	return false;
}

void MV3dEdit::pointSelect(MVector2 point, bool useDepth)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	MWindow * rootWindow = m_window->getRootWindow();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();
	MSelectionManager * selection = editor->getSelectionManager();
	MPreferences * prefs = editor->getPreferences();
	
	// get camera
	MOCamera * camera = &m_view.m_camera;
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;

	// ray
	MVector3 ray_origin = camera->getTransformedPosition();
	MVector3 ray_dest = camera->getUnProjectedPoint(MVector3(point.x, (float)(rootWindow->getHeight() - point.y), 1));

	if(camera->isOrtho())
	{
		MVector3 point;
		float dist = rayPlaneIntersection(ray_dest, -cameraAxis, ray_origin + (cameraAxis * camera->getClippingNear()), cameraAxis);
		ray_origin = ray_dest - cameraAxis*dist;
	}

	ray_dest = ray_origin + ((ray_dest - ray_origin).getNormalized() * (camera->getClippingFar() - camera->getClippingNear()));
	MVector3 ray_dir = (ray_dest - ray_origin).getNormalized();

	// transform
	M_TRANSFORM_MODE transformMode = getTransformMode();
	if(transformMode != M_TRANSFORM_MOUSE && selection->getSelectionSize() > 0)
	{
		// size and pos
		MVector3 position; float size;
		computeTransformSizeAndPos(camera, &position, &size);
	
		M_AXIS axis = getSelectedAxis(transformMode, camera, ray_origin, ray_dir, position, size, transformMode==M_TRANSFORM_SCALE);
		if(axis == M_AXIS_X || axis == M_AXIS_Y || axis == M_AXIS_Z)
			return;
	}
	
	if(! prefs->isShortCutEngaged(rootWindow, "Multiple Selection"))
		selection->clearSelection();
			
	// trace objects
	static const int maxTracedObjects = 256;
	static int tracedObjects[maxTracedObjects];
	static float tracedObjectsDist[maxTracedObjects];
	
	MObject3d * nearestObject = NULL;
	float minDistance = camera->getClippingFar();
	
	unsigned int nb = 0;
	unsigned int i, size = scene->getObjectsNumber();
	for(i=0; i<size; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);
		float distance = getObjectRayNearestIntersectionDistance(object, ray_origin, ray_dir);
		if(distance > 0)
		{
			if(useDepth && nb < maxTracedObjects)
			{
				tracedObjects[nb] = i;
				tracedObjectsDist[nb] = -distance;
				nb++;
			}
			
			if(distance < minDistance)
			{
				nearestObject = object;
				minDistance = distance;
			}
		}
	}
	
	if(! nearestObject)
		return;
		
	if(useDepth && nb < maxTracedObjects && nb > 1)
	{
		sortFloatIndexList(tracedObjects, tracedObjectsDist, 0, nb-1);
	
		int id = tracedObjects[m_selectionDepth%nb];
		MObject3d * object = scene->getObjectByIndex(id);
		selection->select(object);
	}
	else
	{
		selection->select(nearestObject);
	}
}

M_TRANSFORM_MODE MV3dEdit::getTransformMode(void)
{
	if(m_tools)
	{
		for(int i=0; i<4; i++)
		{
			MGuiButton * button = m_tools->getButton(i);
			if(button->isPressed())
				return (M_TRANSFORM_MODE)i;
		}
	}
	
	return M_TRANSFORM_MOUSE;
}