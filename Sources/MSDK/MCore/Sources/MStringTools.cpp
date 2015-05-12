/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// stringTool.cpp
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


void getLocalFilename(char * out, const char * workingDirectory, const char * filename)
{
	string res;
	string file(filename);
	string work(workingDirectory);


	// cut according to "\"
	char * token;

	// filename passes
	token = strtok((char *)file.c_str(), "\\/");
	vector<string> filePasses;
	while(token != NULL)
	{
		// get token
		filePasses.push_back(string(token));

		// next
		token = strtok(NULL, "\\/");
	}

	// working directory passes
	token = strtok((char *)work.c_str(), "\\/");
	vector<string> workPasses;
	while(token != NULL)
	{
		// get token
		workPasses.push_back(string(token));

		// next
		token = strtok(NULL, "\\/");
	}

	// not the same root
	if(filePasses.size() > 0 && workPasses.size() > 0)
	if(strcmp(filePasses[0].c_str(), workPasses[0].c_str()) != 0)
	{
		strcpy(out, filename);
		return;
	}

	// find base id
	unsigned int baseId = 0;

	unsigned int i;
	unsigned int wSize = workPasses.size();
	unsigned int pSize = filePasses.size();

	unsigned int nb = pSize;
	if(wSize < pSize)
		nb = wSize;

	for(i=0; i<nb; i++)
	{
		if(strcmp(filePasses[i].c_str(), workPasses[i].c_str()) != 0)
			break;

		baseId++;
	}

	// write local path
	unsigned int pointPointNumber = workPasses.size() - baseId;

	for(i=0; i<pointPointNumber; i++)
		res += string("../");

	for(i=baseId; i<pSize; i++)
	{
		if(i != baseId)
			res += string("/");
		res += filePasses[i];
	}

	strcpy(out, res.c_str());
}

void getGlobalFilename(char * out, const char * workingDirectory, const char * filename)
{
	if(!out || !workingDirectory)
		return;

	// no filename
	if(!filename)
	{
		strcpy(out, workingDirectory);
		return;
	}

	// check for zero length filename
	if(strlen(filename) == 0)
	{
		strcpy(out, workingDirectory);
		return;
	}

	// check if filename is local
	if(strlen(filename) > 1)
	{
		if(strncmp(filename, "//", 2) != 0) // not Blender local filename
		{
			if((strncmp(filename+1, ":", 1) == 0) || (filename[0] == '/')){ // Win or Unix Root
				strcpy(out, filename);
				return;
			}
		}
	}
	
	string res;
	string file(filename);
	string work(workingDirectory);

	// cut according to "\"
	char * token;

	// file passes
	token = strtok((char *)file.c_str(), "\\/");
	vector<string> filePasses;
	while(token != NULL)
	{
		// get token
		filePasses.push_back(string(token));

		// next
		token = strtok(NULL, "\\/");
	}

	// work passes
	token = strtok((char *)work.c_str(), "\\/");
	vector<string> workPasses;
	while(token != NULL)
	{
		// get token
		workPasses.push_back(string(token));

		// next
		token = strtok(NULL, "\\/");
	}
	

	unsigned int i;
	unsigned int size = filePasses.size();
	for(i=0; i<size; i++)
	{
		if((strcmp(filePasses[i].c_str(), "..") == 0) && (workPasses.size() > 0)){
			workPasses.pop_back();
		}
		else
		{
			filePasses.erase(filePasses.begin()+0, filePasses.begin()+i);
			break;
		}
	}

	if(workingDirectory[0] == '/') // UNIX
		res += "/";
	
	size = workPasses.size();
	for(i=0; i<size; i++){
		res += workPasses[i];
		res += "/";
	}

	size = filePasses.size();
	for(i=0; i<size; i++)
	{
		if(strcmp(filePasses[i].c_str(), "..") != 0)
		{
			res += filePasses[i];
			if((i+1) < size)
				res += "/";
		}
	}

	strcpy(out, res.c_str());
}

void getDirectory(char * out, const char * filename)
{
	string res;
	string file(filename);

	// cut according to "\"
	char * token;

	// file passes
	token = strtok((char *)file.c_str(), "\\/");
	vector<string> filePasses;
	while(token != NULL)
	{
		// get token
		filePasses.push_back(string(token));

		// next
		token = strtok(NULL, "\\/");
	}

	unsigned int i;
	unsigned int size = filePasses.size();
	
	if(filename[0] == '/') // UNIX
		res += "/";
		
	for(i=1; i<size; i++){
		res += filePasses[i-1];
		res += "/";
	}

	strcpy(out, res.c_str());
}

char * readTextFile(const char * filename)
{
	MFile * file;
	char * content = NULL;
	int count = 0;
	
	if(filename != NULL)
	{
		file = M_fopen(filename, "rt");
		
		if(file != NULL)
		{
			M_fseek(file, 0, SEEK_END);
    		count = M_ftell(file);
    		M_rewind(file);
			
			if(count > 0)
			{
				content = (char *)malloc(sizeof(char) * (count+1));
				count = M_fread(content, sizeof(char), count, file);
				content[count] = '\0';
			}
			
			M_fclose(file);
		}
	}

	return content;
}
