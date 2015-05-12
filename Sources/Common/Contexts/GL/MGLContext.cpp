/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MGLContext.cpp
//
// OpenGL Rendering Context
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
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

#include <MCore.h>
#include <iostream>

#ifdef __APPLE__
	#include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
#else
	#include <GLee.h>
#endif

#include "MGLContext.h"


#define MAX_MRCSHADERS 1024
#define MAX_MRCFXS 1024

static int g_GLversion = 0;
static float g_maxAnisotropy = 0.0f;
static GLhandleARB g_shaderIds[MAX_MRCSHADERS];
static GLhandleARB g_fxs[MAX_MRCFXS];


GLenum returnGLType(M_TYPES type)
{
	switch(type)
	{
	default:
		return GL_NONE;

	case M_BOOL:
		return GL_BOOL;

	case M_BYTE:
		return GL_BYTE;

	case M_UBYTE:
		return GL_UNSIGNED_BYTE;

	case M_SHORT:
		return GL_SHORT;

	case M_USHORT:
		return GL_UNSIGNED_SHORT;

	case M_INT:
		return GL_INT;

	case M_UINT:
		return GL_UNSIGNED_INT;

	case M_FLOAT:
		return GL_FLOAT;

	case M_DOUBLE:
		return GL_DOUBLE;
	}
}

GLenum returnPrimitiveType(M_PRIMITIVE_TYPES type)
{
	switch(type)
	{
	default:
		return GL_NONE;

	case M_PRIMITIVE_LINES:
		return GL_LINES;

	case M_PRIMITIVE_LINE_LOOP:
		return GL_LINE_LOOP;

	case M_PRIMITIVE_LINE_STRIP:
		return GL_LINE_STRIP;

	case M_PRIMITIVE_TRIANGLES:
		return GL_TRIANGLES;

	case M_PRIMITIVE_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;

	case M_PRIMITIVE_TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
	}
}

GLenum returnTexFilterMode(M_TEX_FILTER_MODES mode)
{
	switch(mode)
	{
	default:
	case M_TEX_FILTER_NEAREST:
		return GL_NEAREST;

	case M_TEX_FILTER_NEAREST_MIPMAP_NEAREST:
		return GL_NEAREST_MIPMAP_NEAREST;

	case M_TEX_FILTER_NEAREST_MIPMAP_LINEAR:
		return GL_NEAREST_MIPMAP_LINEAR;

	case M_TEX_FILTER_LINEAR:
		return GL_LINEAR;

	case M_TEX_FILTER_LINEAR_MIPMAP_NEAREST:
		return GL_LINEAR_MIPMAP_NEAREST;

	case M_TEX_FILTER_LINEAR_MIPMAP_LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;
	}
}

GLenum returnTexMode(M_TEX_MODES mode)
{
	switch(mode)
	{
	case M_R:
		return GL_R;

	case M_RG:
		return GL_RG;

	default:
	case M_RGB:
		return GL_RGB;

	case M_RGBA:
		return GL_RGBA;

	case M_DEPTH:
		return GL_DEPTH_COMPONENT;
	}
}

GLenum returnAttachType(M_FRAME_BUFFER_ATTACHMENT type)
{
	if(type == M_ATTACH_DEPTH)
		return GL_DEPTH_ATTACHMENT_EXT;
	else if(type == M_ATTACH_STENCIL)
		return GL_STENCIL_ATTACHMENT_EXT;
	else
		return GL_COLOR_ATTACHMENT0_EXT + ((int)type - 2);
}

MGLContext::MGLContext(void):
m_currentFrameBuffer(0)
{
	// version
	const char * version = (const char *)glGetString(GL_VERSION);
	if(version)
	{
	    m_gl_version=version;
		sscanf(version, "%d", &g_GLversion);
	}

	// shaders and fxs
	memset(g_shaderIds, 0, MAX_MRCSHADERS*sizeof(GLhandleARB));
	memset(g_fxs, 0, MAX_MRCFXS*sizeof(GLhandleARB));

	// init cull face (back)
	enableCullFace();
	setCullMode(M_CULL_BACK);

	// normalize
	glEnable(GL_NORMALIZE);

	// fog
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_MODE, GL_LINEAR);

	// depth
	enableDepthTest();
	setDepthMode(M_DEPTH_LEQUAL);
	glClearDepth(1.0f);

	// line
	glLineWidth(1);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// point
	//glPointSize(2.0);
	//glEnable(GL_POINT_SMOOTH);

	//float coeffs[] = {1.0f, 0, 0};
	//glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, coeffs);

	//glPointParameterf (GL_POINT_SIZE_MAX, 100.0);
	//glPointParameterf (GL_POINT_SIZE_MIN, 1.0f);

	// stencil
	glClearStencil(0);

	// pixel pack/unpack
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// anisotropic filtering
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotropy);
}

