#ifndef SCENE_HANDLER_H
#define SCENE_HANDLER_H

#include "headers.h"
#include "sliced_object.h"
#include "tileset_handler.h"
#include "shaders_handler.h"


class TextureHandler {
public:
	std::vector<std::string> textures_names;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> color_textures;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> normal_textures;
	std::vector<GLuint> generated_textures;
	Glib::RefPtr<Gdk::Pixbuf> final_c_texture;
	Glib::RefPtr<Gdk::Pixbuf> final_n_texture;
	
	TextureHandler();
	virtual ~TextureHandler();
	void pack_textures();
	void gen_textures();
	void bind_textures();
	void add_color_and_normals_texture(tlsHandler tileset_handler);
	void add_texture_name(std::string texture_name);
};


class SceneHandler {
public:
	tlsHandler tileset_handler;
	std::vector<Sliced_Object> sliced_objects;
	TextureHandler texture_handler;
	ShaderHandler shader_handler, shadow_shader_handler;
	
	LayeredMesh layered_mesh;
	ActionType action = NO_ACTION;
	
	//Model mesh;
	
	SceneHandler();
	virtual ~SceneHandler();
	
	void init_shaders();
	
	bool add_tile(Glib::ustring filename);
	void remove_tile();
	
	void add_sliced_object_to_scene(vec3Di box_selector_pos, int rotation = 0);
	void delete_sliced_object_from_scene(vec3Di box_selector_pos);
	void add_slices_to_layered_mesh();
	
	void pack_textures();
	
	void bind_textures();
	void make_context_current(Display *xdisplay, Window win, GLXContext glc);
	
	instance* get_seleted_instance(vec3Di box_selector_pos);
	
	bool load_scene(std::string filename);
	void save_scene(std::string filename);
	
	
	
};


#endif