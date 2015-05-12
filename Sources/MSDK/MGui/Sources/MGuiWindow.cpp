/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiWindow.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2013 Anael Seghezzi <www.maratis3d.com>
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


//constructor
MGuiWindow::MGuiWindow(MWindow * rootWindow):
MGui2d(),
m_isFreeView(false),
m_zoom(1),
m_minZoom(0.1f),
m_maxZoom(10),
m_ignoreEvents(false),
m_isScrolled(false),
m_scroll(0, 0),
m_maxScroll(0, 0),
m_minScroll(0, 0),
m_margin(16, 16),
m_currentNode(NULL),
m_currentBranch(NULL),
m_eventCallback(NULL),
m_drawCallbackMode(MGUI_DRAW_CALLBACK_PRE_GUI),
m_drawCallback(NULL),
m_rootWindow(rootWindow)
{
	m_hScrollSlide.setPosition(MVector2(0, m_scale.y-6));
	m_vScrollSlide.setPosition(MVector2(m_scale.x-6, 0));
	m_hScrollSlide.setButtonScale(MVector2(6, 6));
	m_vScrollSlide.setButtonScale(MVector2(6, 6));
	m_hScrollSlide.setNormalColor(MVector4(1, 1, 1, 0.3f));
	m_vScrollSlide.setNormalColor(MVector4(1, 1, 1, 0.3f));
	m_hScrollSlide.setHighLightColor(MVector4(1, 1, 1, 0.3f));
	m_vScrollSlide.setHighLightColor(MVector4(1, 1, 1, 0.3f));
	m_hScrollSlide.setPressedColor(MVector4(1, 1, 1, 0.3f));
	m_vScrollSlide.setPressedColor(MVector4(1, 1, 1, 0.3f));

	m_hScrollSlide.setParentWindow(this);
	m_vScrollSlide.setParentWindow(this);

	// horizontal slide
	m_hScrollSlide.setScrollAffect(false);
	m_hScrollSlide.setMinValue(0);
	m_hScrollSlide.setMaxValue(1);

	// vertical slide
	m_vScrollSlide.setScrollAffect(false);
	m_vScrollSlide.setMinValue(0);
	m_vScrollSlide.setMaxValue(1);

	resizeScroll();
}

MGuiWindow::~MGuiWindow(void)
{
	unsigned int i, oSize = m_objects.size();
	for(i=0; i<oSize; i++)
		SAFE_DELETE(m_objects[i]);

	m_objects.clear();
	m_menus.clear();
	m_buttons.clear();
	m_texts.clear();
	m_images.clear();
	m_editTexts.clear();
	m_slides.clear();
	m_nodes.clear();
}

void MGuiWindow::clear(void)
{
	unsigned int i, oSize = m_objects.size();
	for(i=0; i<oSize; i++)
		m_objects[i]->deleteMe();
}

bool MGuiWindow::isSomethingEditing(void)
{
	unsigned int i;
	unsigned int eSize = m_editTexts.size();
	for(i=0; i<eSize; i++)
	{
		if(m_editTexts[i]->isVisible() && !m_editTexts[i]->isDeleted() && m_editTexts[i]->isPressed())
			return true;
	}

	return false;
}

void MGuiWindow::autoScale(void)
{
	MVector2 tmp;
	MVector2 newScale(0, 0);

    unsigned int i;
    unsigned int size = m_objects.size();
    for(i=0; i<size; i++)
	{
		MGui2d * object = m_objects[i];
		if(!object->isVisible() || object->isDeleted())
			continue;
			
		tmp = object->getPosition()
		    + object->getScale();

		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}
	
	m_scale = newScale;

	resizeScroll();
}

MGuiButton * MGuiWindow::addNewButton(void)
{
	MGuiButton * button = new MGuiButton();
	button->setParentWindow(this);
	m_buttons.push_back(button);
	m_objects.push_back(button);
	return button;
}

MGuiMenu * MGuiWindow::addNewMenu(void)
{
	MGuiMenu * menu = new MGuiMenu(getRootWindow());
	menu->setParentWindow(this);
	m_menus.push_back(menu);
	m_objects.push_back(menu);
	return menu;
}

MGuiText * MGuiWindow::addNewText(void)
{
	MGuiText * text = new MGuiText();
	text->setParentWindow(this);
	m_texts.push_back(text);
	m_objects.push_back(text);
	return text;
}