MGLContext::~MGLContext(void)
{
	// fxs
	for(unsigned int i=0; i<MAX_MRCFXS; i++)
	{
		unsigned int id = i;
		deleteFX(&id);
	}
	
	// shaders
	for(unsigned int i=0; i<MAX_MRCSHADERS; i++)
	{
		unsigned int id = i;
		deleteShader(&id);
	}
}

// view
void MGLContext::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
{
	MMatrix4x4 matrix;

	float ymax, xmax;
	ymax = zNear * tanf((float)(fov * M_PI / 360.0f));
	xmax = ymax * ratio;

	float left = -xmax;
	float right = xmax;
	float bottom = -ymax;
	float top = ymax;

	float temp, temp2, temp3, temp4;
	temp = 2.0f * zNear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zFar - zNear;
	matrix.entries[0] = temp / temp2;
	matrix.entries[1] = 0.0;
	matrix.entries[2] = 0.0;
	matrix.entries[3] = 0.0;
	matrix.entries[4] = 0.0;
	matrix.entries[5] = temp / temp3;
	matrix.entries[6] = 0.0;
	matrix.entries[7] = 0.0;
	matrix.entries[8] = (right + left) / temp2;
	matrix.entries[9] = (top + bottom) / temp3;
	matrix.entries[10] = (-zFar - zNear) / temp4;
	matrix.entries[11] = -1.0;
	matrix.entries[12] = 0.0;
	matrix.entries[13] = 0.0;
	matrix.entries[14] = (-temp * zFar) / temp4;
	matrix.entries[15] = 0.0;

	multMatrix(&matrix);
}

void MGLContext::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar){
	glOrtho(left, right, bottom, top, zNear, zFar);
}

// viewport
void MGLContext::setViewport(int x, int y, unsigned int width, unsigned int height){
	glViewport(x, y, width, height);
}

// clear
void MGLContext::clear(int buffer)
{
	switch(buffer)
	{
	case M_BUFFER_COLOR:
		glClear(GL_COLOR_BUFFER_BIT);
		return;

	case M_BUFFER_DEPTH:
		glClear(GL_DEPTH_BUFFER_BIT);
		return;

	case M_BUFFER_STENCIL:
		glClear(GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_DEPTH:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_STENCIL:
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_COLOR | M_BUFFER_DEPTH | M_BUFFER_STENCIL:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;

	case M_BUFFER_DEPTH | M_BUFFER_STENCIL:
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		return;
	}
}

void MGLContext::setClearColor(const MVector4 & color){
	glClearColor(color.x, color.y, color.z, color.w);
}

// finish
void MGLContext::finish(void){
	glFinish();
}

// texture
void MGLContext::enableTexture(void)
{
	glEnable(GL_TEXTURE_2D);
}

void MGLContext::disableTexture(void)
{
	glDisable(GL_TEXTURE_2D);
}

void MGLContext::setTextureGenMode(M_TEX_GEN_MODES mode)
{
}

void MGLContext::setTextureFilterMode(M_TEX_FILTER_MODES min, M_TEX_FILTER_MODES mag)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, returnTexFilterMode(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, returnTexFilterMode(mag));
}

void MGLContext::setTextureUWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
}

void MGLContext::setTextureVWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

void MGLContext::setTextureCombineMode(M_TEX_COMBINE_MODES combine)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

	switch(combine)
	{
	default:
	case M_TEX_COMBINE_REPLACE:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
		break;

	case M_TEX_COMBINE_MODULATE:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
		break;

	case M_TEX_COMBINE_ADD:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);
		break;

	case M_TEX_COMBINE_SUB:
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_SUBTRACT);
		break;
	}
}

