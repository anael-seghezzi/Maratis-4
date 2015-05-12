/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiNode.cpp
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


#include <MEngine.h>
#include "../Includes/MGui.h"


// Node Branch
void MGuiNodeBranch::addLinkWith(MGuiNode * node, MGuiNodeBranch * branch)
{
	m_links.push_back(MGuiNodeLink(node, branch));
}

void MGuiNodeBranch::unlink(void)
{
	int l, lSize = m_links.size();
	for(l=0; l<lSize; l++)
		m_links[l].getBranch()->removeLinkWith(this);
	
	m_links.clear();
}

void MGuiNodeBranch::removeLinkWith(MGuiNodeBranch * branch)
{
	unsigned int l, lSize = m_links.size();
	for(l=0; l<lSize; l++)
	{
		if(m_links[l].getBranch() == branch)
		{
			m_links.erase(m_links.begin() + l);
			break;
		}
	}
}

void MGuiNodeBranch::removeLinksWith(MGuiNode * node)
{
	unsigned int l, lSize = m_links.size();
	if(lSize > 0)
	{
		for(l=0; l<lSize; l++)
		{
			unsigned int id = lSize-1-l; // scan reverse
			if(m_links[id].getNode() == node)
				m_links.erase(m_links.begin() + id);
		}
	}
}

bool MGuiNodeBranch::isConnectedTo(MGuiNode * node)
{
	unsigned int l, lSize = m_links.size();
	if(lSize > 0)
	{
		for(l=0; l<lSize; l++)
		{
			if(m_links[l].getNode() == node)
				return true;
		}
	}
	
	return false;
}


// Node
MGuiNode::MGuiNode(void):
m_eventCallback(NULL),
m_pinSpace(8),
m_pinScale(4, 4)
{
	setText("");
	setColor(MVector4(0.35f, 0.35f, 0.35f, 1));
	setPressedColor(MVector4(0.65f, 0.65f, 0.65f, 1));
	setTextColor(MVector4(1, 1, 1, 0.7f));
	setShadow(true);
	setAutoScaleFromText(true);
	setTextAlign(M_ALIGN_CENTER);
}

bool MGuiNode::isInputConnectedTo(MGuiNode * node)
{
	unsigned int i, iSize = getInputsNumber();
	for(i=0; i<iSize; i++)
	{
		MGuiNodeBranch * input = getInput(i);
		if(input->isConnectedTo(node))
			return true;
			
		unsigned int l, lSize = input->getLinksNumber();
		for(l=0; l<lSize; l++)
		{
			if(input->getLink(l)->getNode()->isInputConnectedTo(node))
				return true;
		}
	}
	
	return false;
}

bool MGuiNode::isOutputConnectedTo(MGuiNode * node)
{
	unsigned int o, oSize = getOutputsNumber();
	for(o=0; o<oSize; o++)
	{
		MGuiNodeBranch * output = getOutput(o);
		if(output->isConnectedTo(node))
			return true;
			
		unsigned int l, lSize = output->getLinksNumber();
		for(l=0; l<lSize; l++)
		{
			if(output->getLink(l)->getNode()->isOutputConnectedTo(node))
				return true;
		}
	}
	
	return false;
}

void MGuiNode::removeLinksWith(MGuiNode * node)
{
	unsigned int i, iSize = getInputsNumber();
	for(i=0; i<iSize; i++)
		getInput(i)->removeLinksWith(node);
		
	unsigned int o, oSize = getOutputsNumber();
	for(o=0; o<oSize; o++)
		getOutput(o)->removeLinksWith(node);
}

void MGuiNode::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MGuiWindow * parent = getParentWindow();

	// events
	switch(event)
	{
	case MWIN_EVENT_MOUSE_MOVE:
		if(parent->isPressed() && isPressed() && rootWindow->isMouseButtonPressed(MMOUSE_BUTTON_LEFT))
		{
			m_position += rootWindow->getMouseDir()/parent->getZoom();
		}
		
		if(parent->isHighLight() && isMouseInside())
		{
			setHighLight(true);
			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
		}
		else
		{
		    setHighLight(false);
		}
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
	{
		bool isTouched = (parent->isHighLight() && isMouseInside() && !getMouseOverBranch() && rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT);
	
		if(isTouched)
		{
			// un-press other nodes
			if(!isPressed() && !rootWindow->isKeyPressed(MKEY_LCONTROL))
			{
				unsigned int n, nSize = parent->getNodesNumber();
				for(n=0; n<nSize; n++)
					parent->getNode(n)->setPressed(false);
			}
			
			if(! rootWindow->isKeyPressed(MKEY_LCONTROL))
				setPressed(true);
			else
				setPressed(!isPressed());
					
			if(! isHighLight())
				setHighLight(true);
		}
		
		if(parent->isHighLight() && isMouseInside())
		{
			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
		}
		break;
	}
	
	case MWIN_EVENT_MOUSE_BUTTON_UP:
		if(parent->isHighLight() && isMouseInside())
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
			   if(! isHighLight())
                    setHighLight(true);
			}

			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_UP);
		}
		break;
            
	default:
		break;
	}
}