MGuiImage * MGuiWindow::addNewImage(void)
{
	MGuiImage * image = new MGuiImage();
	image->setParentWindow(this);
	m_images.push_back(image);
	m_objects.push_back(image);
	return image;
}

MGuiEditText * MGuiWindow::addNewEditText(void)
{
	MGuiEditText * editText = new MGuiEditText();
	editText->setParentWindow(this);
	m_editTexts.push_back(editText);
	m_objects.push_back(editText);
	return editText;
}

MGuiSlide * MGuiWindow::addNewSlide(void)
{
	MGuiSlide * slide = new MGuiSlide();
	slide->setParentWindow(this);
	m_slides.push_back(slide);
	m_objects.push_back(slide);
	return slide;
}

MGuiNode * MGuiWindow::addNewNode(void)
{
	MGuiNode * node = new MGuiNode();
	node->setParentWindow(this);
	m_nodes.push_back(node);
	m_objects.push_back(node);
	return node;
}

void MGuiWindow::checkDeletedNodes(void)
{
	unsigned int n, nSize = m_nodes.size();
	
	// check if at least one node is delete
	bool nodeDeleted = false;
	for(n=0; n<nSize; n++)
	{
		MGuiNode * node = m_nodes[n];
		if(node->isDeleted())
			nodeDeleted = true;
	}
	
	if(! nodeDeleted)
		return;
	
	// unlink deleted nodes and build update list
	char * updateList = new char[nSize];
	memset(updateList, 0, sizeof(char)*nSize);
	
	for(n=0; n<nSize; n++)
	{
		MGuiNode * node = m_nodes[n];
		if(node->isDeleted())
		{
			// unlink all connected nodes
			unsigned int n2;
			for(n2=0; n2<nSize; n2++)
			{
				MGuiNode * node2 = m_nodes[n2];
				if(node2 != node && !node2->isDeleted())
				{
					if(node2->isInputConnectedTo(node))
						updateList[n2] = 1;
						
					node2->removeLinksWith(node);
				}
			}
		}
	}
	
	// update nodes
	for(n=0; n<nSize; n++)
	{
		if(updateList[n] == 1)
		{
			MGuiNode * node = m_nodes[n];
			node->onChange();
		}
	}
	
	delete [] updateList;
}

void MGuiWindow::update(void)
{
	checkDeletedNodes();
	
	// pop
	#define MGUI2D_POP(vector)\
	{\
		unsigned int i, size = vector.size();\
		for(i=0; i<size; i++)\
		{\
			unsigned int id = (size-1)-i;\
			if(vector[id]->isDeleted())\
				vector.erase(vector.begin() + id);\
		}\
	}
	
	MGUI2D_POP(m_menus);
	MGUI2D_POP(m_buttons);
	MGUI2D_POP(m_texts);
	MGUI2D_POP(m_images);
	MGUI2D_POP(m_editTexts);
	MGUI2D_POP(m_slides);
	MGUI2D_POP(m_nodes);
	
	// delete
	unsigned int i, size = m_objects.size();
	for(i=0; i<size; i++)
	{
		unsigned int id = (size-1)-i;
		if(m_objects[id]->isDeleted())
		{
			SAFE_DELETE(m_objects[id]);
			m_objects.erase(m_objects.begin() + id);
		}
	}
}

bool MGuiWindow::isScrollBarPressed(void)
{
	return (m_hScrollSlide.isPressed() || m_vScrollSlide.isPressed());
}