void MGLContext::bindTexture(unsigned int textureId, unsigned int multitextureId)
{
	glActiveTexture(GL_TEXTURE0 + multitextureId);
	glClientActiveTexture(GL_TEXTURE0 + multitextureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void MGLContext::createTexture(unsigned int * textureId)
{
	glGenTextures(1, textureId);
}

void MGLContext::deleteTexture(unsigned int * textureId)
{
	glDeleteTextures(1, textureId);
}

void MGLContext::sendTextureImage(MImage * image, bool mipMap, bool filter, bool compress)
{
	// get properties
	unsigned int bytePerPix = image->getComponents();

	unsigned int width  = image->getWidth();
	unsigned int height = image->getHeight();

	int internalFormat = GL_RGB;
	int format = GL_RGB;

	if(compress)
	{
		if(bytePerPix == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_COMPRESSED_RGBA_ARB;
		}
		else
		{
			internalFormat = GL_COMPRESSED_RGB_ARB;
		}
	}
	else
	{
		if(bytePerPix == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_RGBA;
		}
	}

	int glType = GL_TEXTURE_2D;

	glEnable(glType);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	if(filter)
	{
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(mipMap)
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(mipMap)
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	glTexImage2D(glType, 0, internalFormat, width, height, 0, format, returnGLType(image->getDataType()), image->getData());
	if(mipMap)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_maxAnisotropy); // anisotropic filtering
		glGenerateMipmapEXT(glType);
	}

	/*
	if(mipMap)
		gluBuild2DMipmaps(glType, bytePerPix, width, height, format, GL_UNSIGNED_BYTE, image->getData());
	else
		glTexImage2D(glType, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, image->getData());
	*/
}

void MGLContext::texImage(unsigned int level, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	GLenum intFormat = format;
	//if(type == M_FLOAT && mode == M_RGB)
	//	intFormat = GL_RGB32F_ARB;

	glTexImage2D(GL_TEXTURE_2D, level, intFormat, width, height, 0, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_maxAnisotropy); // anisotropic filtering
}

void MGLContext::texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_maxAnisotropy); // anisotropic filtering
}

void MGLContext::generateMipMap(void){
	glGenerateMipmapEXT(GL_TEXTURE_2D);
}

void MGLContext::getTexImage(unsigned int level, MImage * image)
{
	if(image)
	{
		int width, height;
		int r, g, b, a, depth, format;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_RED_SIZE, &r);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_GREEN_SIZE, &g);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_BLUE_SIZE, &b);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_ALPHA_SIZE, &a);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_DEPTH_SIZE, &depth);

		unsigned int dpp = 0;
		if(b > 0)
		{
			if(a > 0)
			{
				format = GL_RGBA;
				dpp = 4;
			}
			else
			{
				format = GL_RGB;
				dpp = 3;
			}
		}
		else if(depth > 0)
		{
			format = GL_DEPTH_COMPONENT;
			dpp = 1;
		}

		if(dpp > 0)
		{
			image->create(M_UBYTE, width, height, dpp);
			glGetTexImage(GL_TEXTURE_2D, level, format, GL_UNSIGNED_BYTE, image->getData());
		}
	}
}

// frame buffer
void CHECK_FRAMEBUFFER_STATUS()
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	switch(status){
		case GL_FRAMEBUFFER_COMPLETE:
            break;
          case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("couldn't find a supported config\n");
			break;
          default:
            printf("error");
	}
}


void MGLContext::createFrameBuffer(unsigned int * frameBufferId){
	glGenFramebuffersEXT(1, frameBufferId);
}
void MGLContext::deleteFrameBuffer(unsigned int * frameBufferId){
	glDeleteFramebuffersEXT(1, frameBufferId);
}
void MGLContext::bindFrameBuffer(unsigned int frameBufferId){
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferId);
	m_currentFrameBuffer = frameBufferId;
}
void MGLContext::getCurrentFrameBuffer(unsigned int * frameBufferId){
	(*frameBufferId) = m_currentFrameBuffer;
}
void MGLContext::attachFrameBufferTexture(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId){
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, returnAttachType(attachment), GL_TEXTURE_2D, textureId, 0);
}
void MGLContext::attachFrameBufferRB(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int renderBufferId){
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, returnAttachType(attachment), GL_RENDERBUFFER_EXT, renderBufferId);

	CHECK_FRAMEBUFFER_STATUS();
}
void MGLContext::setDrawingBuffers(M_FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size)
{
	if(size == 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		static GLenum glBuffers[8];

		size = MIN(8, size);
		for(unsigned int i=0; i<size; i++)
			glBuffers[i] = returnAttachType(buffers[i]);

		glDrawBuffers(size, glBuffers);

		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
}

// render buffer
void MGLContext::createRenderBuffer(unsigned int * renderBufferId){
	glGenRenderbuffersEXT(1, renderBufferId);
}

void MGLContext::deleteRenderBuffer(unsigned int * renderBufferId){
	glDeleteRenderbuffersEXT(1, renderBufferId);
}

void MGLContext::bindRenderBuffer(unsigned int renderBufferId){
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderBufferId);
}

