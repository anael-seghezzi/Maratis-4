/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MSelectionManager.h
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


#ifndef _M_SELECTION_MANAGER_H
#define _M_SELECTION_MANAGER_H


class M_EDITOR_EXPORT MSelectionManager
{
private:
	
	MVector3 m_selectionCenter;
	vector <MObject3d *> m_selection;
	
public:
	
	void selectAll(void);
	void selectSameMesh(void);
	void select(MObject3d * object);
	void deselect(MObject3d * object);
	void clearSelection(void);
	void activateSelection(void);
	void deleteSelection(void);
	void duplicateSelection(void);
	void updateSelectionCenter(void);
	
	bool isObjectSelected(MObject3d * object);
	unsigned int getSelectionSize(void);
	MObject3d * getSelectedObject(unsigned int id);
	inline MVector3 const getSelectionCenter(void){ return m_selectionCenter; }
};

#endif