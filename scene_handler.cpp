#include "scene_handler.h"

//============================================
//======== METODOS DE TEXTURE HANDLER ========
//============================================

TextureHandler::TextureHandler()
{
	
}

TextureHandler::~TextureHandler()
{
	
}

void TextureHandler::gen_textures()
{
	GLuint texture;
	GLuint n_texture;
	
	auto color_texture = final_c_texture;
	auto normal_texture = final_n_texture;
	
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, color_texture->get_width(), color_texture->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, color_texture->get_pixels());
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	generated_textures.push_back(texture);
	
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &n_texture);
	glBindTexture(GL_TEXTURE_2D, n_texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, color_texture->get_width(), color_texture->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, normal_texture->get_pixels());
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	generated_textures.push_back(n_texture);
	
	unsigned int depth_map;
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	unsigned int depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	generated_textures.push_back(depth_map);
}

void TextureHandler::add_color_and_normals_texture(tlsHandler tileset_handler)
{
	color_textures.push_back(tileset_handler.get_selected_tile().color_texture);
	normal_textures.push_back(tileset_handler.get_selected_tile().normal_texture);
			
}

void TextureHandler::add_texture_name(std::string texture_name)
{
	textures_names.push_back(texture_name);
	
}





//============================================
//======== METODOS DE SCENER HANDLER ========
//============================================

SceneHandler::SceneHandler()
{
	
}

SceneHandler::~SceneHandler()
{
	
}


void SceneHandler::init_shaders()
{
	shader_handler.get_versions();
	
	shader_handler.parse_shaders("main_shader.glsl");
	shader_handler.create_shaders();
	shader_handler.create_attributes();
	
	shadow_shader_handler.parse_shaders("shadow_shader.glsl");
	shadow_shader_handler.create_shaders();
	shadow_shader_handler.create_attributes();
	
}


bool SceneHandler::add_tile(Glib::ustring filename)
{
	return tileset_handler.load_tile_from_file(filename);
}

void SceneHandler::add_sliced_object_to_scene(vec3Di box_selector_pos, int rotation)
{
	Sliced_Object selected_object = tileset_handler.get_selected_tile().get_selected_object();
	//std::cout << "La tila seleccionada es: " << selected_object.name << std::endl;
	
	selected_object.instancias.push_back({box_selector_pos, rotation});
	
	bool is_in = false;
	int index = 0;
	
	// Comprobamos que el objeto seleccionado no esté incluido en la escena
	
	for (int i = 0; i < int(sliced_objects.size()); i++) {
		if (sliced_objects[i].name == selected_object.name) {
			is_in = true;
			index = i;
			break;
		}
	}
	
	// Si está incluido, añadimos una instancia del mismo con la posicion del box_selecetor
	
	if (is_in) {
		bool instancia_is_in = false;
		
		for (auto instancia : sliced_objects[index].instancias) {
			if (instancia.is_equal_to(selected_object.instancias[0])) {
				instancia_is_in = true;
				break;
			}
		}
		if (!instancia_is_in) sliced_objects[index].instancias.push_back(selected_object.instancias[0]);
		
	}
	
	// En caso contrario, añadimos el objeto seleccionado a la escena así como las texturas (de color y de normales) del tileset al que pertenece 
	
	else {
		
		// Comprobamos que la textura no se haya añadido aún, a partir del nombre del objeto seleccionado
		
		int name_size = selected_object.name.size();
		std::string tileset_name_of_selected_object = selected_object.name.substr(0, name_size - 2);
		
		bool texture_is_in = false;
		
		for (Sliced_Object sl_obj : sliced_objects) {
			int name_size_2 = sl_obj.name.size();
			std::string mother_tileset = sl_obj.name.substr(0, name_size_2 - 2);
			
			if (tileset_name_of_selected_object == mother_tileset) {
				texture_is_in = true;
				break;
			}
		}
		
		// Por último, añadimos el objeto
		
		sliced_objects.push_back(selected_object);
		
		// Si no se ha añadido la textura, la añadimos
		
		if (!texture_is_in){
			
			texture_handler.add_color_and_normals_texture(tileset_handler);
			texture_handler.add_texture_name(tileset_name_of_selected_object);
			//texture_handler.gen_textures();
		
		}
		
		pack_textures();
		
	}
	
	add_slices_to_layered_mesh();
}