void MGLContext::setRenderBuffer(M_RENDER_BUFFER_MODES mode, unsigned int width, unsigned int height)
{
	GLenum internalMode;

	switch(mode)
	{
		default:
			return;
		case M_RENDER_DEPTH:
			internalMode = GL_DEPTH_COMPONENT;
			break;
		case M_RENDER_STENCIL:
			internalMode = GL_STENCIL_INDEX;
			break;
		case M_RENDER_DEPTH_STENCIL:
			internalMode = GL_DEPTH_STENCIL_EXT;
			break;
	}

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);
}

// shaders
void MGLContext::createVertexShader(unsigned int * shaderId)
{
	for(unsigned int i=1; i<MAX_MRCSHADERS; i++)
	if(g_shaderIds[i] == 0)
	{
		*shaderId = i;
		g_shaderIds[i] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		return;
	}
	
	// no free space
	*shaderId = 0;
}

void MGLContext::createPixelShader(unsigned int * shaderId)
{
	for(unsigned int i=1; i<MAX_MRCSHADERS; i++)
	if(g_shaderIds[i] == 0)
	{
		*shaderId = i;
		g_shaderIds[i] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		return;
	}
	
	// no free space
	*shaderId = 0;
}

void MGLContext::deleteShader(unsigned int * shaderId)
{
	unsigned int id = *shaderId;
	if(id > 0)
	{
		glDeleteObjectARB(g_shaderIds[id]);
		g_shaderIds[id] = 0;
		*shaderId = 0;
	}
}

void MGLContext::sendShaderSource(unsigned int shaderId, const char * source)
{
	GLhandleARB object = g_shaderIds[shaderId];

	glShaderSourceARB(object, 1, &source, NULL);
	glCompileShaderARB(object);

	GLint compiled;
	glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
	if(!compiled)
	{
		char shader_link_error[4096];
        glGetInfoLogARB(object, sizeof(shader_link_error), NULL, shader_link_error);
		MLOG_ERROR(shader_link_error);
	}
}

// FX
void MGLContext::bindFX(unsigned int fxId)
{
	glUseProgramObjectARB(g_fxs[fxId]);
}

void MGLContext::createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId)
{
	for(unsigned int i=1; i<MAX_MRCFXS; i++)
	if(g_fxs[i] == 0)
	{
		*fxId = i;
		GLhandleARB object = glCreateProgramObjectARB();
		g_fxs[i] = object;
		
		glAttachObjectARB(object, g_shaderIds[vertexShaderId]);
		glAttachObjectARB(object, g_shaderIds[pixelShaderId]);
		glLinkProgramARB(object);
		return;
	}
	
	// no free space
	*fxId = 0;
}

void MGLContext::updateFX(unsigned int fxId)
{
	glLinkProgramARB(g_fxs[fxId]);
}

void MGLContext::deleteFX(unsigned int * fxId)
{
	unsigned int id = *fxId;
	if(id > 0)
	{
		glDeleteObjectARB(g_fxs[id]);
		g_fxs[id] = 0;
		*fxId = 0;
	}
}

void MGLContext::sendUniformInt(unsigned int fxId, const char * name, int * values, const int count){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniform1ivARB(uValue, count, values);
}

void MGLContext::sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniform1fvARB(uValue, count, values);
}

void MGLContext::sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniform2fvARB(uValue, count, values);
}

void MGLContext::sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniform3fvARB(uValue, count, values);
}

void MGLContext::sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniform4fvARB(uValue, count, values);
}

void MGLContext::sendUniformMatrix(unsigned int fxId, const char * name, MMatrix4x4 * matrix, const int count, const bool transpose){
	GLint uValue = glGetUniformLocationARB(g_fxs[fxId], name);
	if(uValue != -1) glUniformMatrix4fvARB(uValue, count, transpose, matrix->entries);
}