void MGuiWindow::nodesEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	switch(event)
	{
		default:
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		{
			if(isHighLight() && rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				m_currentNode = NULL;
				
				// check if at least one node is pressed
				bool isPressed = false;
				bool isBranchPressed = false;
				
				unsigned int n, nSize = getNodesNumber();
				for(n=0; n<nSize; n++)
				{
					MGuiNode * node = getNode(n);
					MGuiNodeBranch * branch = node->getMouseOverBranch();
					
					if(node->isMouseInside() && !branch)
						isPressed = true;
						
					if(branch)
					{
						if(branch->getType() == 0 && branch->getLinksNumber() > 0)
						{
							m_currentNode = branch->getLink(0)->getNode();
							m_currentBranch = branch->getLink(0)->getBranch();
							branch->unlink();
							node->onChange();
						}
						else
						{
							m_currentNode = node;
							m_currentBranch = branch;
						}
					}
				}
			
				if(!(isPressed && !isBranchPressed) && !rootWindow->isKeyPressed(MKEY_LCONTROL))
				{
					// unpress all
					for(n=0; n<nSize; n++)
					{
						MGuiNode * node = getNode(n);
						node->setPressed(false);
					}
				}
			}
			break;
		}
		
		case MWIN_EVENT_MOUSE_BUTTON_UP:
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			{
				// make link
				if(m_currentNode)
				{
					MGuiNode * node1 = m_currentNode;
					MGuiNodeBranch * branch1 = m_currentBranch;
					MGuiNode * node2 = NULL;
					MGuiNodeBranch * branch2 = NULL;
			
					if(getMouseOverNodeBranch(&node2, &branch2))
					{
						if(node1 != node2 && branch1->getType() != branch2->getType())
						{
							// check for loops
							bool loop = false;
							if(branch1->getType() == 1)
								loop = node1->isInputConnectedTo(node2);
							else
								loop = node2->isInputConnectedTo(node1);
							
							// add link
							if(! loop)
							{
								if(branch1->getType() == 0)
									branch1->unlink();
								else if(branch2->getType() == 0)
									branch2->unlink();
						
								branch1->addLinkWith(node2, branch2);
								branch2->addLinkWith(node1, branch1);
								
								if(branch1->getType() == 0)
									node1->onChange();
								else if(branch2->getType() == 0)
									node2->onChange();
							}
						}
					}
				}
				
				m_currentNode = NULL;
			}
			break;
		}
	}
}

void MGuiWindow::internalEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	unsigned int i, oSize = m_objects.size();
	
	// scolling slides
	if(isHorizontalScroll())
		m_hScrollSlide.onEvent(rootWindow, event);

	if(isVerticalScroll())
		m_vScrollSlide.onEvent(rootWindow, event);

	if(isScrollBarPressed()) // disable gui events if scroll bar pressed
	{
		if(event == MWIN_EVENT_MOUSE_BUTTON_DOWN)
		{
			for(i=0; i<oSize; i++)
				m_objects[i]->setHighLight(false);
		}
		return;
	}

	// nodes event
	nodesEvent(rootWindow, event);

	// objects event
	for(i=0; i<oSize; i++)
	{
		if(m_objects[i]->isVisible() && !m_objects[i]->isDeleted())
			m_objects[i]->onEvent(rootWindow, event);
	}
}

int MGuiWindow::onWindowMenusEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	unsigned int i;
	unsigned int mSize = m_menus.size();

	for(i=0; i<mSize; i++)
	{
		if(m_menus[i]->isVisible() && m_menus[i]->isPressed())
		{
			m_menus[i]->onWindowMenuEvent(rootWindow, event);
			m_menus[i]->onEvent(rootWindow, event);
			return 1;
		}
	}

	return 0;
}

void MGuiWindow::onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	switch(event)
	{
	case MWIN_EVENT_RESIZE:
		if(m_eventCallback)
			m_eventCallback(this, MGUI_EVENT_RESIZE);
		resizeScroll();
		break;
		
	case MWIN_EVENT_KEY_DOWN:
		switch(rootWindow->getKey())
		{
			case MKEY_TAB:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select next edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							unsigned int nextId = i + 1;

							if(nextId >= eSize)
								nextId = 0;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(0);
							m_editTexts[nextId]->setSelection(0, strlen(m_editTexts[nextId]->getText()));

							return;
						}
					}

					break;
				}
				
			case MKEY_DOWN:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select next edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							unsigned int nextId = i + 1;

							if(nextId >= eSize)
								nextId = 0;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(m_editTexts[i]->getCharId());

							return;
						}
					}

					break;
				}
				
			case MKEY_UP:
				{
					// editTexts
					unsigned int eSize = m_editTexts.size();
					unsigned int i;

					// select previous edit text
					for(i=0; i<eSize; i++)
					{
						if(m_editTexts[i]->isSingleLine() && m_editTexts[i]->isPressed())
						{
							m_editTexts[i]->setPressed(false);
							m_editTexts[i]->sendVariable();
							int nextId = i - 1;

							if(nextId < 0)
								nextId = eSize - 1;

							m_editTexts[nextId]->setPressed(true);
							m_editTexts[nextId]->setCharId(m_editTexts[i]->getCharId());

							return;
						}
					}

					break;
				}
			default:
				break;
		}
		break;
		
	case MWIN_EVENT_MOUSE_SCROLL:
		if(isHighLight())
		{
			if(m_isFreeView)
			{
				float prevZoom = m_zoom;
				float factor = rootWindow->getMouseScroll().y*0.1f;
				
				if(factor > 0)
					m_zoom = CLAMP(m_zoom*(1+factor), m_minZoom, m_maxZoom);
				else
					m_zoom = CLAMP(m_zoom/(1-factor), m_minZoom, m_maxZoom);
	
				float size = m_zoom/prevZoom;
				MVector2 center = m_scale*0.5f;
				m_scroll = center + (m_scroll - center)*size;
			}
			else
			{
				moveScroll(rootWindow->getMouseScroll() * getTextSize());
			}
		}
		break;
		
	case MWIN_EVENT_MOUSE_MOVE:
		if(isScrolled())
			moveScroll(rootWindow->getMouseDir());

		if(isMouseInside())
		{
			setHighLight(true);
			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_MOVE);
		}
		else
		    setHighLight(false);
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:

		if(isHighLight())
		{
			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
				setPressed(true);

			if(rootWindow->getMouseButton() == MMOUSE_BUTTON_MIDDLE)
			{
				resizeScroll();
				setScrolled(true);
			}
		}
			
		if(m_eventCallback)
			m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_DOWN);
		break;
		
	case MWIN_EVENT_MOUSE_BUTTON_UP:

		if(rootWindow->getMouseButton() == MMOUSE_BUTTON_MIDDLE)
			setScrolled(false);

		if(rootWindow->getMouseButton() == MMOUSE_BUTTON_LEFT)
			setPressed(false);

		if(isHighLight())
		{
			if(m_eventCallback)
				m_eventCallback(this, MGUI_EVENT_MOUSE_BUTTON_UP);
		}
		break;
		
	default:
		break;
	}

	internalEvent(rootWindow, event);
}

