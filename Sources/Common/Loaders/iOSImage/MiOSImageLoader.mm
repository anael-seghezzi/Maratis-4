/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MiOSImageLoader.cpp
//
// iOS image loader
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2011 Anael Seghezzi <www.maratis3d.com>
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

#include <MCore.h>
#include "MiOSImageLoader.h"

#import <UIKit/UIKit.h>



bool M_loadImage(const char * filename, void * data)
{
	if((! data) || (! filename))
		return false;

	
	// read from MFile
	MFile* fp = M_fopen(filename, "rb");
	if(!fp)
		return false;
	
	M_fseek(fp, 0, SEEK_END);
	int filesize = M_ftell(fp);
	M_rewind(fp);
	
	char* buffer = new char[filesize];
	if(filesize != M_fread(buffer, sizeof(char), filesize, fp))
	{
		M_fclose(fp);
		delete [] buffer;
		return false;
	}
	
	M_fclose(fp);

	NSData *memData = [[NSData alloc] initWithBytes:buffer length:filesize];
	CGImageRef CGImage = [UIImage imageWithData:memData].CGImage;

	delete [] buffer;
	[memData release];
	
	
	
    if(! CGImage)
	{
        return false;
	}
	
	// read CGImage infos
    CGBitmapInfo info = CGImageGetBitmapInfo(CGImage);
    CGColorSpaceModel colormodel = CGColorSpaceGetModel(CGImageGetColorSpace(CGImage));
    size_t bpp = CGImageGetBitsPerPixel(CGImage);
	
    if(bpp < 8 || bpp > 32 || (colormodel != kCGColorSpaceModelMonochrome && colormodel != kCGColorSpaceModelRGB))
    {
        printf("ERROR MiOSImageLoader : unsupported format %s\n", filename);
        return false;
    }
	
	// get data
	CFDataRef CGImgageData = CGDataProviderCopyData(CGImageGetDataProvider(CGImage));
	unsigned char * pixels = (unsigned char *)CFDataGetBytePtr(CGImgageData);
	
	unsigned int width = CGImageGetWidth(CGImage);
    unsigned int height = CGImageGetHeight(CGImage);
	
	// MImage pointer
	MImage * image = (MImage *)data;
	
	// create image
	switch(bpp)
	{
		case 24:
		{
			image->create(M_UBYTE, width, height, 3);
			memcpy(image->getData(), pixels, sizeof(char)*image->getSize());
			break;
		}
			
		case 32:
		{
			image->create(M_UBYTE, width, height, 4);
			memcpy(image->getData(), pixels, sizeof(char)*image->getSize());
			
			bool convert = false;
			switch(info & kCGBitmapAlphaInfoMask)
			{
				case kCGImageAlphaPremultipliedFirst:
				case kCGImageAlphaFirst:
				case kCGImageAlphaNoneSkipFirst:
					convert = true;
					break;
				default:
					convert = false;
			}
			
			if(convert)
			{
				unsigned int size = image->getSize();
				unsigned char * pixel = (unsigned char *)image->getData();
				
				if((info & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host) // ARGB
				{
					for(unsigned int i=0; i<size; i+=4)
					{
						unsigned char tmp[4] = {pixel[0], pixel[1], pixel[2], pixel[3]};
						pixel[0] = tmp[1];
						pixel[1] = tmp[2];
						pixel[2] = tmp[3];
						pixel[3] = tmp[0];
						pixel += 4;
					}
				}
				else // BGRA
				{
					for(unsigned int i=0; i<size; i+=4)
					{
						unsigned char tmp[4] = {pixel[0], pixel[1], pixel[2], pixel[3]};
						pixel[0] = tmp[2];
						pixel[1] = tmp[1];
						pixel[2] = tmp[0];
						pixel[3] = tmp[4];
						pixel += 4;
					}
				}
			}
				
			break;
		}
	}

	CFRelease(CGImgageData);
	return true;
}