void MGLContext::getAttribLocation(unsigned int fxId, const char * name, int * location)
{
	(*location) = glGetAttribLocationARB(g_fxs[fxId], name);
}

// VBO
void MGLContext::createVBO(unsigned int * vboId){
	glGenBuffersARB(1, vboId);
}
void MGLContext::deleteVBO(unsigned int * vboId){
	glDeleteBuffersARB(1, vboId);
}
void MGLContext::bindVBO(M_VBO_TYPES type, unsigned int vboId)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	glBindBufferARB(gltype, vboId);
}
void MGLContext::setVBO(M_VBO_TYPES type, const void * data, unsigned int size, M_VBO_MODES mode)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	switch(mode)
	{
		case M_VBO_STATIC:
			glBufferDataARB(gltype, size, data, GL_STATIC_DRAW_ARB);
			break;
		case M_VBO_DYNAMIC:
			glBufferDataARB(gltype, size, data, GL_DYNAMIC_DRAW_ARB);
			break;
		case M_VBO_STREAM:
			glBufferDataARB(gltype, size, data, GL_STREAM_DRAW_ARB);
			break;
	}
}
void MGLContext::setVBOSubData(M_VBO_TYPES type, unsigned int offset, const void * data, unsigned int size)
{
	GLenum gltype = type == M_VBO_ARRAY ? GL_ARRAY_BUFFER_ARB : GL_ELEMENT_ARRAY_BUFFER_ARB;
	glBufferSubDataARB(gltype, offset, size, data);
}

// arrays
void MGLContext::enableVertexArray(void){
	glEnableClientState(GL_VERTEX_ARRAY);
}

void MGLContext::enableColorArray(void){
	glEnableClientState(GL_COLOR_ARRAY);
}

void MGLContext::enableNormalArray(void){
	glEnableClientState(GL_NORMAL_ARRAY);
}

