/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MString.cpp
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


MString::MString(void):
m_data(NULL)
{}

MString::MString(const char * data):
m_data(NULL)
{
	set(data);
}

MString::MString(const MString & string):
m_data(NULL)
{
	set(string.m_data);
}
		
MString::~MString(void)
{
	clear();
}

void MString::clear(void)
{
	SAFE_DELETE_ARRAY(m_data);
}

const char * MString::getSafeString(void)
{
	if(m_data)
		return m_data;
	
	return "";
}

void MString::set(const char * data)
{
	if(data)
	{
		int len = strlen(data);
		if(len > 0)
		{
			if(m_data)
			{
				if(strlen(m_data) < len)
				{
					SAFE_DELETE_ARRAY(m_data);
					m_data = new char[len+1];
				}
			}
			else
			{
				m_data = new char[len+1];
			}
			
			strcpy(m_data, data);
			return;
		}
	}
	
	clear();
}