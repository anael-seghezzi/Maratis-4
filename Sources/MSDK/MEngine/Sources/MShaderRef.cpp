/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MShaderRef.cpp
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


MShaderRef::MShaderRef(unsigned int shaderId, M_SHADER_TYPES type, const char * filename):
	m_type(type),
	m_shaderId(shaderId)
{
	m_filename.set(filename);
}

MShaderRef::~MShaderRef(void){
	clear();
}

MShaderRef * MShaderRef::getNew(unsigned int shaderId, M_SHADER_TYPES type, const char * filename){
	return new MShaderRef(shaderId, type, filename);
}

void MShaderRef::clear(void){
	MEngine::getInstance()->getRenderingContext()->deleteShader(&m_shaderId);
}

void MShaderRef::destroy(void){
	delete this;
}

void MShaderRef::update(void * arg)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	MLevel * level = (MLevel*)arg;
	if(! level)
	{
		MLOG_ERROR("no level found");
		return;
	}
	
	MFXManager * FXManager = level->getFXManager();
	
	char * text = readTextFile(getFilename());
	if(text)
	{
		// gen shader if null
		if(m_shaderId == 0)
		{
			switch(m_type)
			{
			case M_SHADER_VERTEX:
				render->createVertexShader(&m_shaderId);
				break;
			case M_SHADER_PIXEL:
				render->createPixelShader(&m_shaderId);
				break;
			}
		}

		// send shader source
		render->sendShaderSource(m_shaderId, text);
		
		unsigned int i;
		unsigned int size = FXManager->getFXRefsNumber();
		for(i=0; i<size; i++)
		{
			MFXRef * FXRef = FXManager->getFXRef(i);
			if((FXRef->getPixelShaderRef() == this) || (FXRef->getVertexShaderRef() == this))
			{
				unsigned int FXId = FXRef->getFXId();
				render->updateFX(FXId);
			}
		}
	}
	
	SAFE_FREE(text);
}