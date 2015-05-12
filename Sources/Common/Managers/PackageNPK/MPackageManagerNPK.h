/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisCommon
// MPackageManagerNPK.h
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

#ifndef _M_PACKAGE_MANAGER_NPK_H
#define _M_PACKAGE_MANAGER_NPK_H


class MPackageFileOpenHook : public MFileOpenHook
{
public:
	MFile* open(const char* path, const char* mode);
};

/*--------------------------------------------------------------------------------
 * MPackageManagerNPK
 * Package manager with NPK backend
 * <http://npk.googlecode.com/>
 *-------------------------------------------------------------------------------*/
class MPackageManagerNPK : public MPackageManager
{
private:
	
	MPackage* m_packages;
	MPackageFileOpenHook* m_fileOpenHook;

	MPackage m_writable;
	
public:

	MPackageManagerNPK();
	~MPackageManagerNPK();

	void init();
	void cleanup();
	
	MPackage	loadPackage(const char* packageName);
	MPackageEnt findEntity(const char* name);
	void		unloadPackage(MPackage package);
	// for now this will just call unloadPackage
	// in future it should keep the package in a state
	// where it can be searched (store a list of the files?)
	// and then only reloaded when files are requested
	void		offlinePackage(MPackage package);

	// publishing
	MPackage	openPackage(const char* packageName);
	void		closePackage(MPackage package);
	MPackageEnt addFileToPackage(const char* filename, MPackage package, const char* entityName);
	MPackage	mountPackage(MPackage package);
};

#endif