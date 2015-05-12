/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MES1Context.cpp
//
// OpenGL-ES1 Rendering Context
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2011 Anael Seghezzi <www.maratis3d.com>
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


#ifdef __APPLE__
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#endif

#include "MES1Context.h"


static int g_GLversion = 0;
static float maxAnisotropy = 0.0f;


static GLenum returnGLType(M_TYPES type)
{
	switch(type)
	{
        default:
            //	return GL_NONE;
            
            //case M_BOOL:
            //return GL_BOOL;
            
        case M_BYTE:
            return GL_BYTE;
            
        case M_UBYTE:
            return GL_UNSIGNED_BYTE;
            
        case M_SHORT:
            return GL_SHORT;
            
        case M_USHORT:
            return GL_UNSIGNED_SHORT;
            
            //case M_INT:
            //return GL_INT;
            
            //case M_UINT:
            //return GL_UNSIGNED_INT;
            
        case M_FLOAT:
            return GL_FLOAT;
            
            //case M_DOUBLE:
            //return GL_DOUBLE;
	}
}

static GLenum returnPrimitiveType(M_PRIMITIVE_TYPES type)
{
	switch(type)
	{
        default:
            //return GL_NONE;
            
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
	}
}

static GLenum returnTexFilterMode(M_TEX_FILTER_MODES mode)
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

static GLenum returnTexMode(M_TEX_MODES mode)
{
	switch(mode)
	{
            //case M_R:
            //	return GL_R;
            
            //case M_RG:
            //	return GL_RG;
            
        default:
        case M_RGB:
            return GL_RGB;
            
        case M_RGBA:
            return GL_RGBA;
            
            //case M_DEPTH:
            //	return GL_DEPTH_COMPONENT;
	}
}

static GLenum returnAttachType(M_FRAME_BUFFER_ATTACHMENT type)
{
	if(type == M_ATTACH_DEPTH)
		return GL_DEPTH_ATTACHMENT_OES;
	else if(type == M_ATTACH_STENCIL)
		return GL_STENCIL_ATTACHMENT_OES;
	else
		return GL_COLOR_ATTACHMENT0_OES + ((int)type - 2);
}

MES1Context::MES1Context(void):
m_currentFrameBuffer(0)
{
	// version
	const char * version = (const char *)glGetString(GL_VERSION);
	if(version)
	{
		m_gl_version=version;
		sscanf(version, "%d", &g_GLversion);
	}
    
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
	glClearDepthf(1.0f);
    
	// line
	glLineWidth(1);
    
	// stencil
	glClearStencil(0);
    
	// pixel pack/unpack
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	// anisotropic filtering
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
}

MES1Context::~MES1Context(void)
{}

// view
void MES1Context::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
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

void MES1Context::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar){
	glOrthof(left, right, bottom, top, zNear, zFar);
}

// viewport
void MES1Context::setViewport(int x, int y, unsigned int width, unsigned int height){
	glViewport(x, y, width, height);
}

// clear
void MES1Context::clear(int buffer)
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

void MES1Context::setClearColor(const MVector4 & color){
	glClearColor(color.x, color.y, color.z, color.w);
}

// texture
void MES1Context::enableTexture(void)
{
	glEnable(GL_TEXTURE_2D);
}

void MES1Context::disableTexture(void)
{
	glDisable(GL_TEXTURE_2D);
}

void MES1Context::setTextureGenMode(M_TEX_GEN_MODES mode)
{
}

void MES1Context::setTextureFilterMode(M_TEX_FILTER_MODES min, M_TEX_FILTER_MODES mag)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, returnTexFilterMode(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, returnTexFilterMode(mag));
}

void MES1Context::setTextureUWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
}

void MES1Context::setTextureVWrapMode(M_WRAP_MODES wrap)
{
	int glWrap = GL_REPEAT;
	if(wrap == M_WRAP_CLAMP)
		glWrap = GL_CLAMP_TO_EDGE;
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
}

void MES1Context::setTextureCombineMode(M_TEX_COMBINE_MODES combine)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    
	switch(combine)
	{
        case M_TEX_COMBINE_REPLACE:
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
            break;
            
        case M_TEX_COMBINE_MODULATE:
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            break;
            
        case M_TEX_COMBINE_ADD:
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
            break;
            
        case M_TEX_COMBINE_SUB:
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
            break;
            
        default:
            break;
	}
}

