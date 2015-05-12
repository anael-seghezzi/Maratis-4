/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MV3dView.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#ifndef _MV_3D_VIEW
#define _MV_3D_VIEW


class M_EDITOR_EXPORT MV3dView : public MViewport
{
public:
    
	MV3dView(void);
	virtual ~MV3dView(void);

protected:

	MUserView m_view;
	MGuiWindow * m_window;
	float m_unitSize;

	static void drawCallback(MGuiWindow * window);
	
	void drawGrid(void);
	void drawBoundingBox(MBox3d * box);
	void drawArmature(MOEntity * entity);
	void drawWireframe(MMesh * mesh);
	void drawBillboardObject(MObject3d * object, MMeshRef * meshRef, float unitSize);
	void drawLight(MOLight * light, MMeshRef * meshRef, float unitSize);
	void drawCamera(MOCamera * camera, MMeshRef * meshRef, float unitSize);
	void drawTriangles(MMesh * mesh);
	float getBillboardObjectSize(MObject3d * object, float unitSize);
	
public:
	
	virtual void create(MWindow * rootWindow);
	virtual void resize(MVector2 position, MVector2 scale);
	virtual void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	virtual void hide(void);
	virtual void show(void);
};

#endif