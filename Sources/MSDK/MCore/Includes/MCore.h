/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MCore.h
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


#ifndef _M_CORE_H
#define _M_CORE_H

#ifdef WIN32

	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif

	// M_CORE_EXPORT
	#if defined(MCORE_DLL)
		#define M_CORE_EXPORT __declspec( dllexport )
	#elif defined(MCORE_STATIC)
		#define M_CORE_EXPORT
	#else
		#define M_CORE_EXPORT __declspec( dllimport )
	#endif

#else

	// M_CORE_EXPORT
	#define M_CORE_EXPORT

#endif

extern "C" {
#include <m_math.h>
#include <m_color.h>
#include <m_image.h>
#include <m_image_filter.h>
#include <m_morphology.h>
#include <m_dist.h>
}

typedef int M_TYPES;

class MImage;
class MSound;

#include "MUtils.h"
#include "MString.h"
#include "MFile.h"
#include "MStdFile.h"
#include "MFileTools.h"
#include "MMaths.h"
#include "MSystemContext.h"
#include "MInputContext.h"
#include "MRenderingContext.h"
#include "MSoundContext.h"
#include "MPhysicsContext.h"
#include "MScriptContext.h"
#include "MImage.h"
#include "MSound.h"
#include "MStringTools.h"
#include "MDataManager.h"
#include "MDataLoader.h"
#include "MLog.h"

#endif