void SceneHandler::delete_sliced_object_from_scene(vec3Di box_selector_pos)
{
	for (Sliced_Object &obj : sliced_objects) {
		for (instance &inst : obj.instancias) {
			if (inst.position.is_equal_to(box_selector_pos)) {
				auto it = &inst - &obj.instancias[0];
				obj.instancias.erase(obj.instancias.begin() + it);
			}
		}
	}
	add_slices_to_layered_mesh();
}

void SceneHandler::add_slices_to_layered_mesh()
{
	// Buscamos la altura más alta entre los tileset que conforman nuestra escena
	
	int z_max = 0;
	int z_max_instances = 0;
	
	for (Sliced_Object obj : sliced_objects) {
		int name_size = obj.name.size();
		std::string tileset_name = obj.name.substr(0, name_size - 2);
		
		for (instance inst : obj.instancias) {
			if (z_max_instances < inst.position.z) z_max_instances = inst.position.z;
		}
		
		for (tileset tileSet : tileset_handler.tile_set_list) {
			if (tileSet.name == tileset_name) {
				if (z_max < tileSet.dimensions.z) z_max = tileSet.dimensions.z;
			}
		}
	}
	
	int sliced_counter_in_mesh = 0;
	
	layered_mesh.index_pool.resize(0);
	layered_mesh.vertex_pool.resize(0);
	layered_mesh.textcoord_pool.resize(0);
	layered_mesh.rotation_pool.resize(0);
	
	//std::cout << z_max << std::endl;
	
	for (int z = 0; z < (z_max + z_max_instances); z++) {
		for (Sliced_Object obj : sliced_objects) {
			auto uv_transformation = obj.uv_transformation;
			for (instance inst : obj.instancias) {
			
				slice* lamina = obj.get_slice(z - inst.position.z);  
			
				if (lamina != NULL) {
						
					auto geometry = lamina->rotar(inst.rotation);
					
					for (vec3D v : geometry) {
						float center_in_x = lamina->dimensiones.x / 2.0;
						float center_in_y = lamina->dimensiones.y / 2.0;
						
						layered_mesh.vertex_pool.push_back(v.x + inst.position.x + center_in_x);
						layered_mesh.vertex_pool.push_back(v.z + inst.position.z);
						layered_mesh.vertex_pool.push_back(v.y + inst.position.y + center_in_y);
						
						layered_mesh.rotation_pool.push_back(inst.rotation);
						//std::printf("Vertice: (%f, %f, %f); \n", v.x + inst.position.x, v.y + inst.position.y, v.z + inst.position.z);
						
						
					}
					
					for (vec2D uv : lamina->uvs) {
						
						layered_mesh.textcoord_pool.push_back(uv.x * uv_transformation.y + uv_transformation.x);
						layered_mesh.textcoord_pool.push_back(uv.y * uv_transformation.z);
						
						//std::printf("Vertice uv: (%lf, %lf); \n", uv.x, uv.y);
					}
					
					for (int indice : lamina->indices) {
						layered_mesh.index_pool.push_back(indice + (sliced_counter_in_mesh * 4));
					}
					
					sliced_counter_in_mesh++;
				}
				
			}
		
		}
	}
	
	
}