bool MGuiWindow::isHorizontalScroll(void)
{
	return ((m_maxScroll.x - m_minScroll.x) > m_scale.x);
}

bool MGuiWindow::isVerticalScroll(void)
{
	return ((m_maxScroll.y - m_minScroll.y) > m_scale.y);
}

void MGuiWindow::rescaleScrollingBar(void)
{
	MVector2 scale = getScale();

	m_hScrollSlide.setPosition(MVector2(0, scale.y - m_hScrollSlide.getButtonScale().y));
	m_vScrollSlide.setPosition(MVector2(scale.x - m_vScrollSlide.getButtonScale().x, 0));

	float length = (scale.x / (m_maxScroll.x - m_minScroll.x)) * scale.x;
	length = MAX(32, length);

	if(length < scale.x)
	{
		m_hScrollSlide.setButtonScale(MVector2(length, m_hScrollSlide.getButtonScale().y));
		m_hScrollSlide.setDirection(MVector2(scale.x - length, 0));

		if(isScrollBarPressed())
		{
			float value = m_hScrollSlide.getValue();
			m_scroll.x = -(m_minScroll.x + (m_maxScroll.x - m_minScroll.x - scale.x)*value);
		}
	}

	float height = (scale.y / (m_maxScroll.y - m_minScroll.y)) * scale.y;
	height = MAX(32, height);

	if(height < scale.y)
	{
		m_vScrollSlide.setButtonScale(MVector2(m_vScrollSlide.getButtonScale().x, height));
		m_vScrollSlide.setDirection(MVector2(0, scale.y - height));

		if(isScrollBarPressed())
		{
			float value = m_vScrollSlide.getValue();
			m_scroll.y = -(m_minScroll.y + (m_maxScroll.y - m_minScroll.y - scale.y)*value);
		}
	}
}

void MGuiWindow::limitScroll(void)
{
	if(m_isFreeView)
		return;

	if(isHorizontalScroll())
	{
		if(-m_scroll.x < m_minScroll.x)
			m_scroll.x = -m_minScroll.x;
			
		if(-m_scroll.x > (m_maxScroll.x-m_scale.x))
			m_scroll.x = -(m_maxScroll.x-m_scale.x);
	}
	else
	{
		m_scroll.x = -m_minScroll.x;
	}
	
	if(isVerticalScroll())
	{
		if(-m_scroll.y < m_minScroll.y)
			m_scroll.y = -m_minScroll.y;

		if(-m_scroll.y > (m_maxScroll.y-m_scale.y))
			m_scroll.y = -(m_maxScroll.y-m_scale.y);
	}
	else
	{
		m_scroll.y = -m_minScroll.y;
	}
	
	updateScrollingBar();
}

