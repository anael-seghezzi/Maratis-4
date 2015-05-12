/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MGLContext.h
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

#ifndef _M_GL_CONTEXT_H
#define _M_GL_CONTEXT_H

#define MAX_MRCLIGHTS 8


class MGLContext : public MRenderingContext
{
private:

	// version
    const char * m_gl_version;
	
	// frame buffer
	unsigned int m_currentFrameBuffer;

	// fog
	float m_fogMin, m_fogMax;
	MVector3 m_fogColor;

	// lights
	struct
	{
		MVector4 position;
		MVector4 diffuse;
		MVector4 specular;
		MVector4 ambient;
		MVector3 direction;
		float angle, exponent;
		float constant, linear, quadratic;
	}
	m_lights[MAX_MRCLIGHTS];

public:

	MGLContext(void);
	~MGLContext(void);

	// version
    const char * getRendererVersion(void) { return m_gl_version; }

	// view
	void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
	void setPerspectiveView(float fov, float ratio, float zNear, float zFar);

	// viewport
	void setViewport(int x, int y, unsigned int width, unsigned int height);

	// clear
	void clear(int buffer);
	void setClearColor(const MVector4 & color);

	// finish
	void finish(void);
	
	// texture
	void enableTexture(void);
	void disableTexture(void);
	void setTextureGenMode(M_TEX_GEN_MODES mode);
	void setTextureFilterMode(M_TEX_FILTER_MODES min, M_TEX_FILTER_MODES mag);
	void setTextureUWrapMode(M_WRAP_MODES wrap);
	void setTextureVWrapMode(M_WRAP_MODES wrap);
	void setTextureCombineMode(M_TEX_COMBINE_MODES combine);
	void bindTexture(unsigned int textureId, const unsigned int multitextureId = 0);
	void createTexture(unsigned int * textureId);
	void deleteTexture(unsigned int * textureId);
	void sendTextureImage(MImage * image, bool mipMap, bool filter, bool compress);
	void texImage(unsigned int level, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels);
	void texSubImage(unsigned int level, int xoffset, int yoffset, unsigned int width, unsigned int height, M_TYPES type, M_TEX_MODES mode, const void * pixels);
	void generateMipMap(void);
	void getTexImage(unsigned int level, MImage * image);

	// frame buffer
	void createFrameBuffer(unsigned int * frameBufferId);
	void deleteFrameBuffer(unsigned int * frameBufferId);
	void bindFrameBuffer(unsigned int frameBufferId);
	void getCurrentFrameBuffer(unsigned int * frameBufferId);
	void attachFrameBufferTexture(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int textureId);
	void attachFrameBufferRB(M_FRAME_BUFFER_ATTACHMENT attachment, unsigned int renderBufferId);
	void setDrawingBuffers(M_FRAME_BUFFER_ATTACHMENT * buffers, unsigned int size);

	// render buffer
	void createRenderBuffer(unsigned int * renderBufferId);
	void deleteRenderBuffer(unsigned int * renderBufferId);
	void bindRenderBuffer(unsigned int renderBufferId);
	void setRenderBuffer(M_RENDER_BUFFER_MODES mode, unsigned int width, unsigned int height);

	// shaders
	void createVertexShader(unsigned int * shaderId);
	void createPixelShader(unsigned int * shaderId);
	void deleteShader(unsigned int * shaderId);
	void sendShaderSource(unsigned int shaderId, const char * source);

	// FX
	void bindFX(unsigned int fxId);
	void createFX(unsigned int * fxId, unsigned int vertexShaderId, unsigned int pixelShaderId);
	void updateFX(unsigned int fxId);
	void deleteFX(unsigned int * fxId);
	void sendUniformInt(unsigned int fxId, const char * name, int * values, const int count = 1);
	void sendUniformFloat(unsigned int fxId, const char * name, float * values, const int count = 1);
    void sendUniformVec2(unsigned int fxId, const char * name, float * values, const int count = 1);
    void sendUniformVec3(unsigned int fxId, const char * name, float * values, const int count = 1);
	void sendUniformVec4(unsigned int fxId, const char * name, float * values, const int count = 1);
	void sendUniformMatrix(unsigned int fxId, const char * name, MMatrix4x4 * matrix, const int count = 1, const bool transpose = false);
	void getAttribLocation(unsigned int fxId, const char * name, int * location);

	// VBO
	void createVBO(unsigned int * vboId);
	void deleteVBO(unsigned int * vboId);
	void bindVBO(M_VBO_TYPES type, unsigned int vboId);
	void setVBO(M_VBO_TYPES type, const void * data, unsigned int size, M_VBO_MODES mode);
	void setVBOSubData(M_VBO_TYPES type, unsigned int offset, const void * data, unsigned int size);
	
