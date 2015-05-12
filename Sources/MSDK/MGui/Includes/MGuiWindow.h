/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiWindow.h
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


#ifndef _M_GUI_WINDOW_H
#define _M_GUI_WINDOW_H


enum MGUI_DRAW_CALLBACK_MODES
{
	MGUI_DRAW_CALLBACK_PRE_GUI = 0,
	MGUI_DRAW_CALLBACK_POST_GUI
};


class M_GUI_EXPORT MGuiWindow : public MGui2d
{
private:

	// free view (the view can be scrolled and zoom freely)
	bool m_isFreeView;
	float m_zoom;
	float m_minZoom;
	float m_maxZoom;
	
	// events handling
	bool m_ignoreEvents;
	
	// scrolling
	bool m_isScrolled;
	MVector2 m_scroll;
	MVector2 m_maxScroll, m_minScroll;
	MVector2 m_margin; // margin
	MGuiSlide m_hScrollSlide; // horizontal slide
	MGuiSlide m_vScrollSlide; // vertical slide

	// nodes
	MGuiNode * m_currentNode;
	MGuiNodeBranch * m_currentBranch;

	// events function pointer
	void (* m_eventCallback)(MGuiWindow * window, MGUI_EVENT_TYPE event);

	// draw callback
	MGUI_DRAW_CALLBACK_MODES m_drawCallbackMode;
	void (* m_drawCallback)(MGuiWindow * window);

	// objects
	MWindow * m_rootWindow;
	vector <MGui2d *> m_objects;
	vector <MGuiButton *> m_buttons;
	vector <MGuiMenu *> m_menus;
	vector <MGuiText *> m_texts;
	vector <MGuiImage *> m_images;
	vector <MGuiEditText *> m_editTexts;
	vector <MGuiSlide *> m_slides;
	vector <MGuiNode *> m_nodes;

public:

	// constructor / destructor
	MGuiWindow(MWindow * rootWindow);
	~MGuiWindow(void);

private:

	// scrolling
	bool isHorizontalScroll(void);
	bool isVerticalScroll(void);
	void rescaleScrollingBar(void);

	// internal events
	void nodesEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void internalEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);

	// nodes
	bool getMouseOverNodeBranch(MGuiNode ** node, MGuiNodeBranch ** branch);
	void drawNodeInfo(void);
	void drawNodesLink(void);
	void checkDeletedNodes(void);

public:

	// clear
	void clear(void);

	// root window
	inline MWindow * getRootWindow(void){ return m_rootWindow; }

	// scroll bar
	bool isScrollBarPressed(void);
	void updateScrollingBar(void);
	void setScrollBarVisible(bool visible){ m_hScrollSlide.setVisible(visible); m_vScrollSlide.setVisible(visible); }
	
	// auto scale
	void autoScale(void);

	// margin
	inline void setMargin(const MVector2 & margin){ m_margin = margin; }
	inline MVector2 getMargin(void){ return m_margin; }

	// free view
	inline bool isFreeView(void){ return m_isFreeView; }
	inline void setFreeView(bool isFreeView){ m_isFreeView = isFreeView; }
	inline void setMinZoom(float minZoom){ m_minZoom = minZoom; }
	inline void setMaxZoom(float maxZoom){ m_maxZoom = maxZoom; }
	inline float getZoom(void){ return m_zoom; }
	
	// scrolling
	inline bool isScrolled(void){ return m_isScrolled; }
	inline void setScrolled(bool scroll){ m_isScrolled = scroll; }
	void resizeScroll(void);
	void moveScroll(const MVector2 & direction);
	void limitScroll(void);

	inline MVector2 getScroll(void){ return m_scroll; }
	inline void setScroll(const MVector2 & scroll){ m_scroll = scroll; }

	// menus
	void drawWindowMenus(void);
	int onWindowMenusEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);

	// get objects
	inline unsigned int getButtonsNumber(void){ return m_buttons.size(); }
	inline unsigned int getMenusNumber(void){ return m_menus.size(); }
	inline unsigned int getTextsNumber(void){ return m_texts.size(); }
	inline unsigned int getImagesNumber(void){ return m_images.size(); }
	inline unsigned int getEditTextsNumber(void){ return m_editTexts.size(); }
	inline unsigned int getSlidesNumber(void){ return m_slides.size(); }
	inline unsigned int getNodesNumber(void){ return m_nodes.size(); }

	inline MGuiButton * getButton(unsigned int id){ return m_buttons[id]; }
	inline MGuiMenu * getMenu(unsigned int id){ return m_menus[id]; }
	inline MGuiText * getText(unsigned int id){ return m_texts[id]; }
	inline MGuiImage * getImage(unsigned int id){ return m_images[id]; }
	inline MGuiEditText * getEditText(unsigned int id){ return m_editTexts[id]; }
	inline MGuiSlide * getSlide(unsigned int id){ return m_slides[id]; }
	inline MGuiNode * getNode(unsigned int id){ return m_nodes[id]; }

	// add/remove objects
	MGuiButton * addNewButton(void);
	MGuiMenu * addNewMenu(void);
	MGuiText * addNewText(void);
	MGuiImage * addNewImage(void);
	MGuiEditText * addNewEditText(void);
	MGuiSlide * addNewSlide(void);
	MGuiNode * addNewNode(void);

	// editing
	bool isSomethingEditing(void);

	// pointer event
	void setEventCallback(void (*eventCallback)(MGuiWindow * window, MGUI_EVENT_TYPE event)){ m_eventCallback = eventCallback; }

	// draw callback
	void setDrawCallbackMode(MGUI_DRAW_CALLBACK_MODES mode){ m_drawCallbackMode = mode; }
	MGUI_DRAW_CALLBACK_MODES getDrawCallbackMode(void){ return m_drawCallbackMode; }
	inline void setDrawCallback(void (* drawCallback)(MGuiWindow * window)){ m_drawCallback = drawCallback; }

	// events handling
	void ignoreEvents(bool state){ m_ignoreEvents = state; }
	bool isIgnoringEvents(void){ return m_ignoreEvents; }

	// update
	void update(void);

	// virtual
	int getType(void){ return M_GUI_WINDOW; }
	void onEvent(MWindow * rootWindow, MWIN_EVENT_TYPE event);
	void setHighLight(bool highLight);
	void draw(void);
};

#endif
