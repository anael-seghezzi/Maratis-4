/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MDevILLoader.cpp
//
// Devil image loader
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
//jan 2012 - Philipp Geyer <philipp@geyer.co.uk> - Changed loading to use MFile


#include <IL/il.h>

#include <MCore.h>
#include "MDevILLoader.h"


static void DevILInit(void)
{
	ilInit();
	ilEnable(IL_CONV_PAL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
}

static void DevILShutDown(void)
{
	ilShutDown();
}

static ILenum getILFormat(M_TYPES dataType)
{
	ILenum format;
	switch(dataType)
	{
		default:
		case M_UBYTE:
			format = IL_UNSIGNED_BYTE;
			break;
			
		case M_USHORT:
			format = IL_UNSIGNED_SHORT;
			break;
			
		case M_INT:
			format = IL_INT;
			break;
			
		case M_FLOAT:
			format = IL_FLOAT;
			break;
	}
	
	return format;
}

static void flipImage(MImage * source, MImage * result)
{
	unsigned int y;
	M_TYPES dataType = source->getDataType();
	unsigned int width = source->getWidth();
	unsigned int height = source->getHeight();
	unsigned int components = source->getComponents();
	void * data = source->getData();
	
	result->create(dataType, width, height, components);
	void * resultData = result->getData();
	
	switch(dataType)
	{
		case M_UBYTE:
		{
			for(y=0; y<height; y++)
				memcpy((char*)resultData + y*width*components, (char*)data + (height - y - 1)*width*components, sizeof(char)*width*components);
			break;
		}
		case M_USHORT:
		{
			for(y=0; y<height; y++)
				memcpy((short*)resultData + y*width*components, (short*)data + (height - y - 1)*width*components, sizeof(short)*width*components);
			break;
		}
		case M_INT:
		{
			for(y=0; y<height; y++)
				memcpy((int*)resultData + y*width*components, (int*)data + (height - y - 1)*width*components, sizeof(int)*width*components);
			break;
		}
		case M_FLOAT:
		{
			for(y=0; y<height; y++)
				memcpy((float*)resultData + y*width*components, (float*)data + (height - y - 1)*width*components, sizeof(float)*width*components);
			break;
		}
	}
}

static void flipImage(MImage * source)
{
	M_TYPES dataType = source->getDataType();
	unsigned int width = source->getWidth();
	unsigned int height = source->getHeight();
	unsigned int components = source->getComponents();

	MImage copy;
	copy.create(dataType, width, height, components);
	
	switch(dataType)
	{
		case M_UBYTE:
		{
			memcpy(copy.getData(), source->getData(), sizeof(char)*source->getSize());
			break;
		}
		case M_USHORT:
		{
			memcpy(copy.getData(), source->getData(), sizeof(short)*source->getSize());
			break;
		}
		case M_INT:
		{
			memcpy(copy.getData(), source->getData(), sizeof(int)*source->getSize());
			break;
		}
		case M_FLOAT:
		{
			memcpy(copy.getData(), source->getData(), sizeof(float)*source->getSize());
			break;
		}
	}
	
	flipImage(&copy, source);
}

bool M_loadImageFlip(const char * filename, void * data, bool flip)
{
	DevILInit();
	
	// gen image
	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	
	// bind image
	ilBindImage(ImgId);
	
	MFile* fp = M_fopen(filename, "rb");
	if(!fp)
	{
		ilDeleteImages(1, &ImgId);
		DevILShutDown();
		printf("Error : can't read file %s\n", filename);
		return false;
	}
	
	M_fseek(fp, 0, SEEK_END);
	int filesize = M_ftell(fp);
	M_rewind(fp);
	
	char* buffer = new char[filesize];
	if(filesize != M_fread(buffer, sizeof(char), filesize, fp))
	{
		ilDeleteImages(1, &ImgId);
		DevILShutDown();
		M_fclose(fp);
		delete [] buffer;
		return false;
	}
	
	M_fclose(fp);
	
	// load image
	if(! ilLoadL(IL_TYPE_UNKNOWN, buffer, filesize))
	{
		ilDeleteImages(1, &ImgId);
		DevILShutDown();
		delete [] buffer;
		return false;
	}
	
	delete [] buffer;
	
	// get properties
	int bytePerPix = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int channels = ilGetInteger(IL_IMAGE_CHANNELS);
	
	int width  = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	
	MImage * image = (MImage *)data;
	
	
	// half float RBG
	if(bytePerPix == 6)
	{
		ilConvertImage(IL_RGB, IL_FLOAT);
		
		// create image
		image->create(M_FLOAT, (unsigned int)width, (unsigned int)height, (unsigned int)channels);
		
		// copy data
		memcpy(image->getData(), ilGetData(), image->getSize()*sizeof(float));
	}
	else
	{
		if(bytePerPix == 4)
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		else
		{
			channels = 3;
			bytePerPix = 3;
			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		}
		
		// create image
		image->create(M_UBYTE, (unsigned int)width, (unsigned int)height, (unsigned int)channels);
		
		// copy data
		unsigned int size = image->getSize();
		memcpy(image->getData(), ilGetData(), size);
	}

	if(flip)
	{
		flipImage(image);
	}
	
	ilDeleteImages(1, &ImgId);
	DevILShutDown();
	return true;
}

bool M_loadImage(const char * filename, void * data)
{
	return M_loadImageFlip(filename, data);
}

bool M_saveImage(const char * filename, void * data, unsigned int quality, bool flip)
{
	DevILInit();

	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);

	// bind this image name.
	ilBindImage(ImgId);

	MImage copy;
	MImage * image = (MImage *)data;
	M_TYPES dataType = image->getDataType();
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();

	if(flip)
	{
		flipImage(image, &copy);
		image = &copy;
	}

	ILenum format = getILFormat(dataType);
	
	if(components == 1)
		ilTexImage(width, height, 1, 1, IL_ALPHA, format, image->getData());
	if(components == 3)
		ilTexImage(width, height, 1, 3, IL_RGB, format, image->getData());
	else if(components == 4)
		ilTexImage(width, height, 1, 4, IL_RGBA, format, image->getData());
	
	if(quality < 100)
		ilSetInteger(IL_JPG_QUALITY, quality);

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(filename);

	ilDeleteImages(1, &ImgId);
	DevILShutDown();
	return true;
}