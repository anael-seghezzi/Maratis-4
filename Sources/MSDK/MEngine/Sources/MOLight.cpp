/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOLight.cpp
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


MOLight::MOLight(void):
MObject3d(),
m_lightType(M_LIGHT_POINT),
m_radius(200),
m_intensity(1),
m_color(1, 1, 1),
m_spotAngle(180),
m_spotExponent(0),
m_shadow(false),
m_shadowBias(1.0f),
m_shadowBlur(0.25f),
m_shadowQuality(512)
{}

MOLight::~MOLight(void)
{
	MObject3d::clearObject3d();
}

// copy constructor
MOLight::MOLight(const MOLight & light):
MObject3d(light),
m_lightType(light.m_lightType),
m_radius(light.m_radius),
m_intensity(light.m_intensity),
m_color(light.m_color),
m_spotAngle(light.m_spotAngle),
m_spotExponent(light.m_spotExponent),
m_shadow(light.m_shadow),
m_shadowBias(light.m_shadowBias),
m_shadowBlur(light.m_shadowBlur),
m_shadowQuality(light.m_shadowQuality)
{}

void MOLight::updateVisibility(MOCamera * camera)
{
	MFrustum * frustum = camera->getFrustum();

	if(m_lightType == M_LIGHT_DIRECTIONAL)
	{
		setVisible(true);
		return;
	}
	
	// TODO: optimze test for spot
	MVector3 min = getTransformedPosition() - m_radius;
	MVector3 max = getTransformedPosition() + m_radius;

	MVector3 points[8] = {
		MVector3(min.x, min.y, min.z),
		MVector3(min.x, max.y, min.z),
		MVector3(max.x, max.y, min.z),
		MVector3(max.x, min.y, min.z),
		MVector3(min.x, min.y, max.z),
		MVector3(min.x, max.y, max.z),
		MVector3(max.x, max.y, max.z),
		MVector3(max.x, min.y, max.z)
	};

	// is box in frustum
	setVisible(frustum->isPointCloudVisible(points, 8));
}