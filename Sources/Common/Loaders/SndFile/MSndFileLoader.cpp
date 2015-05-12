/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MSndFileLoader.cpp
//
// SndFile Sound loader
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
//jan 2012 - Philipp Geyer <philipp@geyer.co.uk> - Changed sound loading to use MFile


#include <MCore.h>
#include <sndfile.h>
#include "MSndFileLoader.h"


sf_count_t M_SFGetFileLen(void* user_data)
{
	if(user_data == 0) return 0;  
	MFile* File = (MFile*)user_data;
	
	long pos = File->tell();
	File->rewind();
	File->seek(0, SEEK_END);
	sf_count_t size = File->tell();
	File->seek(pos, SEEK_SET);
	
	return size;
}

sf_count_t M_SFSeek(sf_count_t offset, int whence, void *user_data)
{
	if(user_data == 0) return 0;
	return ((MFile*)user_data)->seek(offset, whence);
}

sf_count_t M_SFRead(void *ptr, sf_count_t count, void *user_data)
{
	if(user_data == 0) return 0;
	return ((MFile*)user_data)->read(ptr, sizeof(char), count);
}

sf_count_t M_SFWrite(const void *ptr, sf_count_t count, void *user_data)
{
	if(user_data == 0) return 0;
	return ((MFile*)user_data)->write(ptr, sizeof(char), count);
}

sf_count_t M_SFTell(void *user_data)
{
	if(user_data == 0) return 0;
	return ((MFile*)user_data)->tell();
}

bool M_loadSound(const char * filename, void * data)
{
	SF_VIRTUAL_IO io;
	io.get_filelen = &M_SFGetFileLen;
	io.seek = &M_SFSeek;
	io.read = &M_SFRead;
	io.write = &M_SFWrite;
	io.tell = &M_SFTell;
	
	// open file
    SF_INFO infos;
	MFile* File = M_fopen(filename, "rb");
    SNDFILE * file = sf_open_virtual(&io, SFM_READ, &infos, File);
	if(! file){
		M_fclose(File);
		printf("ERROR Load Sound : unable to read %s file\n", filename);
		return false;
	}
	
	int nbSamples  = infos.channels * (int)infos.frames;
    int sampleRate = infos.samplerate;
	
	M_SOUND_FORMAT format;
    switch(infos.channels)
    {
		case 1 :
			format = M_SOUND_FORMAT_MONO16;
			break;
		case 2 :
			format = M_SOUND_FORMAT_STEREO16;
			break;
		default :
			printf("ERROR Load Sound : non supported format\n");
			M_fclose(File);
			return false;
    }
	
	MSound * sound = (MSound *)data;
	
	unsigned int size = nbSamples*2;
	sound->create(format, size, (unsigned int)sampleRate);
	
    // 16 bits file reading
	if(sf_read_short(file, (short*)sound->getData(), nbSamples) < nbSamples)
	{
		printf("ERROR Load Sound : unable to read samples\n");
		M_fclose(File);
        return false;
	}

    sf_close(file);
	M_fclose(File);
	return true;
}
