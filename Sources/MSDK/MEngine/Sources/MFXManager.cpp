/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MFXManager.cpp
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


#include "../Includes/MEngine.h"


MFXRef::~MFXRef(void)
{
	MEngine::getInstance()->getRenderingContext()->deleteFX(&m_FXId);
}

MFXManager::~MFXManager(void)
{
	clear();
}

void MFXManager::clear(void)
{
	unsigned int i;
	unsigned int size = m_FXRefs.size();
	for(i=0; i<size; i++)
		SAFE_DELETE(m_FXRefs[i]);

	m_FXRefs.clear();
}

MFXRef * MFXManager::addFXRef(unsigned int FXId, MShaderRef * vertexShaderRef, MShaderRef * pixelShaderRef)
{
	MFXRef * fxRef = new MFXRef(FXId, vertexShaderRef, pixelShaderRef);
	m_FXRefs.push_back(fxRef);
	return fxRef;
}