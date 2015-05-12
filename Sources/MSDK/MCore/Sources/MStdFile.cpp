/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MStdFile.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012 Philipp Geyer <http://nistur.com>
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


#include "../Includes/MCore.h"
#include <stdarg.h>


const void frewind(FILE * file){ rewind(file); }


MStdFile::MStdFile(): m_file(0) {}
MStdFile::MStdFile(const char * path, const char * mode) : m_file(0)
{
	open(path, mode);
}

MStdFile::~MStdFile()
{
	close();
}

MStdFile * MStdFile::getNew(const char * path, const char * mode)
{
	return new MStdFile(path, mode);
}

void MStdFile::destroy(void)
{
	delete this;
}

void MStdFile::open(const char * path, const char * mode)
{
	close();
	m_file = fopen(path, mode);
}

int MStdFile::close()
{
	int rtn = 0;
	if(m_file)
	{
		rtn = fclose(m_file);
		m_file = 0;
	}
	return rtn;
}

size_t MStdFile::read(void * dest, size_t size, size_t count)
{
	if(m_file)
		return fread(dest, size, count, m_file);
	return 0;
}

size_t MStdFile::write(const void * str, size_t size, size_t count)
{
	if(m_file)
		return fwrite(str, size, count, m_file);
	return 0;
}

int MStdFile::print(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	int rtn = print(format, args);
	va_end(args);
	return rtn;
}

int MStdFile::print(const char * format, va_list args)
{
	if(m_file)
		return vfprintf(m_file, format, args);
	return 0;
}

int MStdFile::seek(long offset, int whence)
{
	if(m_file)
		return fseek(m_file, offset, whence);
	return 0;
}

long MStdFile::tell()
{
	if(m_file)
		return ftell(m_file);
	return 0;
}

void MStdFile::rewind(void)
{
	if(m_file)
		frewind(m_file);
}
