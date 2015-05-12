/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MPackageManager.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Philipp Geyer <http://nistur.com>
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

#ifndef _M_PACKAGE_MANAGER_H
#define _M_PACKAGE_MANAGER_H

typedef void * MPackageEnt;
typedef void * MPackage;

/*--------------------------------------------------------------------------------
 * MPackageManager
 * Interface for adding package managers. Can be used for any file I/O managers
 * such as package file, or network streams
 *-------------------------------------------------------------------------------*/
class M_ENGINE_EXPORT MPackageManager
{
public:
	
	// destructor
	virtual ~MPackageManager(void){}
	
	virtual void init() = 0;
	virtual void cleanup() = 0;
	
	// accessors for loading
	// will add a package to the package list and
	// allow searching for files within it
	virtual MPackage	loadPackage(const char * packageName) = 0;
	virtual MPackageEnt findEntity(const char * name) = 0;
	virtual void		offlinePackage(MPackage package) = 0;
	virtual void		unloadPackage(MPackage package) = 0;

	// accessors for publishing.
	// this will not add to the package list
	// handling these packages will be your own task
	virtual MPackage	openPackage(const char * packageName) = 0;
	virtual void		closePackage(MPackage package) = 0;
	virtual MPackageEnt addFileToPackage(const char * filename, MPackage package, const char * entityName) = 0;
	virtual MPackage	mountPackage(MPackage package) = 0;
};

#endif