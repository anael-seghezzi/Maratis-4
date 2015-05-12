/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MVector4.cpp
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


#include "../Includes/MCore.h"


MVector4::MVector4(const MColor & color)
{
    float div = 1.0f/255.0f;
    x = color.r * div;
    y = color.g * div;
    z = color.b * div;
    w = color.a * div;
}

MVector4::operator MVector3()
{
	if(w == 1.0f)
	{
		return MVector3(x, y, z);
	}

	if(w == 0.0f)
	{
		return MVector3(0, 0, 0);
	}

	return MVector3(x/w, y/w, z/w);
}