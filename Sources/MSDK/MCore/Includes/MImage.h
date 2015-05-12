/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MImage.h
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


#ifndef _M_IMAGE_H
#define _M_IMAGE_H

class M_CORE_EXPORT MImage
{
public:

	MImage(void);
	MImage(const MImage & image);
	~MImage(void);

private:

	struct m_image m_raw;

public:

	void create(M_TYPES dataType, unsigned int width, unsigned int height, unsigned int components);
	void clear(void);
	void readPixel(unsigned int x, unsigned int y, void * color);
	void writePixel(unsigned int x, unsigned int y, void * color);
	void copyFrom(const MImage & image);
	
	inline void * getData(void){ return m_raw.data; }
	M_TYPES getDataType(void){ return (M_TYPES)m_raw.type; }

	inline unsigned int getComponents(void){ return m_raw.comp; }
	inline unsigned int getWidth(void){ return m_raw.width; }
	inline unsigned int getHeight(void){ return m_raw.height; }
	inline unsigned int getSize(void){ return m_raw.size; }
	inline struct m_image *getRaw(void){ return &m_raw; }
};

#endif