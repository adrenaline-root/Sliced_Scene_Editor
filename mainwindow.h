#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "drawingareas.h"
#include "scene_handler.h"

class MainWindow : public Gtk::Window
{
public:
	Glib::RefPtr<Gtk::Builder> builder;
	
	glDrawingArea *glAreaN = nullptr;
	
	mDrawingArea *tiles_draw_area = nullptr;
	mDrawingArea *selected_tile_draw_area = nullptr;
	
	SceneHandler scene_handler;
	
	Glib::RefPtr<Gtk::ActionGroup> mRefActionGroup;
	Glib::RefPtr<Gtk::UIManager> mRefUIManager;
	
	MainWindow();
	MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
        
	virtual ~MainWindow();
	
	void setMenu();
	void configureButtons();
	
	void loadScene();
	void saveScene();
	void showFileChooser(Gtk::Entry *entry);
	
	void loadTileSet();
	void deleteTileSet();
	void selectTileSet();
	
	
	void add_object_to_scene();
	void delete_object_from_scene();
	void select_tile();
	void rotar_objeto();
	void mover_objeto();
	void set_rejilla();
	
};

#endif