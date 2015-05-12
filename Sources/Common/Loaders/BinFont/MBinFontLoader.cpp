/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MBinFontLoader.cpp
//
// Maratis bin font loader
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
//12.01.2012 - Philipp Geyer <philipp@geyer.co.uk> - Changed font loading to use MFile

#include <MEngine.h>
#include "MBinFontLoader.h"


bool M_loadBinFont(const char * filename, void * data)
{
	// open file
	MFile * file = M_fopen(filename, "rb");
	if(! file)
	{
		printf("Error : can't read file %s\n", filename);
		return false;
	}
	
	// bin
	char header[8];
	M_fread(header, sizeof(char), 8, file);
	
	if(strcmp(header, "MFONT") != 0)
		return false;
	
	// version
	int version;
	M_fread(&version, sizeof(int), 1, file);
	
	// font size
	unsigned int fontSize;
	M_fread(&fontSize, sizeof(int), 1, file);
	
	// init font
	MFont * font = (MFont *)data;
	font->setFontSize(fontSize);
	
	
	// create image
	{
		unsigned int width, height;
		M_fread(&width, sizeof(int), 1, file);
		M_fread(&height, sizeof(int), 1, file);
		
		font->setTextureWidth(width);
		font->setTextureHeight(height);
		
		MImage image;
		image.create(M_UBYTE, width, height, 4);
		
		unsigned char color[4] = {255, 255, 255, 0};
		unsigned int x, y;
		for(y=0; y<height; y++)
		{
			for(x=0; x<width; x++)
			{
				M_fread(&color[3], sizeof(char), 1, file);
				image.writePixel(x, y, color);
			}
		}
		
		MEngine * engine = MEngine().getInstance();
		MRenderingContext * render = engine->getRenderingContext();
		
		// gen texture id
		unsigned int textureId = font->getTextureId();
		if(textureId == 0)
		{
			render->createTexture(&textureId);
			font->setTextureId(textureId);
		}
		
		// send texture image
		render->bindTexture(textureId);
		render->setTextureUWrapMode(M_WRAP_REPEAT);
		render->setTextureVWrapMode(M_WRAP_REPEAT);
		render->sendTextureImage(&image, 0, 1, 0);
	}
	
	// read characters infos
	{
		// size
		unsigned int size;
		M_fread(&size, sizeof(int), 1, file);
		
		// characters
		unsigned int i;
		for(i=0; i<size; i++)
		{
			unsigned int charCode;
			MVector2 pos;
			MVector2 offset;
			MVector2 scale;
			float xadvance;
			
			M_fread(&charCode, sizeof(int), 1, file);
			M_fread(&pos, sizeof(float), 2, file);
			M_fread(&offset, sizeof(float), 2, file);
			M_fread(&scale, sizeof(float), 2, file);
			M_fread(&xadvance, sizeof(float), 1, file);
			
			font->setCharacter(charCode, MCharacter(xadvance, offset, pos, scale));
		}
	}

	M_fclose(file);
	return true;
}