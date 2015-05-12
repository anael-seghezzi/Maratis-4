/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MObject3d.h
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


#ifndef _M_OBJECT3D_H
#define _M_OBJECT3D_H


class M_ENGINE_EXPORT MObject3d
{
public:

	// constructor
	MObject3d(void);

	// destructor
	virtual ~MObject3d(void);

	// copy constructor
	MObject3d(const MObject3d & object);
	
protected:

	// name
	MString m_name;

	// transform
	MVector3 m_position;
	MVector3 m_scale;
	MQuaternion m_rotation;

	// matrices
	MMatrix4x4 m_matrix;

	// childs
	vector <MObject3d *> m_childs;

	// properties
	bool m_isActive;
	bool m_isVisible;
	bool m_needToUpdate;

	// linking
	MObject3d * m_parent;

	// behaviors
	vector <MBehavior *> m_behaviors;

private:

	void removeChild(MObject3d * child);

protected:
	
	void clearObject3d(void);
	
public:

	// matrices
	void updateMatrix(void);
	void computeLocalMatrix(void);
	inline MMatrix4x4 *	getMatrix(void){ return &m_matrix; }

	// childs
	void unlinkChilds(void);
	void computeChildsMatrices(void);

	// transform
	MVector3 getUniformRotatedVector(const MVector3 & vector);
	inline MVector3	getInverseRotatedVector(const MVector3 & vector) const	{ return m_matrix.getInverse().getRotatedVector3(vector); }
	inline MVector3	getRotatedVector(const MVector3 & vector) const			{ return m_matrix.getRotatedVector3(vector); }
	inline MVector3	getInversePosition(const MVector3 & position) const		{ return m_matrix.getInverse().getTransformedVector3(position); }
	inline MVector3	getTransformedVector(const MVector3 & vector) const		{ return m_matrix.getTransformedVector3(vector); }

	// position
	void setPosition(const MVector3 & position);
	inline MVector3	getTransformedPosition(void) const { return MVector3(m_matrix.entries[12], m_matrix.entries[13], m_matrix.entries[14]); }
	inline MVector3 getPosition(void) const { return m_position; }

	// rotation
	void setEulerRotation(const MVector3 & euler);
	void setAxisAngleRotation(const MVector3 & axis, float angle);
	void addAxisAngleRotation(const MVector3 & axis, float angle);
	void setRotation(const MQuaternion & rotation);
	inline MVector3	getTransformedRotation(void) const { return m_matrix.getEulerAngles(); }
	inline MVector3 getEulerRotation(void) const { return m_rotation.getEulerAngles(); }
	inline MQuaternion getRotation(void) const { return m_rotation; }

	// scale
	void setScale(const MVector3 & scale);
	inline MVector3 getTransformedScale(void) const { return m_matrix.getScale(); }
	inline MVector3 getScale(void) const { return m_scale; }
	
	// linking
	void linkTo(MObject3d * parent);
	void unLink(void);
	inline void setParent(MObject3d * object){ m_parent = object; }
	inline void addChild(MObject3d * child){ m_childs.push_back(child); }
	inline bool hasParent(void){ return (m_parent != NULL); }
	inline unsigned int getChildsNumber(void){ return m_childs.size(); }
	inline MObject3d * getParent(void){ return m_parent; }
	inline MObject3d * getChild(unsigned int id){ return m_childs[id]; }

	// behaviors
	void updateBehaviors(void);
	void drawBehaviors(void);
	void deleteBehavior(unsigned int id);
	void invertBehavior(unsigned int idA, unsigned int idB);
	void changeBehavior(unsigned int id, MBehavior * behavior);
	inline void addBehavior(MBehavior * behavior){ m_behaviors.push_back(behavior); }
	inline unsigned int getBehaviorsNumber(void){ return m_behaviors.size(); }
	inline MBehavior * getBehavior(unsigned int id){ return m_behaviors[id]; }

	// need to update
	inline bool needToUpdate(void){ return m_needToUpdate; }

	// active
	virtual void setActive(bool active){ m_isActive = active; }
	inline bool isActive(void){ return m_isActive; }

	// visibility
	inline void setVisible(bool visible){ m_isVisible = visible; }
	inline bool isVisible(void){ return m_isVisible; }
	virtual void updateVisibility(MOCamera * camera){}

	// name
	void setName(const char * name);
	inline const char * getName(void){ return m_name.getSafeString(); }

	// type
	virtual int getType(void){ return M_OBJECT3D; }

	// update
	virtual void update(void);
};

#endif