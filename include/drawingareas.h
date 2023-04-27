#ifndef DRAWING_AREAS_H
#define DRAWING_AREAS_H

#include "mesh.h"
#include "scene_handler.h"


enum DrawAreaType {TILE_SET_TYPE, TILE_TYPE};


class glDrawingArea : public Gtk::DrawingArea
{
public:
	Glib::RefPtr<Gtk::Builder> builder;

	Gtk::Window* parentWin;
	GdkWindow *parent;
	GdkDisplay *display;
	GdkWindowAttr attributes;
	vec2D mouse_pos;
	
	float* data = NULL;
	Glib::RefPtr<Gdk::Pixbuf> bufferdata;
	
	float rotation = 0.0;
	float scale = 1.0;
	float tranlation_x = 0.0;
	float tranlation_y = 0.0;
	int rejillaSize = 32;
	
	SceneHandler *scene_handler;
	
	Model boxSelector;
	
	Display *xdisplay;
	Window root;
	GLint att[5] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo *vi;
	Window win;
	GLXContext glc;
	XWindowAttributes gwa;
	
	
	float zoom = 1.0;
	float giro = 0.0;
	
	bool attached = false;
	
	glDrawingArea();
	glDrawingArea(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
	virtual ~glDrawingArea();

	void attach_xwindow();
	bool on_event(GdkEvent*);
	bool on_key_press_event(GdkEventKey *e);
	bool on_my_draw(const Cairo::RefPtr<Cairo::Context> &cr);
	void DrawAQuad();
	void Draw_ejes();
	void outlineSelectedModel(Model model);
	void readPixelsToPixbuff();
	void drawRejilla();
	void DrawPoints();
	void drawBoxSelector();
	void drawLigth(Ligth *ligth);

};

class mDrawingArea : public Gtk::DrawingArea
{
public:
	Glib::RefPtr<Gtk::Builder> builder;
	tileset *ref_tile_set = NULL;
	DrawAreaType draw_type;
	mDrawingArea *child_area;
	
	mDrawingArea();
	mDrawingArea(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
	virtual ~mDrawingArea();
	
	bool on_my_draw(const Cairo::RefPtr<Cairo::Context> &cr);
	bool on_event(GdkEvent*);
	void set_tileset_reference(tileset *ref);
	void unset_tileset_reference();
	

};




#endif 