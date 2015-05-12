/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiNode.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
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


#ifndef _M_GUI_NODE_H
#define _M_GUI_NODE_H


// Node Link
class MGuiNodeBranch;
class M_GUI_EXPORT MGuiNodeLink
{
private:

	MGuiNode * m_node;
	MGuiNodeBranch * m_branch;
	
public:

	MGuiNodeLink(MGuiNode * node, MGuiNodeBranch * branch):
		m_node(node),
		m_branch(branch)
	{}
	
	MGuiNode * getNode(void){ return m_node; }
	MGuiNodeBranch * getBranch(void){ return m_branch; }
};


// Node Branch
class M_GUI_EXPORT MGuiNodeBranch
{
private:

	unsigned int m_id;
	int m_type;
	MString m_name;
	vector <MGuiNodeLink> m_links;
	
public:
	
	MGuiNodeBranch(unsigned int id, int type, const char * name):
		m_id(id),
		m_type(type),
		m_name(name)
	{}
	
	inline unsigned int getId(void){ return m_id; }
	inline int getType(void){ return m_type; }
	inline const char * getName(void){ return m_name.getSafeString(); }
	inline unsigned int getLinksNumber(void){ return m_links.size(); }
	inline MGuiNodeLink * getLink(unsigned int id){ return &m_links[id]; }
	
	void unlink(void);
	void addLinkWith(MGuiNode * node, MGuiNodeBranch * branch);
	void removeLinkWith(MGuiNodeBranch * branch);
	void removeLinksWith(MGuiNode * node);
	bool isConnectedTo(MGuiNode * node);
};


// Node
class M_GUI_EXPORT MGuiNode : public MGui2d
{
private:

	// events function pointer
	void (* m_eventCallback)(MGuiNode * node, MGUI_EVENT_TYPE event);

	vector<MGuiNodeBranch> m_inputs;
	vector<MGuiNodeBranch> m_outputs;

	float m_pinSpace;
	MVector2 m_pinScale;

public:

	// constructor
	MGuiNode(void);

public:

	// inuts / outputs
	void addInput(const char * name);
	void addOutput(const char * name);
	inline unsigned int getInputsNumber(void){ return m_inputs.size(); }
	inline unsigned int getOutputsNumber(void){ return m_outputs.size(); }
	inline MGuiNodeBranch * getInput(unsigned int id){ return &m_inputs[id]; }
	inline MGuiNodeBranch * getOutput(unsigned int id){ return &m_outputs[id]; }
	MGuiNodeBranch * getMouseOverBranch(void);
	MVector2 getBranchPosition(MGuiNodeBranch * branch);
	bool isInputConnectedTo(MGuiNode * node);
	bool isOutputConnectedTo(MGuiNode * node);
	void removeLinksWith(MGuiNode * node);

	// change
	void onChange(void);

	// text
	inline void setText(const char * text){ m_textObject.setText(text); if(m_autoScaleFromText) autoScaleFromText(); }

	// pointer event
	inline void setEventCallback(void (* eventCallback)(MGuiNode * node, MGUI_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// virtual
	int getType(void){ return M_GUI_NODE; }
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void autoScaleFromText(void);
	void draw(void);
};

#endif
