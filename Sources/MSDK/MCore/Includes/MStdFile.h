/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MStdFile.h
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


#ifndef _M_STD_FILE_H
#define _M_STD_FILE_H


// Base File class
// exposes standard file I/O functions such as open/close read/write

class M_CORE_EXPORT MStdFile : public MFile
{
private:
	
	FILE * m_file;
	
public:
	
	MStdFile();
	MStdFile(const char * path, const char * mode);
	~MStdFile();
	
	static MStdFile * getNew(const char * path, const char * mode);
	
	void	open(const char * path, const char * mode);
	int		close();
	size_t	read(void* dest, size_t size, size_t count);
	size_t	write(const void * str, size_t size, size_t count);
	int		print(const char * format, ...);
	int		print(const char * format, va_list args);
	int		seek(long offset, int whence);
	long	tell();
	void	rewind();
	
	bool 	isOpen(){ return m_file != 0; }
	void	destroy(void);
};

#endif