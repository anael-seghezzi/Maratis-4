/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTexturesAnim.h
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


#ifndef _M_TEXTURES_ANIM
#define _M_TEXTURES_ANIM


class M_ENGINE_EXPORT MTexturesAnim
{
public:

	MTexturesAnim(void);
	~MTexturesAnim(void);

	static MTexturesAnim * getNew(void);
	void destroy(void);

private:

	unsigned int m_texturesAnimNumber;
	MTextureAnim * m_texturesAnim;

public:

	MTextureAnim * allocTexturesAnim(unsigned int size);
	void clearTexturesAnim(void);

	inline unsigned int getTexturesAnimNumber(void){ return m_texturesAnimNumber; }
	inline MTextureAnim * getTexturesAnim(void){ return m_texturesAnim; }
};

#endif