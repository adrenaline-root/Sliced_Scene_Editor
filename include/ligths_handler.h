#ifndef LIGTHS_HANDLER_H
#define LIGTHS_HANDLER_H

#include "headers.h"
#include "utilities.h"
#include "mesh.h"
#include "sliced_object.h"
#include "shaders_handler.h"

#include <cmath>
#include <glm/gtx/io.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

enum LigthType {DIRECTIONAL, POINT, FOCUS};

class Ligth {
public:
	std::string name;
	vec3D position = {0.0, 0.0, 0.0};
	vec3D direction = {1.0, 0.0, 0.0};
	vec3D ambient = {1., 1., 1.};
	vec3D specular = {1., 1., 1.};
	float attenuation = .04; 
	float frustrum = 30.;
	float strength = 2048.;
	
	LigthType ligth_type;
	
	Glib::RefPtr<Gdk::Pixbuf> oclusion_map;
	
	Ligth();
	virtual ~Ligth();
	
	// Ray Caster Height maps
	void render_oclusion_map(Glib::RefPtr<Gdk::Pixbuf> tile_map, Glib::RefPtr<Gdk::Pixbuf> heigth_map1, Glib::RefPtr<Gdk::Pixbuf> heigth_map2, vec2Di screen);
	void load_oclusion_map_to_texture(unsigned int texture_id);
	void push_ligth_into_shader(Shader *shader, int index);
};

class LigthsHandler {
public:
	std::vector<Ligth> ligths;
	ShaderHandler shader_handler;
	Glib::RefPtr<Gdk::Pixbuf> tile_map, heigth_map1, heigth_map2;
	int selected_ligth;
	
	LigthsHandler();
	virtual ~LigthsHandler();
	
	void prepare_shaders();
	
	void add_ligth(Ligth ligth);
	void remove_ligth(std::string name);
	Ligth get_ligth(std::string name);
	Ligth* get_selected_ligth();
	void load_pixbuf_to_texture(int texture_id, Glib::RefPtr<Gdk::Pixbuf> pixbuf);
	
	// Prepare a tilemap for RayCasting
	void render_tile_map();
	
	// Prepare height maps for RayCasting
	void render_heigth_maps(StandardMesh mesh, unsigned int texture);
	
	
};


#endif