void SceneHandler::pack_textures()
{
	int text_height = 0;
	int text_width = 0;
	std::string text_name_max_sizey;
	
	for (int i = 0; i < int(texture_handler.color_textures.size()); i++) {
		text_width += texture_handler.color_textures[i]->get_width();
		
		if (texture_handler.color_textures[i]->get_height() > text_height) {
			text_height = texture_handler.color_textures[i]->get_height();
			text_name_max_sizey = texture_handler.textures_names[i];
		}
	}
	
	// Final color texture
	
	Cairo::RefPtr<Cairo::ImageSurface> final_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, text_width, text_height);
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(final_surface);
	
	cr->set_source_rgba(0.0, 0.0, 0.0, 0.0);
	cr->paint();
	
	// Final normal texture
	
	Cairo::RefPtr<Cairo::ImageSurface> final_n_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, text_width, text_height);
	Cairo::RefPtr<Cairo::Context> cr_n = Cairo::Context::create(final_n_surface);
	
	cr_n->set_source_rgba(0.0, 0.0, 0.0, 0.0);
	cr_n->paint();
	
	int x_increment = 0;
	
	for (int i = 0; i < int(texture_handler.color_textures.size()); i++) {
		
		cr->save();
		cr->translate(x_increment, 0);
		Gdk::Cairo::set_source_pixbuf(cr, texture_handler.color_textures[i], 0, 0);
		cr->paint();
		cr->restore();
		
		cr_n->save();
		cr_n->translate(x_increment, 0);
		Gdk::Cairo::set_source_pixbuf(cr_n, texture_handler.normal_textures[i], 0, 0);
		cr_n->paint();
		cr_n->restore();
		

		float scalar_y = float(texture_handler.color_textures[i]->get_height()) /  float(text_height);
		float scalar_x = float(texture_handler.color_textures[i]->get_width()) / float(text_width);
		
		float x_increment_scalar = float(x_increment) / float(text_width); 
		
		
		for (Sliced_Object &sobj : sliced_objects) {
			std::string t_name = sobj.name.substr(0, texture_handler.textures_names[i].size());
			
			if (texture_handler.textures_names[i] == t_name) {
				sobj.uv_transformation = {x_increment_scalar, scalar_x, scalar_y}; 
			}
		}

		x_increment += texture_handler.color_textures[i]->get_width();
	}
	
	texture_handler.final_c_texture = Gdk::Pixbuf::create(final_surface, 0, 0, final_surface->get_width(), final_surface->get_height());
	texture_handler.final_n_texture = Gdk::Pixbuf::create(final_n_surface, 0, 0, final_n_surface->get_width(), final_n_surface->get_height());
	texture_handler.gen_textures();
		
	
	
}

void SceneHandler::make_context_current(Display *xdisplay, Window win, GLXContext glc)
{
	glXMakeCurrent(xdisplay, win, glc);
}

void SceneHandler::bind_textures()
{
	
}

instance* SceneHandler::get_seleted_instance(vec3Di box_selector_pos)
{
	for (Sliced_Object &sl : sliced_objects) {
		for (instance &inst : sl.instancias) {
			if (inst.position.is_equal_to(box_selector_pos)) {
				auto it = &inst - &sl.instancias[0];
				
				return &sl.instancias[it];
			}
		}
	}
	
	return NULL;
}

bool SceneHandler::load_scene(std::string filename)
{
	std::cout << filename << std::endl;
	
	int filename_size = filename.size();
	std::string extension = filename.substr(filename_size - 4);
	
	//std::cout << "File Extension: " << extension << std::endl;
	
	if (extension == ".scn") {
		std::ifstream file;
		file.open(filename);
		
		if (file) {
			std::string line;
			while(getline(file, line)) {
				std::string prefijo = line.substr(0, 4);
				std::string info = line.substr(4);
				
				if (prefijo == "tp- ") {
					tileset_handler.load_tile_from_file(info);
				}
				
				if (prefijo == "ei- ") {
					char obj_named[30];
					instance instancia;
					
					sscanf(info.c_str(), "%s %d %d %d %d", obj_named, &instancia.position.x, &instancia.position.y, &instancia.position.z, &instancia.rotation);
					
					std::string obj_name;
					obj_name = obj_named;
					
					int selected_tile = 0;
					for (tileset &tileSet : tileset_handler.tile_set_list) {
						// Establecer la tileSet como selected y despues seleccinar el objeto a añadir
						
						std::string comparison_name = obj_name.substr(0, obj_name.size() - 2);
						if (tileSet.name == comparison_name) {
							tileset_handler.select_tile(selected_tile); 
							tileSet.selected_element = stoi(obj_name.substr(tileSet.name.size() + 1));
							add_sliced_object_to_scene(instancia.position, instancia.rotation);
							
						}
						
						selected_tile += 1;
					}
				}
			}
		}
		return true;
		
	}
	else {
		std::cerr << "Invalid file extension" << std::endl;
		return false;
	}
	
}

void SceneHandler::save_scene(std::string filename)
{
	if (sliced_objects.size() > 0 ) {
		std::ofstream file;
		file.open(filename + ".scn");
	
		if (!file) std::cerr << "File couldn't be opened" << std::endl;
		else {
			for (auto tile_set : tileset_handler.tile_set_list) {
				file << "tp- " << tile_set.path << std::endl;
			}
			
			for (Sliced_Object sobj : sliced_objects) {
				for (instance inst : sobj.instancias) {
					file << "ei- " << sobj.name << " " << inst.position.x << " " << inst.position.y << " " << inst.position.z << " " << inst.rotation << std::endl;  
				}
			}
			
		}
		file.close();
	}
}