void MGuiWindow::updateScrollingBar(void)
{
	// set scrolling bar value
	float value;

	// horizontal
	value = (-m_scroll.x - m_minScroll.x) / (m_maxScroll.x - m_minScroll.x - getScale().x);
	m_hScrollSlide.setValue(value);

	// vertical
	value = (-m_scroll.y - m_minScroll.y) / (m_maxScroll.y - m_minScroll.y - getScale().y);
	m_vScrollSlide.setValue(value);
}

void MGuiWindow::moveScroll(const MVector2 & direction)
{
	m_scroll.x += direction.x;
	m_scroll.y += direction.y;
	limitScroll();
}

void MGuiWindow::resizeScroll(void)
{
	unsigned int i;

	MVector2 min;
	MVector2 max;

	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
	{
		if(!m_objects[i]->isVisible() || m_objects[i]->isDeleted())
			continue;
			
		MVector2 p1 = m_objects[i]->getPosition();
		MVector2 p2 = p1 + m_objects[i]->getScale();

		min.x = MIN(min.x, p1.x);
		min.y = MIN(min.y, p1.y);

		max.x = MAX(max.x, p2.x);
		max.y = MAX(max.y, p2.y);
	}

	m_minScroll = min;
	m_maxScroll = max;

	if(isHorizontalScroll())
	{
		if(m_minScroll.x < 0)
			m_minScroll.x -= m_margin.x;
		else
			m_minScroll.x = 0;
		m_maxScroll.x += m_margin.x;
	}

	if(isVerticalScroll())
	{
		if(m_minScroll.y < 0)
			m_minScroll.y -= m_margin.y;
		else
			m_minScroll.y = 0;
		m_maxScroll.y += m_margin.y;
	}

	rescaleScrollingBar();

	m_hScrollSlide.onChange();
	m_vScrollSlide.onChange();

	limitScroll();
}

void MGuiWindow::setHighLight(bool highLight)
{
	MGui2d::setHighLight(highLight);

	if(! highLight)
	{
		unsigned int i;
		unsigned int oSize = m_objects.size();
		for(i=0; i<oSize; i++)
			m_objects[i]->setHighLight(false);
	}
}

void MGuiWindow::drawWindowMenus(void)
{
	unsigned int i;
	unsigned int mSize = m_menus.size();
	for(i=0; i<mSize; i++)
	{
		if(m_menus[i]->isVisible() && m_menus[i]->isPressed())
		{
			m_menus[i]->drawWindowMenu();
			return;
		}
	}
}

bool MGuiWindow::getMouseOverNodeBranch(MGuiNode ** node, MGuiNodeBranch ** branch)
{
	*node = NULL;
	*branch = NULL;
	
	unsigned int n, nSize = getNodesNumber();
	for(n=0; n<nSize; n++)
	{
		MGuiNode * curNode = getNode(n);
		if(! curNode->isVisible())
			continue;
		
		MGuiNodeBranch * curBranch = curNode->getMouseOverBranch();
		if(curBranch)
		{
			*branch = curBranch;
			*node = curNode;
		}
	}
	
	if(*node)
		return true;

	return false;
}

void MGuiWindow::drawNodeInfo(void)
{
	if(! isHighLight())
		return;

	MGuiNode * node = NULL;
	MGuiNodeBranch * branch = NULL;

	if(getMouseOverNodeBranch(&node, &branch))
	{
		MGuiText text;
		text.setTextSize(node->getTextSize());
		text.setFont(node->getFont());
		text.setText(branch->getName());
		text.setColor(node->getTextColor());
		text.setPosition(getPointLocalPosition(m_rootWindow->getMousePosition()));
		text.setPosition(text.getPosition() + MVector2(-text.getScale().x*0.5f, 0));
			
		if(branch->getType() == 0)
			text.setPosition(text.getPosition() - MVector2(0, 24));
		else
			text.setPosition(text.getPosition() + MVector2(0, 16));
			
		text.draw();
	}
}