void MGuiNode::addInput(const char * name)
{
	m_inputs.push_back(MGuiNodeBranch(m_inputs.size(), 0, name));
}

void MGuiNode::addOutput(const char * name)
{
	m_outputs.push_back(MGuiNodeBranch(m_outputs.size(), 1, name));
}

MVector2 MGuiNode::getBranchPosition(MGuiNodeBranch * branch)
{
	unsigned int nb = 0;
	MVector2 pos;

	// inputs
	if(branch->getType() == 0)
	{
		nb = getInputsNumber();
		pos.y = m_position.y;
	}
	// outputs
	else
	{
		nb = getOutputsNumber();
		pos.y = m_position.y + (m_scale.y - m_pinScale.y);
	}
	
	if(nb > 0)
	{
		float LX = m_pinScale.x*nb + m_pinSpace*(nb-1);
		pos.x = m_position.x + (m_scale.x - LX)*0.5f + (m_pinScale.x + m_pinSpace)*branch->getId();
	}
	
	return pos;
}

MGuiNodeBranch * MGuiNode::getMouseOverBranch(void)
{
	MWindow * rootWindow = m_parentWindow->getRootWindow();
	
	unsigned int iSize = getInputsNumber();
	unsigned int oSize = getOutputsNumber();
	
	float R2 = m_pinSpace*m_pinSpace;
		
	MVector2 mousePos = getPointLocalPosition(rootWindow->getMousePosition());
	
	// inputs
	if(iSize > 0)
	{
		float LX = m_pinScale.x*iSize + m_pinSpace*(iSize-1);
		MVector2 pos = m_position + MVector2((m_scale.x - LX)*0.5f, 0);
		
		for(unsigned int i=0; i<iSize; i++)
		{
			if((mousePos - (pos + m_pinScale*0.5f)).getSquaredLength() < R2)
				return &m_inputs[i];
			pos.x += m_pinScale.x + m_pinSpace;
		}
	}
	
	// inputs
	if(oSize > 0)
	{
		float LX = m_pinScale.x*oSize + m_pinSpace*(oSize-1);
		MVector2 pos = m_position + MVector2((m_scale.x - LX)*0.5f, m_scale.y-m_pinScale.y);
		
		for(unsigned int o=0; o<oSize; o++)
		{
			if((mousePos - (pos + m_pinScale*0.5f)).getSquaredLength() < R2)
				return &m_outputs[o];
			pos.x += m_pinScale.x + m_pinSpace;
		}
	}

	return NULL;
}

void MGuiNode::autoScaleFromText(void)
{
	MGui2d::autoScaleFromText();
	m_scale.x = m_scale.x+32;
	m_scale.y += 16;
}

void MGuiNode::onChange(void)
{
	if(m_eventCallback)
		m_eventCallback(this, MGUI_EVENT_ON_CHANGE);
}

void MGuiNode::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	if(! isVisible())
		return;

	MGui2d::draw();
	
	unsigned int iSize = getInputsNumber();
	unsigned int oSize = getOutputsNumber();
	
	if(iSize > 0 || oSize > 0)
	{
		MVector4 color(0, 0, 0, 0.5f);
	
		MGuiImage plot;
		plot.setScale(m_pinScale);
	
		// inputs
		if(iSize > 0)
		{
			float LX = m_pinScale.x*iSize + m_pinSpace*(iSize-1);
			MVector2 pos = m_position + MVector2((m_scale.x - LX)*0.5f, 0);
		
			render->setColor4(color);
			for(unsigned int i=0; i<iSize; i++)
			{
				plot.setPosition(pos);
				plot.drawQuad();
				pos.x += m_pinScale.x + m_pinSpace;
			}
		}
		
		// outputs
		if(oSize > 0)
		{
			float LX = m_pinScale.x*oSize + m_pinSpace*(oSize-1);
			MVector2 pos = m_position + MVector2((m_scale.x - LX)*0.5f, m_scale.y-m_pinScale.y);
		
			render->setColor4(color);
			for(unsigned int o=0; o<oSize; o++)
			{
				plot.setPosition(pos);
				plot.drawQuad();
				pos.x += m_pinScale.x + m_pinSpace;
			}
		}
	}
}