void MES1Context::bindTexture(unsigned int textureId, unsigned int multitextureId)
{
	glActiveTexture(GL_TEXTURE0 + multitextureId);
	glClientActiveTexture(GL_TEXTURE0 + multitextureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void MES1Context::createTexture(unsigned int * textureId)
{
	glGenTextures(1, textureId);
}

void MES1Context::deleteTexture(unsigned int * textureId)
{
	glDeleteTextures(1, textureId);
}

void MES1Context::sendTextureImage(MImage * image, bool mipMap, bool filter, bool compress)
{
	// get properties
	unsigned int bytePerPix = image->getComponents();
    
	unsigned int width  = image->getWidth();
	unsigned int height = image->getHeight();
    
	int internalFormat = GL_RGB;
	int format = GL_RGB;
    
	/*if(compress)
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
     else*/
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
    
	glTexImage2D(glType, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, image->getData());
	if(mipMap)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
		glGenerateMipmapOES(glType);
	}
}

void MES1Context::texImage(unsigned int level, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	GLenum intFormat = format;
	//if(type == M_FLOAT && mode == M_RGB)
	//	intFormat = GL_RGB32F_ARB;
	
	glTexImage2D(GL_TEXTURE_2D, level, intFormat, width, height, 0, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void MES1Context::texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels)
{
	GLenum format = returnTexMode(mode);
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, returnGLType(type), pixels);
	if(level > 0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); // anisotropic filtering
}

void MES1Context::generateMipMap(void){
	glGenerateMipmapOES(GL_TEXTURE_2D);
}

// frame buffer
void MES1Context::createFrameBuffer(unsigned int * frameBufferId){
	glGenFramebuffersOES(1, frameBufferId);
}
void MES1Context::deleteFrameBuffer(unsigned int * frameBufferId){
	glDeleteFramebuffersOES(1, frameBufferId);
}
void MES1Context::bindFrameBuffer(unsigned int frameBufferId){
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, frameBufferId);
	m_currentFrameBuffer = frameBufferId;
}
void MES1Context::getCurrentFrameBuffer(unsigned int * frameBufferId){
	(*frameBufferId) = m_currentFrameBuffer;
}
void MES1Context::attachFrameBufferTexture(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId){
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, returnAttachType(attachment), GL_TEXTURE_2D, textureId, 0);
}
void MES1Context::setDrawingBuffers(M_FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size)
{
	/*
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
     }*/
}

// shaders
void MES1Context::createVertexShader(unsigned int * shaderId){
    //	*shaderId = (unsigned int)(unsigned long)glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
}

void MES1Context::createPixelShader(unsigned int * shaderId){
    //	*shaderId = (unsigned int)(unsigned long)glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
}

void MES1Context::deleteShader(unsigned int * shaderId){
	//glDeleteObjectARB((GLhandleARB)(*shaderId));
}

void MES1Context::sendShaderSource(unsigned int shaderId, const char * source)
{
	/*glShaderSourceARB((GLhandleARB)shaderId, 1, &source, NULL);
     glCompileShaderARB((GLhandleARB)shaderId);
     
     GLint compiled;
     glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
     if(!compiled)
     {
     printf("ERROR OpenGL : unable to compile shader\n");
     char shader_link_error[4096];
     glGetInfoLogARB((GLhandleARB)shaderId, sizeof(shader_link_error), NULL, shader_link_error);
     printf("%s", shader_link_error);
     }*/
}

// FX
void MES1Context::bindFX(unsigned int fxId){
	//glUseProgramObjectARB((GLhandleARB)fxId);
}

void MES1Context::createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId)
{
	/*
     *fxId = (unsigned int)(unsigned long)glCreateProgramObjectARB();
     glAttachObjectARB((GLhandleARB)*fxId, (GLhandleARB)vertexShaderId);
     glAttachObjectARB((GLhandleARB)*fxId, (GLhandleARB)pixelShaderId);
     glLinkProgramARB((GLhandleARB)*fxId);*/
}

void MES1Context::updateFX(unsigned int fxId)
{
	//glLinkProgramARB((GLhandleARB)fxId);
}

void MES1Context::deleteFX(unsigned int * fxId){
	//glDeleteObjectARB((GLhandleARB)(*fxId));
}

