/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMaterialAnim.h
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


#ifndef _M_MATERIAL_ANIM
#define _M_MATERIAL_ANIM


class M_ENGINE_EXPORT MMaterialAnim
{
public:

	MMaterialAnim(void);
	~MMaterialAnim(void);

private:

	unsigned int m_opacityKeysNumber;
	unsigned int m_shininessKeysNumber;
	unsigned int m_customValueKeysNumber;
	unsigned int m_diffuseKeysNumber;
	unsigned int m_specularKeysNumber;
	unsigned int m_emitKeysNumber;
	unsigned int m_customColorKeysNumber;

	MKey * m_opacityKeys;
	MKey * m_shininessKeys;
	MKey * m_customValueKeys;
	MKey * m_diffuseKeys;
	MKey * m_specularKeys;
	MKey * m_emitKeys;
	MKey * m_customColorKeys;

public:

	MKey * allocOpacityKeys(unsigned int size);
	MKey * allocShininessKeys(unsigned int size);
	MKey * allocCustomValueKeys(unsigned int size);
	MKey * allocDiffuseKeys(unsigned int size);
	MKey * allocSpecularKeys(unsigned int size);
	MKey * allocEmitKeys(unsigned int size);
	MKey * allocCustomColorKeys(unsigned int size);

	void clearOpacityKeys(void);
	void clearShininessKeys(void);
	void clearCustomValueKeys(void);
	void clearDiffuseKeys(void);
	void clearSpecularKeys(void);
	void clearEmitKeys(void);
	void clearCustomColorKeys(void);

	inline unsigned int getOpacityKeysNumber(void){ return m_opacityKeysNumber; }
	inline unsigned int getShininessKeysNumber(void){ return m_shininessKeysNumber; }
	inline unsigned int getCustomValueKeysNumber(void){ return m_customValueKeysNumber; }
	inline unsigned int getDiffuseKeysNumber(void){ return m_diffuseKeysNumber; }
	inline unsigned int getSpecularKeysNumber(void){ return m_specularKeysNumber; }
	inline unsigned int getEmitKeysNumber(void){ return m_emitKeysNumber; }
	inline unsigned int getCustomColorKeysNumber(void){ return m_customColorKeysNumber; }

	inline MKey * getOpacityKeys(void){ return m_opacityKeys; }
	inline MKey * getShininessKeys(void){ return m_shininessKeys; }
	inline MKey * getCustomValueKeys(void){ return m_customValueKeys; }
	inline MKey * getDiffuseKeys(void){ return m_diffuseKeys; }
	inline MKey * getSpecularKeys(void){ return m_specularKeys; }
	inline MKey * getEmitKeys(void){ return m_emitKeys; }
	inline MKey * getCustomColorKeys(void){ return m_customColorKeys; }
};

#endif