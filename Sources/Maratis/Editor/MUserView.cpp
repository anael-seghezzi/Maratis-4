/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MUserView.cpp
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


MUserView::MUserView(void)
{
	initPerspective();
}

void MUserView::initPerspective(void)
{
	m_camera.setClippingNear(1);
	m_camera.setClippingFar(1000000);
	m_camera.setPosition(MVector3(0, -200, 200));
	m_camera.setEulerRotation(MVector3(40, 0, 0));
	m_camera.enableOrtho(false);
	m_camera.updateMatrix();

	m_pivot.loadIdentity();
}

void MUserView::initOrtho(int mode)
{
	MVector3 position = m_camera.getPosition();

	if(! m_camera.isOrtho())
		switchProjectionMode();

	float dist = (m_pivot - position).getLength();

	// set vue
	switch(mode)
	{
        case 1:
            m_camera.setPosition(MVector3(0, -dist, 0));
            m_camera.setEulerRotation(MVector3(90, 0, 0));
            break;
        case 3:
            m_camera.setPosition(MVector3(dist, 0, 0));
            m_camera.setEulerRotation(MVector3(90, 0, 90));
            break;
        case 7:
            m_camera.setPosition(MVector3(0, 0, dist));
            m_camera.setEulerRotation(MVector3(0, 0, 0));
            break;
        case 9:
            m_camera.setPosition(MVector3(0, 0, -dist));
            m_camera.setEulerRotation(MVector3(180, 0, 0));
            break;
	}

	m_camera.updateMatrix();
}

void MUserView::rotate(float mx, float my)
{
	// inverse center
	MVector3 lCenter = m_camera.getInversePosition(m_pivot);

	// rotation
	m_camera.addAxisAngleRotation(MVector3(1, 0, 0), -my*0.65f);

	MMatrix4x4 matrix;
	MQuaternion rotation = m_camera.getRotation();
	matrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
	matrix.invert();

	MVector3 axis = matrix * MVector3(0, 0, 1);
	m_camera.addAxisAngleRotation(axis, -mx*0.65f);
	m_camera.updateMatrix();

	// position
	MVector3 fCenter = m_camera.getTransformedVector(lCenter);
	m_camera.setPosition(m_camera.getPosition() + (m_pivot - fCenter));

	m_camera.updateMatrix();
}

void MUserView::pan(float mx, float my)
{
	int * viewport = m_camera.getCurrentViewport();

	MVector3 vecX(1, 0, 0);
	MVector3 vecZ(0, 1, 0);
	MVector3 xAxis = m_camera.getRotatedVector(vecX);
	MVector3 zAxis = m_camera.getRotatedVector(vecZ);
	MVector3 position = m_camera.getPosition();
	
	if(m_camera.isOrtho())
	{
		float dFactor = m_camera.getFov() / (viewport[3]);

		m_camera.setPosition(position + ((xAxis * (-mx)) + (zAxis * my))*dFactor);
		m_camera.updateMatrix();
	}
	else
	{
		MVector3 axis = m_camera.getRotatedVector(MVector3(0, 0, -1));

		float z = (m_pivot - position).dotProduct(axis);
		float fovFactor = m_camera.getFov() * 0.0192f;

		float dx = - ((mx / (float)viewport[3]) * z) * fovFactor;
		float dy = ((my / (float)viewport[3]) * z) * fovFactor;

		m_camera.setPosition(position + (xAxis * dx) + (zAxis * dy));
		m_camera.updateMatrix();
	}
}

void MUserView::zoom(float mz)
{
	MVector3 position = m_camera.getPosition();
	MVector3 axis = m_camera.getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	float dist = (m_pivot - position).dotProduct(axis);
	
	if(m_camera.isOrtho())
	{
		float factor = (m_camera.getFov() - 1);
		
		m_camera.setFov(m_camera.getFov() - (m_camera.getFov() * mz * 0.15f));
		m_camera.setPosition(position + (axis * (dist - factor)));
		m_camera.updateMatrix();
	}
	else
	{
		float factor = (dist * 0.01f) * (mz * 20.0f);

		if(mz > 0)
		{
			if(factor > (dist - m_camera.getClippingNear()))
				factor = dist - m_camera.getClippingNear();
		}
		else
		{
			if((dist-factor) > m_camera.getClippingFar())
				factor = -(m_camera.getClippingFar() - dist);
		}

		m_camera.setPosition(position + (axis * factor));
		m_camera.updateMatrix();
	}
}

void MUserView::switchProjectionMode(void)
{
	MVector3 cameraAxis = m_camera.getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
	MVector3 position = m_camera.getPosition();

	float dist = (m_pivot - position).dotProduct(cameraAxis);

	if(m_camera.isOrtho())
	{
		float factor = (m_camera.getFov() - 1);

		m_camera.setPosition(position + (cameraAxis * (dist - factor)));
		m_camera.updateMatrix();

		m_camera.setFov(60);
		m_camera.setClippingNear(1);
		m_camera.setClippingFar(100000);
		m_camera.enableOrtho(false);
	}
	else
	{
		m_camera.setClippingNear(-1000);
		m_camera.setClippingFar(50000);
		m_camera.setFov(1 + dist);
		m_camera.enableOrtho(true);
	}
}