void MES1Context::sendUniformInt(unsigned int fxId, const char * name, int * values, const int count){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniform1ivARB(uValue, count, values);
}

void MES1Context::sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniform1fvARB(uValue, count, values);
}

void MES1Context::sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniform2fvARB(uValue, count, values);
}

void MES1Context::sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniform3fvARB(uValue, count, values);
}

void MES1Context::sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniform4fvARB(uValue, count, values);
}

void MES1Context::sendUniformMatrix(unsigned int fxId, const char * name, MMatrix4x4 * matrix, const int count, const bool transpose){
	//GLint uValue = glGetUniformLocationARB((GLhandleARB)fxId, name);
	//if(uValue != -1) glUniformMatrix4fvARB(uValue, count, transpose, matrix->entries);
}

void MES1Context::getAttribLocation(unsigned int fxId, const char * name, int * location)
{
	//(*location) = glGetAttribLocationARB((GLhandleARB)fxId, name);
}

// arrays
void MES1Context::enableVertexArray(void){
	glEnableClientState(GL_VERTEX_ARRAY);
}

void MES1Context::enableColorArray(void){
	glEnableClientState(GL_COLOR_ARRAY);
}

void MES1Context::enableNormalArray(void){
	glEnableClientState(GL_NORMAL_ARRAY);
}