void MGuiWindow::drawNodesLink(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MVector2 vertices[2];
	MVector4 colors[2];
				
	colors[0] = MVector4(0, 0, 0, 0.35f);
	colors[1] = MVector4(0, 0, 0, 0.18f);
	
	render->disableTexture();
	render->disableCullFace();
	render->disableNormalArray();
	render->disableTexCoordArray();
	render->enableColorArray();
	render->enableVertexArray();
	render->enableLineAntialiasing();
				
	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->setColorPointer(M_FLOAT, 4, colors);
	
	MVector2 offset = MVector2(2, 2);
	
	// all links
	{
		unsigned int n, nSize = getNodesNumber();
		for(n=0; n<nSize; n++)
		{
			MGuiNode * node = getNode(n);
			if(! node->isVisible())
				continue;
			
			unsigned int o, oSize = node->getOutputsNumber();
			for(o=0; o<oSize; o++)
			{
				MGuiNodeBranch * branch = node->getOutput(o);
				unsigned int l, lSize = branch->getLinksNumber();
				for(l=0; l<lSize; l++)
				{
					MGuiNodeLink * link = branch->getLink(l);
					MGuiNode * node2 = link->getNode();
					MGuiNodeBranch * branch2 = link->getBranch();
					
					vertices[0] = node->getBranchPosition(branch) + offset;
					vertices[1] = node2->getBranchPosition(branch2) + offset;
					
					render->drawArray(M_PRIMITIVE_LINES, 0, 2);
				}
			}
		}
	}
	
	// current
	if(m_currentNode)
	{
		if(m_currentNode->isVisible())
		{
			render->disableColorArray();

			vertices[0] = m_currentNode->getBranchPosition(m_currentBranch) + offset;
			vertices[1] = m_currentNode->getPointLocalPosition(m_rootWindow->getMousePosition());
	
			render->setColor4(MVector4(0, 0, 0, 0.35f));
			render->drawArray(M_PRIMITIVE_LINES, 0, 2);
		}
	}
	
	render->disableLineAntialiasing();
}

void MGuiWindow::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	if(! isVisible())
		return;
	
	
	// get viewport and matrices
	int viewport[4];
	render->getViewport(viewport);
	
	MMatrix4x4 projMatrix;
	render->getProjectionMatrix(&projMatrix);
	
	MMatrix4x4 viewMatrix;
	render->getModelViewMatrix(&viewMatrix);
	

	// scissor
	rescaleScrollingBar();
	render->enableScissorTest();
	render->setScissor((int)getPosition().x, viewport[3] - (int)getPosition().y - (unsigned int)getScale().y, (unsigned int)getScale().x, (unsigned int)getScale().y);

	
	// modes
	render->disableDepthTest();
	render->disableCullFace();
	render->disableLighting();
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);


	// clear
	if(hasNormalTexture())
	{
		render->setColor4(getNormalColor());
		drawTexturedQuad(getNormalTexture()->getTextureId());
	}
	else
	{
		if(getNormalColor().w < 1.0f)
		{
			render->setColor4(getNormalColor());
			drawQuad();
		}
		else
		{
			render->setClearColor(getNormalColor());
			render->clear(M_BUFFER_COLOR);
		}
	}


	// draw callback pre-gui
	if(m_drawCallback && m_drawCallbackMode == MGUI_DRAW_CALLBACK_PRE_GUI)
	{
		m_drawCallback(this);

		// restore viewport
		render->setViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		// restore matrices
		render->setMatrixMode(M_MATRIX_PROJECTION);
		render->loadIdentity();
		render->multMatrix(&projMatrix);
		render->setMatrixMode(M_MATRIX_MODELVIEW);
		render->loadIdentity();
		render->multMatrix(&viewMatrix);
	
		// restore modes
		render->disableDepthTest();
		render->disableCullFace();
		render->disableLighting();
		render->enableBlending();
		render->setBlendingMode(M_BLENDING_ALPHA);
	}


	// gui
	render->pushMatrix();
	render->translate(MVector3((int)getPosition().x, (int)getPosition().y, 0));

	render->pushMatrix();
	render->translate(MVector3((int)getScroll().x, (int)getScroll().y, 0));

	if(m_isFreeView)
	{
		render->scale(MVector3(m_zoom, m_zoom, 1));
	}

	drawNodesLink();

	// drawing
	unsigned int i;
	unsigned int oSize = m_objects.size();
	for(i=0; i<oSize; i++)
	{
		if(m_objects[i]->isVisible() && !m_objects[i]->isDeleted())
			m_objects[i]->draw();
	}

	render->popMatrix();

	// draw shadows
	if(hasShadow())
	{
		render->disableScissorTest();
		drawShadow();
		render->enableScissorTest();
	}

	// scolling slides
	if(! m_isFreeView)
	{
		if(isHorizontalScroll())
			m_hScrollSlide.draw();

		if(isVerticalScroll())
			m_vScrollSlide.draw();
	}
		
	render->popMatrix();
	
	drawNodeInfo();
	
	// draw callback post-gui
	if(m_drawCallback && m_drawCallbackMode == MGUI_DRAW_CALLBACK_POST_GUI)
		m_drawCallback(this);
}
