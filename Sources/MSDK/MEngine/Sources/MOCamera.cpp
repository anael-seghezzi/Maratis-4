/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOCamera.cpp
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


// constructor
MOCamera::MOCamera(void):
MObject3d(),
m_fog(false),
m_ortho(false),
m_fov(60),
m_fogDistance(0),
m_clippingNear(1),
m_clippingFar(1000),
m_clearColor(0.2f, 0.3f, 0.4f),
m_sceneLayer(0),
m_renderColorTexture(NULL),
m_renderDepthTexture(NULL)
{
	m_currentViewport[0] = 0;
	m_currentViewport[1] = 0;
	m_currentViewport[2] = 0;
	m_currentViewport[3] = 0;
}

// destructor
MOCamera::~MOCamera(void)
{
	MObject3d::clearObject3d();
}

// copy constructor
MOCamera::MOCamera(const MOCamera & camera):
MObject3d(camera),
m_fog(camera.m_fog),
m_ortho(camera.m_ortho),
m_fov(camera.m_fov),
m_fogDistance(camera.m_fogDistance),
m_clippingNear(camera.m_clippingNear),
m_clippingFar(camera.m_clippingFar),
m_clearColor(camera.m_clearColor),
m_currentViewMatrix(camera.m_currentViewMatrix),
m_currentProjMatrix(camera.m_currentProjMatrix),
m_frustum(camera.m_frustum),
m_sceneLayer(camera.m_sceneLayer),
m_renderColorTexture(NULL),
m_renderDepthTexture(NULL)
{
	m_currentViewport[0] = camera.m_currentViewport[0];
	m_currentViewport[1] = camera.m_currentViewport[1];
	m_currentViewport[2] = camera.m_currentViewport[2];
	m_currentViewport[3] = camera.m_currentViewport[3];
}

MVector3 MOCamera::getProjectedPoint(const MVector3 & point) const
{
	MVector4 v = m_currentViewMatrix * MVector4(point);
	v = m_currentProjMatrix * v;
	v.x = v.x / v.w;
	v.y = v.y / v.w;
	v.z = v.z / v.w;

	v.x = m_currentViewport[0] + (m_currentViewport[2] * ((v.x + 1) / 2.0f));
	v.y = m_currentViewport[1] + (m_currentViewport[3] * ((v.y + 1) / 2.0f));
	v.z = (v.z + 1) / 2.0f;

	return MVector3(v.x, v.y, v.z);
}

MVector3 MOCamera::getUnProjectedPoint(const MVector3 & point) const
{
	MVector4 nPoint;

	nPoint.x = (2 * ((point.x - m_currentViewport[0]) / ((float)m_currentViewport[2]))) - 1;
	nPoint.y = (2 * ((point.y - m_currentViewport[1]) / ((float)m_currentViewport[3]))) - 1;
	nPoint.z = (2 * point.z) - 1;
	nPoint.w = 1;

	MMatrix4x4 matrix = (m_currentProjMatrix * m_currentViewMatrix).getInverse();
	MVector4 v = matrix * nPoint;
	
	if(v.w == 0)
		return getTransformedPosition();
	
	float iw = 1.0f / v.w;
	return MVector3(v.x, v.y, v.z)*iw;
}

void MOCamera::updateListener(void)
{
	MSoundContext * soundContext = MEngine::getInstance()->getSoundContext();
	if(soundContext)
	{
		MVector3 position = getTransformedPosition();
		MVector3 direction = getRotatedVector(MVector3(0, 0, -1));
		MVector3 up = getRotatedVector(MVector3(0, 1, 0));
		soundContext->updateListenerPosition(position, direction, up);
	}
}

static void createPerspectiveView(MMatrix4x4 * matrix, float fov, float ratio, float zNear, float zFar)
{
	float ymax, xmax;
	ymax = zNear * tanf((float)(fov * M_PI / 360.0f));
	xmax = ymax * ratio;
    
	float left = -xmax;
	float right = xmax;
	float bottom = -ymax;
	float top = ymax;
    
	float temp, temp2, temp3, temp4;
	temp = 2.0f * zNear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zFar - zNear;
	matrix->entries[0] = temp / temp2;
	matrix->entries[1] = 0.0f;
	matrix->entries[2] = 0.0f;
	matrix->entries[3] = 0.0f;
	matrix->entries[4] = 0.0f;
	matrix->entries[5] = temp / temp3;
	matrix->entries[6] = 0.0f;
	matrix->entries[7] = 0.0f;
	matrix->entries[8] = (right + left) / temp2;
	matrix->entries[9] = (top + bottom) / temp3;
	matrix->entries[10] = (-zFar - zNear) / temp4;
	matrix->entries[11] = -1.0f;
	matrix->entries[12] = 0.0f;
	matrix->entries[13] = 0.0f;
	matrix->entries[14] = (-temp * zFar) / temp4;
	matrix->entries[15] = 0.0f;
}

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

void MOCamera::updateViewMatrix(void)
{
	MVector3 scale = getTransformedScale();
	MVector3 iScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

	MMatrix4x4 iScaleMatrix;
	iScaleMatrix.setScale(iScale);
	
	m_currentViewMatrix = ((*getMatrix()) * iScaleMatrix).getInverse();
}

void MOCamera::updateProjMatrix(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	render->getViewport(m_currentViewport);
	
	float ratio = (m_currentViewport[2] / (float)m_currentViewport[3]);

	// perspective view
	if(! isOrtho())
	{
		// normal perspective projection
		createPerspectiveView(&m_currentProjMatrix, m_fov, ratio, m_clippingNear, m_clippingFar);
		return;
	}

	// ortho view
	float height = m_fov * 0.5f;
	float width = height * ratio;

	createOrthoView(&m_currentProjMatrix, -width, width, -height, height, m_clippingNear, m_clippingFar);
}

void MOCamera::enableViewProjMatrix(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	// projection
	render->setMatrixMode(M_MATRIX_PROJECTION);
	render->loadIdentity();
	render->multMatrix(&m_currentProjMatrix);

	// model view mode
	render->setMatrixMode(M_MATRIX_MODELVIEW);
	render->loadIdentity();
	render->multMatrix(&m_currentViewMatrix);
}

void MOCamera::enable(void)
{
	updateViewMatrix();
	updateProjMatrix();
	enableViewProjMatrix();
}

MBox3d MOCamera::createMatrixOrientedBoundingBox(MMatrix4x4 * matrix)
{
	const MVector3 * points = m_frustum.getPoints();

	MVector3 min, max;
	min = max = (*matrix) * points[0];
	for(int i=1; i<8; i++)
	{
		MVector3 localPoint = (*matrix) * points[i];
		min.x = MIN(min.x, localPoint.x);
		min.y = MIN(min.y, localPoint.y);
		min.z = MIN(min.z, localPoint.z);
		max.x = MAX(max.x, localPoint.x);
		max.y = MAX(max.y, localPoint.y);
		max.z = MAX(max.z, localPoint.z);
	}

	return MBox3d(min, max);
}