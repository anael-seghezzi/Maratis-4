/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore Additional
// MWavSoundLoader.cpp
//
// simple wav Sound loader
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


#include <MCore.h>
#include "MWavSoundLoader.h"


bool M_loadWavSound(const char * filename, void * data)
{
	MFile * file = M_fopen(filename, "rb");
	if(! file)
	{
		printf("ERROR Load Sound : unable to read %s file\n", filename);
		return false;
	}
	
	// read header
	char header[4];
	M_fread(header, sizeof(char), 4, file);
	
	// RIFF - WAV
	if(strncmp(header, "RIFF", 4) == 0)
	{
		unsigned int size;
		M_fread(&size, sizeof(int), 1, file);
		M_fread(header, sizeof(char), 4, file);
		
		if(strncmp(header, "WAVE", 4) == 0)
		{
			short format_tag, channels, block_align, bits_per_sample;
			unsigned int format_length, sample_rate, avg_bytes_sec, data_size;
			
			M_fread(header, sizeof(char), 4, file); // "fmt ";
			M_fread(&format_length, sizeof(int),1,file);
			M_fread(&format_tag, sizeof(short), 1, file);
			M_fread(&channels, sizeof(short),1,file);
			M_fread(&sample_rate, sizeof(int), 1, file);
			M_fread(&avg_bytes_sec, sizeof(int), 1, file);
			M_fread(&block_align, sizeof(short), 1, file);
			M_fread(&bits_per_sample, sizeof(short), 1, file);
			M_fread(header, sizeof(char), 4, file); // "data"
			M_fread(&data_size, sizeof(int), 1, file);
			
			if((! (bits_per_sample == 8 || bits_per_sample == 16)) || (! (channels > 0 && channels <= 2)) || (format_tag != 1))
			{
				printf("1ERROR Load Sound : unsupported RIFF file\n");
				M_fclose(file);
				return false;
			}
			
			M_SOUND_FORMAT format;
			switch(bits_per_sample)
			{
				case 8:
				{
					switch(channels)
					{
						case 1:
							format = M_SOUND_FORMAT_MONO8;
							break;
						case 2:
							format = M_SOUND_FORMAT_STEREO8;
							break;
					}
					break;
				}
				case 16:
				{
					switch(channels)
					{
						case 1:
							format = M_SOUND_FORMAT_MONO16;
							break;
						case 2:
							format = M_SOUND_FORMAT_STEREO16;
							break;
					}
					break;
				}
			}
			
			// create sound
			MSound * sound = (MSound *)data;
			sound->create(format, data_size, sample_rate);
			
			// read sound data
			M_fread((char *)sound->getData(), sizeof(char), data_size, file);
			
			M_fclose(file);
			return true;
		}
		else{
			printf("ERROR Load Sound : unsupported RIFF file\n");
		}
	}
	
	M_fclose(file);
	return false;
}