	// arrays
	void enableVertexArray(void);
	void enableColorArray(void);
	void enableNormalArray(void);
	void enableTexCoordArray(void);
	void enableAttribArray(unsigned int location);
	void disableVertexArray(void);
	void disableColorArray(void);
	void disableNormalArray(void);
	void disableTexCoordArray(void);
	void disableAttribArray(unsigned int location);
	void setVertexPointer(M_TYPES type, unsigned int components, const void * pointer);
	void setColorPointer(M_TYPES type, unsigned int components, const void * pointer);
	void setNormalPointer(M_TYPES type, const void * pointer);
	void setTexCoordPointer(M_TYPES type, unsigned int components, const void * pointer);
	void setAttribPointer(unsigned int location, M_TYPES type, unsigned int components, const void * pointer, const bool normalized = false);

	// draw
	void drawArray(M_PRIMITIVE_TYPES type, unsigned int begin, unsigned int size);
	void drawElement(M_PRIMITIVE_TYPES type, unsigned int size, M_TYPES indicesType, const void * indices);

	// lines
	void enableLineAntialiasing(void);
	void disableLineAntialiasing(void);

	// material
	void setMaterialDiffuse(const MVector4 & diffuse);
	void setMaterialSpecular(const MVector4 & specular);
	void setMaterialAmbient(const MVector4 & ambient);
	void setMaterialEmit(const MVector4 & emit);
	void setMaterialShininess(float shininess);

	// scissor
	void enableScissorTest(void);
	void disableScissorTest(void);
	void setScissor(int x, int y, unsigned int width, unsigned int height);

	// color
	void setColor(const MColor & color);
	void setColor3(const MVector3 & color);
	void setColor4(const MVector4 & color);

	// masks
	void setColorMask(bool r, bool g, bool b, bool a);
	void setDepthMask(bool depth);

	// alpha
	void setAlphaTest(float value);

	// depth
	void enableDepthTest(void);
	void disableDepthTest(void);
	void setDepthMode(M_DEPTH_MODES mode);
	void setPolygonOffset(float factor, float units);

	// stencil
	void enableStencilTest(void);
	void disableStencilTest(void);
	void setStencilFunc(M_STENCIL_FUNCS func, int ref=0);
	void setStencilOp(M_STENCIL_OPS op);

	// cull face
	void enableCullFace(void);
	void disableCullFace(void);
	void setCullMode(M_CULL_MODES mode);

	// occlusion
	void createQuery(unsigned int * queryId);
	void deleteQuery(unsigned int * queryId);
	void beginQuery(unsigned int queryId);
	void endQuery(void);
	void getQueryResult(unsigned int queryId, unsigned int * result);
	bool isQueryResultAvailable(unsigned int queryId);

	// matrix
	void loadIdentity(void);
	void setMatrixMode(M_MATRIX_MODES mode);
	void pushMatrix(void);
	void popMatrix(void);
	void multMatrix(const MMatrix4x4 * matrix);
	void translate(const MVector3 & position);
	void rotate(const MVector3 & axis, float angle);
	void scale(const MVector3 & scale);
	void getViewport(int * viewport);
	void getModelViewMatrix(MMatrix4x4 * matrix);
	void getProjectionMatrix(MMatrix4x4 * matrix);
	void getTextureMatrix(MMatrix4x4 * matrix);

	// fog
	void enableFog(void);
	void disableFog(void);
	void setFogColor(const MVector3 & color);
	void setFogDistance(float min, float max);
	void getFogColor(MVector3 * color);
	void getFogDistance(float * min, float * max);

	// lighting
	void enableLighting(void);
	void disableLighting(void);
	void enableLight(unsigned int id);
	void disableLight(unsigned int id);
	void setLightPosition(unsigned int id, const MVector4 & position);
	void setLightDiffuse(unsigned int id, const MVector4 & diffuse);
	void setLightSpecular(unsigned int id, const MVector4 & specular);
	void setLightAmbient(unsigned int id, const MVector4 & ambient);
	void setLightAttenuation(unsigned int id, float constant, float linear, float quadratic);
	void setLightSpotDirection(unsigned int id, const MVector3 & direction);
	void setLightSpotAngle(unsigned int id, float angle);
	void setLightSpotExponent(unsigned int id, float exponent);
	void getLightPosition(unsigned int id, MVector4 * position);
	void getLightDiffuse(unsigned int id, MVector4 * diffuse);
	void getLightSpecular(unsigned int id, MVector4 * specular);
	void getLightAmbient(unsigned int id, MVector4 * ambient);
	void getLightAttenuation(unsigned int id, float * constant, float * linear, float * quadratic);
	void getLightSpotDirection(unsigned int id, MVector3 * direction);
	void getLightSpotAngle(unsigned int id, float * angle);
	void getLightSpotExponent(unsigned int id, float * exponent);

	// blending
	void enableBlending(void);
	void disableBlending(void);
	void setBlendingMode(M_BLENDING_MODES mode);
};

#endif