void MES1Context::enableTexCoordArray(void){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MES1Context::enableAttribArray(unsigned int location)
{
	//glEnableVertexAttribArray(location);
}

void MES1Context::disableVertexArray(void){
	glDisableClientState(GL_VERTEX_ARRAY);
}

void MES1Context::disableColorArray(void){
	glDisableClientState(GL_COLOR_ARRAY);
}

void MES1Context::disableNormalArray(void){
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MES1Context::disableTexCoordArray(void){
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MES1Context::disableAttribArray(unsigned int location)
{
	//glDisableVertexAttribArray(location);
}

void MES1Context::setVertexPointer(M_TYPES type, unsigned int components, const void * pointer){
	glVertexPointer(components, returnGLType(type), 0, pointer);
}

void MES1Context::setColorPointer(M_TYPES type, unsigned int components, const void * pointer){
	glColorPointer(components, returnGLType(type), 0, pointer);
}

void MES1Context::setNormalPointer(M_TYPES type, const void * pointer){
	glNormalPointer(returnGLType(type), 0, pointer);
}

void MES1Context::setTexCoordPointer(M_TYPES type, unsigned int components, const void * pointer){
	glTexCoordPointer(components, returnGLType(type), 0, pointer);
}

void MES1Context::setAttribPointer(unsigned int location, M_TYPES type, unsigned int components, const void * pointer, const bool normalized)
{
	//glVertexAttribPointer(location, components, returnGLType(type), normalized, 0, pointer);
}

// draw
void MES1Context::drawArray(M_PRIMITIVE_TYPES type, unsigned int begin, unsigned int size)
{
	glDrawArrays(returnPrimitiveType(type), begin, size);
}

void MES1Context::drawElement(M_PRIMITIVE_TYPES type, unsigned int size, M_TYPES indicesType, const void * indices)
{
	glDrawElements(returnPrimitiveType(type), size, returnGLType(indicesType), indices);
}

// lines
void MES1Context::enableLineAntialiasing(void)	{ glEnable(GL_LINE_SMOOTH); }
void MES1Context::disableLineAntialiasing(void)	{ glDisable(GL_LINE_SMOOTH); }

// material
void MES1Context::setMaterialDiffuse(const MVector4 & diffuse){
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}
void MES1Context::setMaterialSpecular(const MVector4 & specular){
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
void MES1Context::setMaterialAmbient(const MVector4 & ambient){
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
}
void MES1Context::setMaterialEmit(const MVector4 & emit){
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
}
void MES1Context::setMaterialShininess(float shininess){
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// scissor
void MES1Context::enableScissorTest(void) { glEnable(GL_SCISSOR_TEST); }
void MES1Context::disableScissorTest(void){ glDisable(GL_SCISSOR_TEST); }
void MES1Context::setScissor(int x, int y, unsigned int width, unsigned int height){
	glScissor(x, y, width, height);
}

// color
void MES1Context::setColor(const MColor & color){
	glColor4ub(color.r, color.g, color.b, color.a);
}

void MES1Context::setColor4(const MVector4 & color){
	glColor4f(color.x, color.y, color.z, color.w);
}
void MES1Context::setColor3(const MVector3 & color){
	glColor4f(color.x, color.y, color.z, 1.0f);
}

// masks
void MES1Context::setColorMask(bool r, bool g, bool b, bool a){
	glColorMask(r, g, b, a);
}
void MES1Context::setDepthMask(bool depth){
	glDepthMask(depth);
}

void MES1Context::setAlphaTest(float value)
{
	if(value > 0.0f)
		glEnable(GL_ALPHA_TEST);
	else {
		glDisable(GL_ALPHA_TEST);
	}
    
	glAlphaFunc(GL_GREATER, value);
}

// depth
void MES1Context::enableDepthTest(void) { glEnable (GL_DEPTH_TEST); }
void MES1Context::disableDepthTest(void){ glDisable(GL_DEPTH_TEST); }
void MES1Context::setDepthMode(M_DEPTH_MODES mode)
{
	switch(mode)
	{
        case M_DEPTH_ALWAYS:
            glDepthFunc(GL_ALWAYS);
            return;
            
        case M_DEPTH_LESS:
            glDepthFunc(GL_LESS);
            return;
            
        case M_DEPTH_GREATER:
            glDepthFunc(GL_GREATER);
            return;
            
        case M_DEPTH_EQUAL:
            glDepthFunc(GL_EQUAL);
            return;
            
        case M_DEPTH_LEQUAL:
            glDepthFunc(GL_LEQUAL);
            return;
            
        case M_DEPTH_GEQUAL:
            glDepthFunc(GL_GEQUAL);
            return;
            
        case M_DEPTH_NOTEQUAL:
            glDepthFunc(GL_NOTEQUAL);
            return;
            
        default:
            break;
	}
}

// stencil
void MES1Context::enableStencilTest(void) { glEnable (GL_STENCIL_TEST); }
void MES1Context::disableStencilTest(void){ glDisable(GL_STENCIL_TEST); }

// cull face
void MES1Context::enableCullFace(void) { glEnable (GL_CULL_FACE); }
void MES1Context::disableCullFace(void){ glDisable(GL_CULL_FACE); }
void MES1Context::setCullMode(M_CULL_MODES mode)
{
	switch(mode)
	{
        case M_CULL_FRONT:
            glCullFace(GL_FRONT);
            return;
            
        case M_CULL_BACK:
            glCullFace(GL_BACK);
            return;
            
        case M_CULL_FRONT_BACK:
            glCullFace(GL_FRONT_AND_BACK);
            return;
            
        default:
            break;
	}
}

// matrix
void MES1Context::loadIdentity(void){
	glLoadIdentity();
}

void MES1Context::setMatrixMode(M_MATRIX_MODES matrixMode)
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

void MES1Context::pushMatrix(void)									{ glPushMatrix(); }
void MES1Context::popMatrix(void)									{ glPopMatrix(); }
void MES1Context::multMatrix(const MMatrix4x4 * matrix)				{ glMultMatrixf(matrix->entries); }
void MES1Context::translate(const MVector3 & position)				{ glTranslatef(position.x, position.y, position.z); }
void MES1Context::rotate(const MVector3 & axis, float angle)			{ glRotatef(angle, axis.x, axis.y, axis.z); }
void MES1Context::scale(const MVector3 & scale)						{ glScalef(scale.x, scale.y, scale.z); }
void MES1Context::getViewport(int * viewport)
{
	glGetIntegerv(GL_VIEWPORT, viewport);
}
void MES1Context::getModelViewMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix->entries);
}
void MES1Context::getProjectionMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_PROJECTION_MATRIX, matrix->entries);
}
void MES1Context::getTextureMatrix(MMatrix4x4 * matrix)
{
	glGetFloatv(GL_TEXTURE_MATRIX, matrix->entries);
}

// fog
void MES1Context::enableFog(void)						{ glEnable (GL_FOG); }
void MES1Context::disableFog(void)						{ glDisable(GL_FOG); }
void MES1Context::setFogColor(const MVector3 & color)
{
	glFogfv(GL_FOG_COLOR, MVector4(color));
	m_fogColor = color;
}
void MES1Context::setFogDistance(float min, float max)
{
	glFogf(GL_FOG_START, min);
	glFogf(GL_FOG_END,   max);
	m_fogMin = min;
	m_fogMax = max;
}
void MES1Context::getFogColor(MVector3 * color){
	(*color) = m_fogColor;
}
void MES1Context::getFogDistance(float * min, float * max){
	(*min) = m_fogMin;
	(*max) = m_fogMax;
}

// lighting
void MES1Context::enableLighting(void)			{ glEnable (GL_LIGHTING); }
void MES1Context::disableLighting(void)			{ glDisable(GL_LIGHTING); }
void MES1Context::enableLight(unsigned int id)	{ glEnable (GL_LIGHT0 + id); }
void MES1Context::disableLight(unsigned int id)	{ glDisable(GL_LIGHT0 + id); }
void MES1Context::setLightPosition(unsigned int id, const MVector4 & position)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].position = position;
		glLightfv(GL_LIGHT0 + id, GL_POSITION, position);
	}
}
void MES1Context::setLightDiffuse(unsigned int id, const MVector4 & diffuse)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].diffuse = diffuse;
		glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, diffuse);
	}
}
void MES1Context::setLightSpecular(unsigned int id, const MVector4 & specular)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].specular = specular;
		glLightfv(GL_LIGHT0 + id, GL_SPECULAR, specular);
	}
}
void MES1Context::setLightAmbient(unsigned int id, const MVector4 & ambient)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].ambient = ambient;
		glLightfv(GL_LIGHT0 + id, GL_AMBIENT, ambient);
	}
}
void MES1Context::setLightAttenuation(unsigned int id, float constant, float linear, float quadratic)
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
void MES1Context::setLightSpotDirection(unsigned int id, const MVector3 & direction)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].direction = direction;
		glLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);
	}
}
void MES1Context::setLightSpotAngle(unsigned int id, float angle)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].angle = angle;
		glLightf(GL_LIGHT0 + id, GL_SPOT_CUTOFF, angle);
	}
}
void MES1Context::setLightSpotExponent(unsigned int id, float exponent)
{
	if(id < MAX_MRCLIGHTS)
	{
		m_lights[id].exponent = exponent*128.0f;
		glLightf(GL_LIGHT0 + id, GL_SPOT_EXPONENT, m_lights[id].exponent);
	}
}
void MES1Context::getLightPosition(unsigned int id, MVector4 * position)
{
	if(id < MAX_MRCLIGHTS)
		(*position) = m_lights[id].position;
}
void MES1Context::getLightDiffuse(unsigned int id, MVector4 * diffuse)
{
	if(id < MAX_MRCLIGHTS)
		(*diffuse) = m_lights[id].diffuse;
}
void MES1Context::getLightSpecular(unsigned int id, MVector4 * specular)
{
	if(id < MAX_MRCLIGHTS)
		(*specular) = m_lights[id].specular;
}
void MES1Context::getLightAmbient(unsigned int id, MVector4 * ambient)
{
	if(id < MAX_MRCLIGHTS)
		(*ambient) = m_lights[id].ambient;
}
void MES1Context::getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic)
{
	if(id < MAX_MRCLIGHTS)
	{
		(*constant) = m_lights[id].constant;
		(*linear) = m_lights[id].linear;
		(*quadratic) = m_lights[id].quadratic;
	}
}
void MES1Context::getLightSpotDirection(unsigned int id, MVector3 * direction)
{
	if(id < MAX_MRCLIGHTS)
		(*direction) = m_lights[id].direction;
}
void MES1Context::getLightSpotAngle(unsigned int id, float * angle)
{
	if(id < MAX_MRCLIGHTS)
		(*angle) = m_lights[id].angle;
}
void MES1Context::getLightSpotExponent(unsigned int id, float * exponent)
{
	if(id < MAX_MRCLIGHTS)
		(*exponent) = m_lights[id].exponent;
}

// blending
void MES1Context::enableBlending(void)	{ glEnable (GL_BLEND); }
void MES1Context::disableBlending(void)	{ glDisable(GL_BLEND); }
void MES1Context::setBlendingMode(M_BLENDING_MODES mode)
{
	switch(mode)
	{
        case M_BLENDING_NONE:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;
            
        case M_BLENDING_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
            
        case M_BLENDING_ADD:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
            
        case M_BLENDING_PRODUCT:
            glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            break;
            
        case M_BLENDING_LIGHT:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;
            
        default:
            break;
	}
}