void MGLContext::enableTexCoordArray(void){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MGLContext::enableAttribArray(unsigned int location)
{
	glEnableVertexAttribArray(location);
}

void MGLContext::disableVertexArray(void){
	glDisableClientState(GL_VERTEX_ARRAY);
}

void MGLContext::disableColorArray(void){
	glDisableClientState(GL_COLOR_ARRAY);
}

void MGLContext::disableNormalArray(void){
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MGLContext::disableTexCoordArray(void){
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MGLContext::disableAttribArray(unsigned int location)
{
	glDisableVertexAttribArray(location);
}

void MGLContext::setVertexPointer(M_TYPES type, unsigned int components, const void * pointer){
	glVertexPointer(components, returnGLType(type), 0, pointer);
}

void MGLContext::setColorPointer(M_TYPES type, unsigned int components, const void * pointer){
	glColorPointer(components, returnGLType(type), 0, pointer);
}

void MGLContext::setNormalPointer(M_TYPES type, const void * pointer){
	glNormalPointer(returnGLType(type), 0, pointer);
}

void MGLContext::setTexCoordPointer(M_TYPES type, unsigned int components, const void * pointer){
	glTexCoordPointer(components, returnGLType(type), 0, pointer);
}

void MGLContext::setAttribPointer(unsigned int location, M_TYPES type, unsigned int components, const void * pointer, const bool normalized)
{
	glVertexAttribPointer(location, components, returnGLType(type), normalized, 0, pointer);
}

// draw
void MGLContext::drawArray(M_PRIMITIVE_TYPES type, unsigned int begin, unsigned int size)
{
	glDrawArrays(returnPrimitiveType(type), begin, size);
}

void MGLContext::drawElement(M_PRIMITIVE_TYPES type, unsigned int size, M_TYPES indicesType, const void * indices)
{
	glDrawElements(returnPrimitiveType(type), size, returnGLType(indicesType), indices);
}

// lines
void MGLContext::enableLineAntialiasing(void)	{ glEnable(GL_LINE_SMOOTH); }
void MGLContext::disableLineAntialiasing(void)	{ glDisable(GL_LINE_SMOOTH); }

// material
void MGLContext::setMaterialDiffuse(const MVector4 & diffuse){
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}
void MGLContext::setMaterialSpecular(const MVector4 & specular){
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
void MGLContext::setMaterialAmbient(const MVector4 & ambient){
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void MGLContext::setMaterialEmit(const MVector4 & emit){
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
}
void MGLContext::setMaterialShininess(float shininess){
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// scissor
void MGLContext::enableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }
void MGLContext::disableScissorTest(void){ glDisable(GL_SCISSOR_TEST); }
void MGLContext::setScissor(int x, int y, unsigned int width, unsigned int height){
	glScissor(x, y, width, height);
}

// color
void MGLContext::setColor(const MColor & color){
	glColor4ub(color.r, color.g, color.b, color.a);
}

void MGLContext::setColor4(const MVector4 & color){
	glColor4f(color.x, color.y, color.z, color.w);
}
void MGLContext::setColor3(const MVector3 & color){
	glColor4f(color.x, color.y, color.z, 1.0f);
}

// masks
void MGLContext::setColorMask(bool r, bool g, bool b, bool a){
	glColorMask(r, g, b, a);
}
void MGLContext::setDepthMask(bool depth){
	glDepthMask(depth);
}

void MGLContext::setAlphaTest(float value)
{
	if(value > 0.0f)
		glEnable(GL_ALPHA_TEST);
	else {
		glDisable(GL_ALPHA_TEST);
	}

	glAlphaFunc(GL_GREATER, value);
}

// depth
void MGLContext::enableDepthTest(void) { glEnable (GL_DEPTH_TEST); }
void MGLContext::disableDepthTest(void){ glDisable(GL_DEPTH_TEST); }
void MGLContext::setDepthMode(M_DEPTH_MODES mode)
{
	switch(mode)
	{
	case M_DEPTH_ALWAYS:
		glDepthFunc(GL_ALWAYS);
		break;

	case M_DEPTH_LESS:
		glDepthFunc(GL_LESS);
		break;

	case M_DEPTH_GREATER:
		glDepthFunc(GL_GREATER);
		break;

	case M_DEPTH_EQUAL:
		glDepthFunc(GL_EQUAL);
		break;

	case M_DEPTH_LEQUAL:
		glDepthFunc(GL_LEQUAL);
		break;

	case M_DEPTH_GEQUAL:
		glDepthFunc(GL_GEQUAL);
		break;

	case M_DEPTH_NOTEQUAL:
		glDepthFunc(GL_NOTEQUAL);
		break;
	
	case M_DEPTH_NONE:
		glDepthFunc(GL_NEVER);
		break;
	}
}
void MGLContext::setPolygonOffset(float factor, float units)
{
	if(factor == 0 && units == 0)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(factor, units);
	}
}

// stencil
void MGLContext::enableStencilTest(void) { glEnable (GL_STENCIL_TEST); }
void MGLContext::disableStencilTest(void){ glDisable(GL_STENCIL_TEST); }
void MGLContext::setStencilFunc(M_STENCIL_FUNCS func, int ref)
{
	switch(func)
	{
		default:
		case M_STENCIL_ALWAYS:
			glStencilFunc(GL_ALWAYS, ref, ~0);
			break;
		case M_STENCIL_EQUAL:
			glStencilFunc(GL_EQUAL, ref, ~0);
			break;
		case M_STENCIL_NOTEQUAL:
			glStencilFunc(GL_NOTEQUAL, ref, ~0);
			break;
		case M_STENCIL_NEVER:
			glStencilFunc(GL_NEVER, ref, ~0);
			break;
		case M_STENCIL_LESS:
			glStencilFunc(GL_LESS, ref, ~0);
			break;
		case M_STENCIL_LEQUAL:
			glStencilFunc(GL_LEQUAL, ref, ~0);
			break;
		case M_STENCIL_GREATER:
			glStencilFunc(GL_GREATER, ref, ~0);
			break;
		case M_STENCIL_GEQUAL:
			glStencilFunc(GL_GEQUAL, ref, ~0);
			break;
	}
}
void MGLContext::setStencilOp(M_STENCIL_OPS op)
{
	switch(op)
	{
		default:
		case M_STENCIL_KEEP:
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			break;
		case M_STENCIL_INVERT:
			glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
			break;
		case M_STENCIL_DECR:
			glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
			break;
		case M_STENCIL_INCR:
			glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
			break;
		case M_STENCIL_REPLACE:
			glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
			break;
	}
}

// cull face
void MGLContext::enableCullFace(void) { glEnable (GL_CULL_FACE); }
void MGLContext::disableCullFace(void){ glDisable(GL_CULL_FACE); }
void MGLContext::setCullMode(M_CULL_MODES mode)
{
	switch(mode)
	{
	case M_CULL_FRONT:
		glCullFace(GL_FRONT);
		break;

	case M_CULL_BACK:
		glCullFace(GL_BACK);
		break;

	case M_CULL_FRONT_BACK:
		glCullFace(GL_FRONT_AND_BACK);
		break;
		
	case M_CULL_NONE:
		break;
	}
}

// occlusion
void MGLContext::createQuery(unsigned int * queryId){
	glGenQueriesARB(1, queryId);
}
void MGLContext::deleteQuery(unsigned int * queryId){
	glDeleteQueriesARB(1, queryId);
}
void MGLContext::beginQuery(unsigned int queryId){
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queryId);
}
void MGLContext::endQuery(void){
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}
void MGLContext::getQueryResult(unsigned int queryId, unsigned int * result){
	glGetQueryObjectuivARB(queryId, GL_QUERY_RESULT_ARB, result);
}
bool MGLContext::isQueryResultAvailable(unsigned int queryId){
	int result;
	glGetQueryObjectivARB(queryId, GL_QUERY_RESULT_AVAILABLE_ARB, &result);
	return result!=0;
}

// matrix
void MGLContext::loadIdentity(void){
	glLoadIdentity();
}

void MGLContext::setMatrixMode(M_MATRIX_MODES matrixMode)
{
	switch(matrixMode)
	{
	case M_MATRIX_MODELVIEW:
		glMatrixMode(GL_MODELVIEW);
		return;

	case M_MATRIX_PROJECTION:
		glMatrixMode(GL_PROJECTION);
		return;

	case M_MATRIX_TEXTURE:
		glMatrixMode(GL_TEXTURE);
		return;
	}
}

void MGLContext::pushMatrix(void)									{ glPushMatrix(); }
void MGLContext::popMatrix(void)									{ glPopMatrix(); }
void MGLContext::multMatrix(const MMatrix4x4 * matrix)				{ glMultMatrixf(matrix->entries); }
void MGLContext::translate(const MVector3 & position)				{ glTranslatef(position.x, position.y, position.z); }
void MGLContext::rotate(const MVector3 & axis, float angle)			{ glRotatef(angle, axis.x, axis.y, axis.z); }
void MGLContext::scale(const MVector3 & scale)						{ glScalef(scale.x, scale.y, scale.z); }
void MGLContext::getViewport(int * viewport)
{
	glGetIntegerv(GL_VIEWPORT, viewport);
}
void MGLContext::getModelViewMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix->entries);
}
void MGLContext::getProjectionMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_PROJECTION_MATRIX, matrix->entries);
}
void MGLContext::getTextureMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_TEXTURE_MATRIX, matrix->entries);
}

