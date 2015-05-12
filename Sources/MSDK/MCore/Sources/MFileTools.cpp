/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// FileTools.cpp
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
// jan 2012, FILE wrapper by Philipp Geyer <http://nistur.com>


#include "../Includes/MCore.h"

#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <algorithm>

#ifdef WIN32
	#ifdef __CYGWIN__
		// on Cygwin, mkdir is to be used as on POSIX OS
		#define mkdir(file) mkdir(file, 0777)
	#else
		#include <direct.h>
		#define mkdir _mkdir
		#define rmdir _rmdir
	#endif
#else
	#define mkdir(file) mkdir(file, 0777)
    #include <unistd.h>
#endif

static MFileOpenHook * s_fileOpenHook = 0;



bool copyFile(const char * inFilename, const char * outFilename)
{
	MFile * in = M_fopen(inFilename, "rb");
	if(! in)
	{
		printf("unable to read %s file\n", inFilename);
		return false;
	}

	MFile * out = M_fopen(outFilename, "wb");
	if(! out)
	{
		printf("unable to create %s file\n", outFilename);
		M_fclose(in);
		return false;
	}

	size_t n;
    char buffer[BUFSIZ];

    while((n = M_fread(buffer, sizeof(char), sizeof(buffer), in)) > 0)
    {
        if(M_fwrite(buffer, sizeof(char), n, out) != n)
            printf("write failed\n");
    }

	M_fclose(in);
	M_fclose(out);
	return true;
}

static bool createDirectoryInternal(const char * filename)
{
	if(mkdir(filename) != -1)
		return true;
	else
		return false;
}

bool createDirectory(const char * filename, bool recursive)
{
	if(! recursive)
		return createDirectoryInternal(filename);

    char* pp;
    char* sp;
    bool status = false;
    char copypath[256];
	strcpy(copypath, filename);

    pp = copypath;
    while (!status && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            // Neither root nor double slash in path
            *sp = '\0';
            status = createDirectoryInternal(copypath);
            *sp = '/';
        }
        pp = sp + 1;
    }
	
    return createDirectoryInternal(filename);
}

bool isFileExist(const char * filename)
{
	MFile * file = M_fopen(filename, "r");
	if(! file)
		return false;
	M_fclose(file);
	return true;
}

bool isDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;
    closedir(pdir);
	return true;
}

bool isEmptyDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return true;

	dirent * pent = NULL;
    while((pent = readdir(pdir)))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		return false;
	}

    closedir(pdir);
	return true;
}

bool clearDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;

    while((pent = readdir(pdir)))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char file[256];
		getGlobalFilename(file, filename, pent->d_name);

		if(! isDirectory(file))
			remove(file);
    }

    closedir(pdir);
    return true;
}

bool removeDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;

    while((pent = readdir(pdir)))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char file[256];
		getGlobalFilename(file, filename, pent->d_name);

		if(isDirectory(file))
			removeDirectory(file);
		else
			remove(file);
    }

    closedir(pdir);
	rmdir(filename);
    return true;
}

bool copyDirectory(const char * inFilename, const char * outFilename)
{
    DIR * pdir = opendir(inFilename);
	if(! pdir)
		return false;

	mkdir(outFilename);
	dirent * pent = NULL;

    while((pent = readdir(pdir)))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char fileIn[256];
		char fileOut[256];
		getGlobalFilename(fileIn, inFilename, pent->d_name);
		getGlobalFilename(fileOut, outFilename, pent->d_name);

		if(isDirectory(fileIn))
			copyDirectory(fileIn, fileOut);
		else
			copyFile(fileIn, fileOut);
    }

    closedir(pdir);
    return true;
}

bool readDirectory(const char * filename, vector<string> * files, bool hiddenFiles, bool recursive)
{
	DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;
    while((pent = readdir(pdir)))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		if(! hiddenFiles && strlen(pent->d_name) > 0)
		{
			#ifndef _WIN32
			if(pent->d_name[0] == '.')
				continue;
			#endif
		}
		   
		if(recursive)
		{
			char file[256];
			getGlobalFilename(file, filename, pent->d_name);
			
			if(isDirectory(file))
				readDirectory(file, files, hiddenFiles, recursive);
			else
				files->push_back(file);
		}
		else
		{
			files->push_back(string(pent->d_name));
		}
    }
	
    closedir(pdir);
    return true;
}

void M_registerFileOpenHook(MFileOpenHook * hook)
{		
	s_fileOpenHook = hook;
}

MFileOpenHook* M_getFileOpenHook()
{
	return s_fileOpenHook;
}

MFile * M_fopen(const char * path, const char * mode)
{
	MFile * rtn;
	if(s_fileOpenHook)
		rtn = s_fileOpenHook->open(path, mode);
	else
		rtn = MStdFile::getNew(path, mode);
	
	// if all loading failed, return 0
	if(!rtn->isOpen())
	{
		rtn->destroy();
		return 0;
	}
	
	return rtn;
}

int M_fclose(MFile * stream)
{
	int rtn = 0;

	if(stream)
	{
		rtn = stream->close();
		stream->destroy();
	}
	return rtn;
}

size_t M_fread(void * dest, size_t size, size_t count, MFile * stream)
{
	if(stream)
		return stream->read(dest, size, count);

	return 0;
}

size_t M_fwrite(const void * str, size_t size, size_t count, MFile * stream)
{
	if(stream)
		return stream->write(str, size, count);

	return 0;
}

int M_fprintf(MFile * stream, const char * format, ...)
{
	va_list args;
	if(stream)
	{
		va_start(args, format);
		return stream->print(format, args);
		va_end(args);
	}
	return 0;
}

int M_fseek(MFile * stream, long offset, int whence)
{
	if(stream)
		return stream->seek(offset, whence);

	return 0;
}

long M_ftell(MFile * stream)
{
	if(stream)
		return stream->tell();

	return 0;
}

void M_rewind(MFile * stream)
{
	if(stream)
		stream->rewind();
}
