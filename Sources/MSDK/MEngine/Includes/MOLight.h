/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOLight.h
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


#ifndef _M_OLIGHT_H
#define _M_OLIGHT_H


// light types
enum M_LIGHT_TYPES
{
	M_LIGHT_POINT = 0,
	M_LIGHT_SPOT,
	M_LIGHT_DIRECTIONAL
};

class M_ENGINE_EXPORT MOLight : public MObject3d
{
public:

	// constructor / destructor
	MOLight(void);
	~MOLight(void);

	// copy constructor
	MOLight(const MOLight & light);

private:

	// lightType
	M_LIGHT_TYPES m_lightType;
	
	// radius
	float m_radius;

	// color
	float m_intensity;
	MVector3 m_color;
	
	// spot
	float m_spotAngle;
	float m_spotExponent;
	
	// shadow
	bool m_shadow;
	float m_shadowBias;
	float m_shadowBlur;
	unsigned int m_shadowQuality;

public:

	// type
	int getType(void){ return M_OBJECT3D_LIGHT; }

	// lightType
	inline void setLightType(M_LIGHT_TYPES lightType){ m_lightType = lightType; }
	inline M_LIGHT_TYPES getLightType(void){ return m_lightType; }
	
	// shadow
	inline void castShadow(bool shadow){ m_shadow = shadow; }
	inline void setShadowQuality(unsigned int shadowQuality){ m_shadowQuality = shadowQuality; }
	inline void setShadowBias(float shadowBias){ m_shadowBias = shadowBias; }
	inline void setShadowBlur(float shadowBlur){ m_shadowBlur = shadowBlur; }
	inline bool isCastingShadow(void){ return m_shadow; }
	inline unsigned int getShadowQuality(void){ return m_shadowQuality; }
	inline float getShadowBias(void){ return m_shadowBias; }
	inline float getShadowBlur(void){ return m_shadowBlur; }

	// radius
	inline void setRadius(float radius){ m_radius = radius; }
	inline float getRadius(void){ return m_radius; }

	// color
	inline void setIntensity(float intensity){ m_intensity = intensity; }
	inline void setColor(const MVector3 & color){ m_color = color; }
	inline float getIntensity(void){ return m_intensity; }
	inline MVector3 getColor(void) const { return m_color; }
	inline MVector3 getFinalColor(void) const { return m_color * m_intensity; }

	// spot
	inline void setSpotAngle(float angle){ m_spotAngle = CLAMP(angle, 1.0f, 180.0f); }
	inline void setSpotExponent(float exponent){ m_spotExponent = CLAMP(exponent, 0.0f, 1.0f); }
	inline float getSpotAngle(void){ return m_spotAngle; }
	inline float getSpotExponent(void){ return m_spotExponent; }
	
	// visibility
	void updateVisibility(MOCamera * camera);
};

#endif