// fog
void MGLContext::enableFog(void)						{ glEnable (GL_FOG); }
void MGLContext::disableFog(void)						{ glDisable(GL_FOG); }
void MGLContext::setFogColor(const MVector3 & color)
{
	glFogfv(GL_FOG_COLOR, MVector4(color));
	m_fogColor = color;
}
void MGLContext::setFogDistance(float min, float max)
{
	glFogf(GL_FOG_START, min);
	glFogf(GL_FOG_END,   max);
	m_fogMin = min;
	m_fogMax = max;
}
void MGLContext::getFogColor(MVector3 * color){
	(*color) = m_fogColor;
}
void MGLContext::getFogDistance(float * min, float * max){
	(*min) = m_fogMin;
	(*max) = m_fogMax;
}

// lighting
void MGLContext::enableLighting(void)			{ glEnable (GL_LIGHTING); }
void MGLContext::disableLighting(void)			{ glDisable(GL_LIGHTING); }
void MGLContext::enableLight(unsigned int id)	{ glEnable (GL_LIGHT0 + id); }
void MGLContext::disableLight(unsigned int id)	{ glDisable(GL_LIGHT0 + id); }
void MGLContext::setLightPosition(unsigned int id, const MVector4 & position)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].position = position;
		glLightfv(GL_LIGHT0 + id, GL_POSITION, position);
	}
}
void MGLContext::setLightDiffuse(unsigned int id, const MVector4 & diffuse)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].diffuse = diffuse;
		glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, diffuse);
	}
}
void MGLContext::setLightSpecular(unsigned int id, const MVector4 & specular)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].specular = specular;
		glLightfv(GL_LIGHT0 + id, GL_SPECULAR, specular);
	}
}
void MGLContext::setLightAmbient(unsigned int id, const MVector4 & ambient)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].ambient = ambient;
		glLightfv(GL_LIGHT0 + id, GL_AMBIENT, ambient);
	}
}
void MGLContext::setLightAttenuation(unsigned int id, float constant, float linear, float quadratic)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].constant = constant;
		m_lights[id].linear = linear;
		m_lights[id].quadratic = quadratic;
		glLightf(GL_LIGHT0 + id, GL_CONSTANT_ATTENUATION, constant);
		glLightf(GL_LIGHT0 + id, GL_LINEAR_ATTENUATION, linear);
		glLightf(GL_LIGHT0 + id, GL_QUADRATIC_ATTENUATION, quadratic);
	}
}
void MGLContext::setLightSpotDirection(unsigned int id, const MVector3 & direction)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].direction = direction;
		glLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);
	}
}
void MGLContext::setLightSpotAngle(unsigned int id, float angle)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].angle = angle;
		glLightf(GL_LIGHT0 + id, GL_SPOT_CUTOFF, angle);
	}
}
void MGLContext::setLightSpotExponent(unsigned int id, float exponent)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].exponent = exponent*128.0f;
		glLightf(GL_LIGHT0 + id, GL_SPOT_EXPONENT, m_lights[id].exponent);
	}
}
void MGLContext::getLightPosition(unsigned int id, MVector4 * position)
{
	if(id < MAX_MRCLIGHTS)
		(*position) = m_lights[id].position;
}
void MGLContext::getLightDiffuse(unsigned int id, MVector4 * diffuse)
{
	if(id < MAX_MRCLIGHTS)
		(*diffuse) = m_lights[id].diffuse;
}
void MGLContext::getLightSpecular(unsigned int id, MVector4 * specular)
{
	if(id < MAX_MRCLIGHTS)
		(*specular) = m_lights[id].specular;
}
void MGLContext::getLightAmbient(unsigned int id, MVector4 * ambient)
{
	if(id < MAX_MRCLIGHTS)
		(*ambient) = m_lights[id].ambient;
}
void MGLContext::getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic)
{
	if(id < MAX_MRCLIGHTS)
	{
		(*constant) = m_lights[id].constant;
		(*linear) = m_lights[id].linear;
		(*quadratic) = m_lights[id].quadratic;
	}
}
void MGLContext::getLightSpotDirection(unsigned int id, MVector3 * direction)
{
	if(id < MAX_MRCLIGHTS)
		(*direction) = m_lights[id].direction;
}
void MGLContext::getLightSpotAngle(unsigned int id, float * angle)
{
	if(id < MAX_MRCLIGHTS)
		(*angle) = m_lights[id].angle;
}
void MGLContext::getLightSpotExponent(unsigned int id, float * exponent)
{
	if(id < MAX_MRCLIGHTS)
		(*exponent) = m_lights[id].exponent;
}

// blending
void MGLContext::enableBlending(void)	{ glEnable (GL_BLEND); }
void MGLContext::disableBlending(void)	{ glDisable(GL_BLEND); }
void MGLContext::setBlendingMode(M_BLENDING_MODES mode)
{
	switch(mode)
	{
	case M_BLENDING_NONE:
		glBlendEquationEXT(GL_FUNC_ADD_EXT);
		glBlendFunc(GL_ONE, GL_ZERO);
		break;
	case M_BLENDING_ALPHA:
		glBlendEquationEXT(GL_FUNC_ADD_EXT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case M_BLENDING_ADD:
		glBlendEquationEXT(GL_FUNC_ADD_EXT);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case M_BLENDING_PRODUCT:
		glBlendEquationEXT(GL_FUNC_ADD_EXT);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		break;
	case M_BLENDING_LIGHT:
		glBlendEquationEXT(GL_FUNC_ADD_EXT);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;
	case M_BLENDING_SUB:
		glBlendEquationEXT(GL_FUNC_SUBTRACT_EXT);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	}
}
