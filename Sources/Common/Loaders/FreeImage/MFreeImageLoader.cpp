/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MFreeImageLoader.h
//
// FreeImage image loader
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
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


#include <FreeImage.h>
#include <FreeImageIO.h>
#include <Utilities.h>

#include <MCore.h>
#include "MFreeImageLoader.h"


unsigned readProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return (unsigned)M_fread(buffer, size, count, (MFile *)handle);
}

unsigned writeProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return (unsigned)M_fwrite(buffer, size, count, (MFile *)handle);
}

int seekProc(fi_handle handle, long offset, int origin) {
	return M_fseek((MFile *)handle, offset, origin);
}

long tellProc(fi_handle handle) {
	return M_ftell((MFile *)handle);
}

void M_initFreeImage(void)
{
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise(TRUE);
	#endif
}

void M_closeFreeImage(void)
{
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif
}

bool M_loadImage(const char * filename, void * data, void * arg)
{
	if(! filename)
		return false;
		
	if(! data)
		return false;

	// freeimage io
	FreeImageIO io;
	io.read_proc  = (FI_ReadProc)readProc;
	io.write_proc = (FI_WriteProc)writeProc;
	io.tell_proc  = (FI_TellProc)tellProc;
	io.seek_proc  = (FI_SeekProc)seekProc;
	
	
	// read file buffer
	MFile * fp = M_fopen(filename, "rb");
	if(! fp)
	{
		printf("Error : can't read file %s\n", filename);
		return false;
	}
	
	
	// read freeimage
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)fp, 0);
	if(format == FIF_UNKNOWN)
	{
		printf("Error : unknow format %s\n", filename);
		M_fclose(fp);
		return false;
	}
	
	FIBITMAP * dib = FreeImage_LoadFromHandle(format, &io, (fi_handle)fp);
	if(! dib)
	{
		printf("Error : unknow dib %s\n", filename);
		M_fclose(fp);
		return false;
	}
	
	BYTE * bits = FreeImage_GetBits(dib);
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	unsigned int bpp = FreeImage_GetBPP(dib);
	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	
	if((bits == 0) || (width == 0) || (height == 0))
	{
		FreeImage_Unload(dib);
		M_fclose(fp);
		return false;
	}
	
	// flip
	FreeImage_FlipVertical(dib);
	
	// create image
	MImage * image = (MImage *)data;
	
	switch(image_type)
	{
	case FIT_BITMAP:
		switch(bpp)
		{
		case 8:
			image->create(M_UBYTE, width, height, 1);
			for(unsigned int y=0; y<height; y++)
			{
				unsigned char * dest = (unsigned char *)image->getData() + width*y;
				bits = FreeImage_GetScanLine(dib, y);
				memcpy(dest, bits, width*sizeof(char));
			}
			break;

		case 24:
			SwapRedBlue32(dib);
			image->create(M_UBYTE, width, height, 3);
			for(unsigned int y=0; y<height; y++)
			{
				unsigned char * dest = (unsigned char *)image->getData() + width*y*3;
				bits = FreeImage_GetScanLine(dib, y);
				memcpy(dest, bits, width*3*sizeof(char));
			}
			break;
	
		case 32:
			SwapRedBlue32(dib);
			image->create(M_UBYTE, width, height, 4);
			for(unsigned int y=0; y<height; y++)
			{
				unsigned char * dest = (unsigned char *)image->getData() + width*y*4;
				bits = FreeImage_GetScanLine(dib, y);
				memcpy(dest, bits, width*4*sizeof(char));
			}
			break;
			
		default:
			break;
		}
		break;
		
	case FIT_RGB16:
		image->create(M_USHORT, width, height, 3);
		for(unsigned int y=0; y<height; y++)
		{
			unsigned short * dest = (unsigned short *)image->getData() + width*y*3;
			bits = FreeImage_GetScanLine(dib, y);
			memcpy(dest, bits, width*3*sizeof(short));
		}
		break;
	
	case FIT_RGBA16:
		image->create(M_USHORT, width, height, 4);
		for(unsigned int y=0; y<height; y++)
		{
			unsigned short * dest = (unsigned short *)image->getData() + width*y*4;
			bits = FreeImage_GetScanLine(dib, y);
			memcpy(dest, bits, width*4*sizeof(short));
		}
		break;

	case FIT_FLOAT:
		image->create(M_FLOAT, width, height, 1);
		for(unsigned int y=0; y<height; y++)
		{
			float * dest = (float *)image->getData() + width*y;
			bits = FreeImage_GetScanLine(dib, y);
			memcpy(dest, bits, width*sizeof(float));
		}
		break;
		
	case FIT_RGBF:
		image->create(M_FLOAT, width, height, 3);
		for(unsigned int y=0; y<height; y++)
		{
			float * dest = (float *)image->getData() + width*y*3;
			bits = FreeImage_GetScanLine(dib, y);
			memcpy(dest, bits, width*3*sizeof(float));
		}
		break;
		
	case FIT_RGBAF:
		image->create(M_FLOAT, width, height, 4);
		for(unsigned int y=0; y<height; y++)
		{
			float * dest = (float *)image->getData() + width*y*4;
			bits = FreeImage_GetScanLine(dib, y);
			memcpy(dest, bits, width*4*sizeof(float));
		}
		break;
		
	default:
		break;
	}
	
	
	// clean
	FreeImage_Unload(dib);
	M_fclose(fp);
	return true;
}

bool M_saveImage(const char * filename, void * data)
{
	if(! filename)
		return false;
		
	if(! data)
		return false;
		
		
	// format
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filename);
	if(format == FIF_UNKNOWN)
	{
		printf("Error freeImage: unknow image format %s\n", filename);
		return false;
	}
		
		
	// image
	MImage * image = (MImage *)data;
	if(! image->getData())
		return false;
	
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();
	unsigned int yStep = width*components;
	
	FIBITMAP * dib = NULL;
	if(image->getDataType() == M_UBYTE)
	{
		dib = FreeImage_AllocateT(FIT_BITMAP, width, height, components*8);
		if(dib)
		{
			unsigned char * srcBits = (unsigned char *)image->getData();
			for(unsigned int y=0; y<height; y++)
			{
				BYTE * bits = FreeImage_GetScanLine(dib, y);
				memcpy(bits, srcBits, yStep*sizeof(char));
				srcBits += yStep;
			}
			
			if(components == 3 || components == 4)
				SwapRedBlue32(dib);
		}
	}
	else if(image->getDataType() == M_FLOAT)
	{
		switch(image->getComponents())
		{
		case 1:
			dib = FreeImage_AllocateT(FIT_FLOAT, image->getWidth(), image->getHeight(), 32);
			break;
		case 3:
			dib = FreeImage_AllocateT(FIT_RGBF, image->getWidth(), image->getHeight(), 3*32);
			break;
		case 4:
			dib = FreeImage_AllocateT(FIT_RGBAF, image->getWidth(), image->getHeight(), 4*32);
			break;
		}
		
		if(dib)
		{
			float * srcBits = (float *)image->getData();
			for(unsigned int y=0; y<height; y++)
			{
				BYTE * bits = FreeImage_GetScanLine(dib, y);
				memcpy(bits, srcBits, yStep*sizeof(float));
				srcBits += yStep;
			}
		}
	}
	
	
	if(dib)
	{
		// flip
		FreeImage_FlipVertical(dib);

		// save
		if(FreeImage_Save(format, dib, filename, PNG_Z_BEST_SPEED))
		{
			FreeImage_Unload(dib);
			return true;
		}
	}
	
		
	return false;
}
