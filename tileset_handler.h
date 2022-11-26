#ifndef TILESET_HANDLER_H
#define TILESET_HANDLER_H

#include "headers.h"
#include "sliced_object.h"

class tileset {

public:
	std::string path;
	std::string name;
	vec3Di dimensions;
	vec2Di elements_dimensions;
	int number_of_elements;
	std::vector<Sliced_Object> sliced_objects;
	
	Glib::RefPtr<Gdk::Pixbuf> color_texture;
	Glib::RefPtr<Gdk::Pixbuf> normal_texture;
	
	int selected_element = 0;
	
	tileset();
	virtual ~tileset();
	
	Glib::RefPtr<Gdk::Pixbuf> get_element_image(int element_index);
	Sliced_Object get_selected_object();
	Sliced_Object get_object_by_name(std::string name);
	
};

class tlsHandler {
public:
	std::vector<tileset> tile_set_list;
	int selected_tile = 0;

	tlsHandler();
	virtual ~tlsHandler();
	bool load_tile_from_file(Glib::ustring file);
	void select_tile(int index);
	tileset get_selected_tile();
	tileset* get_selected_tile_pntr();
};


#endif