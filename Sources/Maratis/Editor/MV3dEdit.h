/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MV3dEdit.h
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


#ifndef _MV_3D_EDIT
#define _MV_3D_EDIT


enum M_AXIS
{
	M_AXIS_NONE = 0,
	M_AXIS_X,
	M_AXIS_Y,
	M_AXIS_Z,
	M_AXIS_VIEW
};

enum M_ORIENTATION_MODE
{
	M_ORIENTATION_WORLD = 0,
	M_ORIENTATION_LOCAL
};

enum M_TRANSFORM_MODE
{
	M_TRANSFORM_MOUSE = 0,
	M_TRANSFORM_POSITION,
	M_TRANSFORM_ROTATION,
	M_TRANSFORM_SCALE
};


class M_EDITOR_EXPORT MV3dEdit : public MV3dView
{
public:
    
	MV3dEdit(void);
	~MV3dEdit(void);

private:

	MVector2 m_tMousePosition;
	MVector2 m_tCenterPosition;
	MVector2 m_tVectorDirection;
	MVector2 m_tOffsetDirection;
	MVector3 m_tPlane;
	
	void computeTransformDirection(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float distance, const MVector3 & axis);
	void computeTransformPlane(MOCamera * camera, const MVector3 & position, const MVector3 & axis);
	void computeTransformSizeAndPos(MOCamera * camera, MVector3 * position, float * size);
	
protected:

	int m_selectionDepth;
	M_AXIS m_currentAxis;
	MGuiWindow * m_tools;
	bool m_axisVis[3];
	
	static void drawCallback(MGuiWindow * window);
	
	void pointSelect(MVector2 point, bool useDepth = true);
	void drawEdit(M_TRANSFORM_MODE mode, MOCamera * camera, bool local=false);
	void drawAxis(M_TRANSFORM_MODE mode, M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, bool viewTest = true);
	float getObjectRayNearestIntersectionDistance(MObject3d * object, const MVector3 & origin, const MVector3 & direction);
	M_AXIS getSelectedAxis(M_TRANSFORM_MODE mode, MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float radius, bool local=false);

public:
	
	M_TRANSFORM_MODE getTransformMode(void);
	
	void create(MWindow * rootWindow);
	void resize(MVector2 position, MVector2 scale);
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void hide(void);
	void show(void);
};

#endif