/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// FileTools.h
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


#ifndef _FILE_TOOLS_H
#define _FILE_TOOLS_H

// general file and directory tools
M_CORE_EXPORT bool copyFile(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool createDirectory(const char * filename, bool recursive = false);
M_CORE_EXPORT bool isDirectory(const char * filename);
M_CORE_EXPORT bool isEmptyDirectory(const char * filename);
M_CORE_EXPORT bool clearDirectory(const char * filename);
M_CORE_EXPORT bool removeDirectory(const char * filename);
M_CORE_EXPORT bool isFileExist(const char * filename);
M_CORE_EXPORT bool copyDirectory(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool readDirectory(const char * filename, vector<string> * files, bool hiddenFiles = false, bool recursive = false);

// file wrapper
M_CORE_EXPORT void M_registerFileOpenHook(MFileOpenHook * hook);
M_CORE_EXPORT MFileOpenHook * M_getFileOpenHook();

M_CORE_EXPORT MFile* M_fopen(const char * path, const char* mode);
M_CORE_EXPORT int	 M_fclose(MFile * stream);
M_CORE_EXPORT size_t M_fread(void * dest, size_t size, size_t count, MFile * stream);
M_CORE_EXPORT size_t M_fwrite(const void * str, size_t size, size_t count, MFile * stream);
M_CORE_EXPORT int	 M_fprintf(MFile * stream, const char * format, ...);
M_CORE_EXPORT int	 M_fseek(MFile * stream, long offset, int whence);
M_CORE_EXPORT long	 M_ftell(MFile * stream);
M_CORE_EXPORT void	 M_rewind(MFile * stream);

#endif