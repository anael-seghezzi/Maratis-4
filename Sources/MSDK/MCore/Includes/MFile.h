/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MFile.h
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


#ifndef _M_FILE_H
#define _M_FILE_H


// File
class M_CORE_EXPORT MFile
{
public:
	
	// destructor
	virtual ~MFile(void){}
	
	virtual void	open(const char * path, const char * mode) = 0;
	virtual int		close() = 0;
	virtual size_t	read(void * dest, size_t size, size_t count) = 0;
	virtual size_t	write(const void * str, size_t size, size_t count) = 0;
	virtual int		print(const char * format, ...) = 0;
	virtual int		print(const char * format, va_list args) = 0;
	virtual int		seek(long offset, int whence) = 0;
	virtual long	tell() = 0;
	virtual void	rewind() = 0;
	
	virtual bool 	isOpen() = 0;
	virtual void	destroy() = 0;
};

// File Open Hook
class M_CORE_EXPORT MFileOpenHook
{
public:
	virtual ~MFileOpenHook(void){}
	virtual MFile * open(const char * path, const char * mode) = 0;
};

#endif