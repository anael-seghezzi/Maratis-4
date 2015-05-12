/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTexture.h
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


#ifndef _M_TEXTURE_H
#define _M_TEXTURE_H


class M_ENGINE_EXPORT MTexture
{
public:

	// constructor
	MTexture(MTextureRef * texRef):
		m_texRef(texRef),
		m_genMode(M_TEX_GEN_NONE),
		m_texTranslate(0, 0),
		m_texScale(1, 1),
		m_texRotate(0)
		{
			m_wrapMode[0] = M_WRAP_REPEAT;
			m_wrapMode[1] = M_WRAP_REPEAT;
		}

	// copy constructor
	MTexture(const MTexture & texture):
		m_texRef(texture.m_texRef),
		m_genMode(texture.m_genMode),
		m_texTranslate(texture.m_texTranslate),
		m_texScale(texture.m_texScale),
		m_texRotate(texture.m_texRotate)
		{
			m_wrapMode[0] = texture.m_wrapMode[0];
			m_wrapMode[1] = texture.m_wrapMode[1];
		}

private:

	// texture ref
	MTextureRef * m_texRef;

	// gen mode
	M_TEX_GEN_MODES m_genMode;

	// wrap mode
	M_WRAP_MODES m_wrapMode[2];

	// texture matrix
	MVector2 m_texTranslate;
	MVector2 m_texScale;
	float m_texRotate;

public:

	// texture ref
	void setTextureRef(MTextureRef * texRef){ m_texRef = texRef; }
	MTextureRef * getTextureRef(void){ return m_texRef; }

	// gen mode
	void setGenMode(M_TEX_GEN_MODES genMode){ m_genMode = genMode; }
	M_TEX_GEN_MODES getGenMode(void){ return m_genMode; }

	// wrap mode
	void setUWrapMode(M_WRAP_MODES wrapMode){ m_wrapMode[0] = wrapMode; }
	void setVWrapMode(M_WRAP_MODES wrapMode){ m_wrapMode[1] = wrapMode; }
	M_WRAP_MODES getUWrapMode(void){ return m_wrapMode[0]; }
	M_WRAP_MODES getVWrapMode(void){ return m_wrapMode[1]; }

	// texture matrix
	inline void setTexTranslate(const MVector2 & texTranslate){ m_texTranslate = texTranslate; }
	inline void setTexScale(const MVector2 & texScale){ m_texScale = texScale; }
	inline void setTexRotate(float texRotate){ m_texRotate = texRotate; }

	inline MVector2 getTexTranslate(void) const { return m_texTranslate; }
	inline MVector2 getTexScale(void) const { return m_texScale; }
	inline float getTexRotate(void){ return m_texRotate; }
};

#endif