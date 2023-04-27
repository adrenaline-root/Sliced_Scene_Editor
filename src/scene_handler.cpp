#include "../include/scene_handler.h"
#include <math.h>
#include <glm/gtx/io.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#define PI 3.14159265358979323846


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
	
	generated_textures.resize(0);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, color_texture->get_width(), color_texture->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, color_texture->get_pixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glGenerateMipmap(GL_TEXTURE_2D);
	
	generated_textures.push_back(texture);
	
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &n_texture);
	glBindTexture(GL_TEXTURE_2D, n_texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, color_texture->get_width(), color_texture->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, normal_texture->get_pixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glGenerateMipmap(GL_TEXTURE_2D);
	
	generated_textures.push_back(n_texture);
	
	
	std::cout << "Textures generated! " << generated_textures.size() << std::endl;
	
	

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
	// Init main shader
	Shader main_shader, grey_scale_shader, shadow_shader, deferred_shader, dsplce_mp_shader;
	
	main_shader.name = "main_shader";
	main_shader.parse_shaders("source-files/shaders/main_shader.glsl");
	main_shader.create_shaders();
	
	
	// Preparing mesh for main_shader
	
	standard_mesh.vertex_format.addAttribute({"vPosition", GL_FLOAT, 3});
	standard_mesh.vertex_format.addAttribute({"vTextCoords", GL_FLOAT, 2});
	standard_mesh.vertex_format.addAttribute({"vRotation", GL_FLOAT, 1});
	
	// Init grey_scale and ligth_direction shader
	
	grey_scale_shader.name = "grey_scale_shader";
	grey_scale_shader.parse_shaders("source-files/shaders/grey_scale_shader.glsl");
	grey_scale_shader.create_shaders();
	
	// Preparing grey_scale_mesh
	
	grey_scale_mesh.vertex_format.addAttribute({"vPosition", GL_FLOAT, 3});
	grey_scale_mesh.vertex_format.addAttribute({"vTextCoords", GL_FLOAT, 2});
	
	
	// Init shadow shader
	
	shadow_shader.name = "shadow_shader";
	shadow_shader.parse_shaders("source-files/shaders/shadow_shader.glsl");
	shadow_shader.create_shaders();
	
	// Preparing mesh for shadow_shader
	
	
	// Init cgf shader
	
	deferred_shader.name = "deferred_shader";
	deferred_shader.parse_shaders("source-files/shaders/deferred_shader.glsl");
	deferred_shader.create_shaders();
	
	FrameBuffer deferred_framebuffer;
	deferred_framebuffer.set_framebuffer(512, 512, 3);
	deferred_shader.frame_buffers.push_back(deferred_framebuffer);
	
	// Preparing mesh for cgf shader
	
	dsplce_mp_shader.name = "dsplce_mp_shader";
	dsplce_mp_shader.parse_shaders("source-files/shaders/dsplce_mp.glsl");
	dsplce_mp_shader.create_shaders();
	
	
	Shader comp_shadows_shader;
	comp_shadows_shader.name = "comp_shadows";
	comp_shadows_shader.parse_shaders("source-files/shaders/comp_shadows.glsl");
	comp_shadows_shader.create_shaders();
	
	Shader comp_shadow_test;
	comp_shadow_test.name = "comp_shadows2";
	comp_shadow_test.parse_shaders("source-files/shaders/compshadtest.glsl");
	comp_shadow_test.create_shaders();
	
	
	shader_handler.add_shader(main_shader);
	shader_handler.add_shader(grey_scale_shader);
	shader_handler.add_shader(shadow_shader);
	shader_handler.add_shader(deferred_shader);
	shader_handler.add_shader(dsplce_mp_shader);
	shader_handler.add_shader(comp_shadows_shader);
	shader_handler.add_shader(comp_shadow_test);
	
	

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
	
	standard_mesh.float_buffer_data.resize(0);
	standard_mesh.int_buffer_data.resize(0);
	
	std::vector<int> index_pool;
	std::vector<float> vertex_pool;
	std::vector<float> textcoord_pool;
	std::vector<float> rotation_pool;
	
	
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
						
						vertex_pool.push_back(v.x + inst.position.x + center_in_x);
						vertex_pool.push_back(v.z + inst.position.z);
						vertex_pool.push_back(v.y + inst.position.y + center_in_y);
						
						rotation_pool.push_back(inst.rotation);
						
					}
					
					for (vec2D uv : lamina->uvs) {

						textcoord_pool.push_back(uv.x * uv_transformation.y + uv_transformation.x);
						textcoord_pool.push_back(uv.y * uv_transformation.z);
						
						
						//std::printf("Vertice uv: (%lf, %lf); \n", uv.x, uv.y);
					}
					
					for (int indice : lamina->indices) {
						index_pool.push_back(indice + (sliced_counter_in_mesh * 4));
						
					}
					
					sliced_counter_in_mesh++;
				}
				
			}
		
		}
	}
	
	
	standard_mesh.add_float_data(vertex_pool);
	standard_mesh.add_float_data(textcoord_pool);
	standard_mesh.add_float_data(rotation_pool);
	standard_mesh.add_int_data(index_pool);
	
	
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

void SceneHandler::render_scene(int screen_width, int screen_heigth, float translation_x, float translation_y, float scale, float rotation)
{
	
	float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
	z_aspect_correction = 1.;
	
	Shader *main_shader_handler = shader_handler.get_shader_ptr("main_shader");
		
	main_shader_handler->use_program();
	main_shader_handler->load_mesh_to_buffers(standard_mesh);
		
	int gen_textures_size = texture_handler.generated_textures.size();
	GLuint texture = texture_handler.generated_textures[gen_textures_size - 2];
	GLuint texture_n = texture_handler.generated_textures[gen_textures_size - 1];
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glm::mat4 orto = glm::ortho(float(float(-screen_width) / 2.0), float(float(screen_width) / 2.0),
								float(float(-screen_heigth) / 2.0) / z_aspect_correction , float(float(screen_heigth) / 2.0 / z_aspect_correction),
								float(-screen_heigth * 10), float(screen_heigth * 10));
								//0.01f, float(screen_width * 2));
	main_shader_handler->set_glmmat4("viewport_matrix", orto);
	
	glm::mat4 rotation_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::rotate(rotation_matrix, glm::radians(float(-rotation)), glm::vec3(0., 1., 0.));
	main_shader_handler->set_glmmat4("rotation_matrix", rotation_matrix);
	
	glm::mat4 view_rotation = glm::mat4(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(-45.0f), glm::vec3(1., 0., 0.));
	main_shader_handler->set_glmmat4("view_rotation_matrix", view_rotation);
	
	
	
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translation_x * scale, 0.0, translation_y * scale));
	
	glm::mat4 scalem = glm::scale(trans, glm::vec3(scale, scale, scale));
	
	main_shader_handler->set_glmmat4("trans_scale_matrix", scalem);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	main_shader_handler->set_sampler("c_texture", 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_n);
	main_shader_handler->set_sampler("n_texture", 2);
	                   
	main_shader_handler->set_glmmat4("mat_rot90", glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0)));
	main_shader_handler->set_glmmat4("mat_rot180", glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)));
	main_shader_handler->set_glmmat4("mat_rot190", glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0)));
	

	main_shader_handler->check_gl_errors();
	
	glDrawElements(GL_TRIANGLES, int(standard_mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	main_shader_handler->unuse_program();
  
//	Some computation here
//	auto start = std::chrono::system_clock::now();
//	render_ligth_scene();
//	auto end = std::chrono::system_clock::now();
// 
// 	std::chrono::duration<double> elapsed_seconds = end-start;
//
//	std::cout << elapsed_seconds.count() << std::endl;
	
	

}



void SceneHandler::render_filtered_scene(int screen_width, int screen_heigth, float translation_x, float translation_y, float scale, float rotation)
{
	float divisor = 1.0;
	int width = (float)screen_width / divisor;
	int heigth = (float)screen_heigth / divisor;
	
	float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
	
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	//glActiveTexture(GL_TEXTURE3);
	glActiveTexture(GL_TEXTURE0);
	unsigned int framebuffer_texture;
	glGenTextures(1, &framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	} //else std::cout << "Framebuffer created succesfully!" << std::endl;
	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.2, 0.2, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glViewport(0, 0, width, heigth);

	glColor3f(1., 0., 1.);
	//render_scene(screen_width, screen_heigth, translation_x, translation_y, scale, rotation);
	render_grey_scale(screen_width, screen_heigth, translation_x, translation_y, scale, rotation);
	grey_scale = framebuffer_texture;

	glDeleteFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	
	glViewport(0, 0, screen_width, screen_heigth);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glColor3f(1., 1., 1.);
	
//	glBegin(GL_TRIANGLES);
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 1.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(1.0, 1.0);
//	glEnd();
//	
//	glBegin(GL_TRIANGLES);
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(1., 1.);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(1., -1.);
//		
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//	glEnd();
	
	//glDeleteTextures(1, &framebuffer_texture);
	
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	
	//main_shader_handler.check_gl_errors();
	
	
}

void SceneHandler::render_shadow_map(int screen_width, int screen_heigth, vec2D mouse_pos)
{

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glActiveTexture(GL_TEXTURE0);
	unsigned int framebuffer_texture;
	glGenTextures(1, &framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	
//	std::cout << "===========================" << std::endl;
//	std::cout << framebuffer << std::endl;
//	std::cout << framebuffer_texture << std::endl; 
//	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	} //else std::cout << "Framebuffer created succesfully!" << std::endl;
	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0., 0., 0., 0.);
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	
	float quad[] = {  -1., -1,
					-1., 1,
					1., 1,
					1., -1
	};
	
	float uv_quad[] = {	0., 0.,
						0., 1., 
						1., 1.,
						1., 0.
		
	};
	
	unsigned int vertex_buffer;
	glGenBuffers(1, &vertex_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(0); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	unsigned int uvs_buffer;
	glGenBuffers(1, &uvs_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_quad), &uv_quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(1); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<const void*>(2 * sizeof(float)));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	glEnable(GL_TEXTURE_2D);
	//unsigned int src_texture;
	//glGenTextures(1, &src_texture);
	glBindTexture(GL_TEXTURE_2D, grey_scale);
	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferdata->get_width(), bufferdata->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bufferdata->get_pixels());
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glViewport(0, 0, screen_width, screen_heigth);
//	
//	std::cout << vertex_buffer << std::endl;
//	std::cout << uvs_buffer << std::endl;
//	std::cout << src_texture << std::endl;
	
	
	Shader shadow_shader_handler = shader_handler.get_shader("shadow_shader");
	shadow_shader_handler.use_program();
	
	float mouse[] = {float(mouse_pos.x) / float(screen_width), (float(screen_heigth) - float(mouse_pos.y)) / float(screen_heigth), 0.0};
	shadow_shader_handler.set_vec("ligth_pos", mouse, 3);
	
	float viewPort_size[] = {float(screen_width), float(screen_heigth)};
	shadow_shader_handler.set_vec("viewport_size", viewPort_size, 2);
	
	
	glDrawArrays(GL_QUADS, 0, 8);
	
	shadow_shader_handler.unuse_program();
	
	
	glDeleteFramebuffers(1, &framebuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport(0, 0, screen_width, screen_heigth);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	
	glDeleteTextures(1, &grey_scale);
	grey_scale = framebuffer_texture;
	//shadow_map_tex = framebuffer_texture;
	
	
	glColor3f(1., 1., 1.);
	
	glBegin(GL_TRIANGLES);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
		
		glTexCoord2d(0.0, 1.0);
		glVertex2d(-1.0, 1.0);
		
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1.0, 1.0);
	glEnd();
	
	glBegin(GL_TRIANGLES);
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1., 1.);
		
		glTexCoord2d(1.0, 0.0);
		glVertex2d(1., -1.);
		
		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
	glEnd();
	
	
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDeleteFramebuffers(1, &framebuffer);
	
	glDeleteBuffers(1, &vertex_buffer);
	
	glDeleteBuffers(1, &uvs_buffer);
	
}

void SceneHandler::render_grey_scale(int screen_width, int screen_heigth, float translation_x, float translation_y, float scale, float rotation)
{
	
	float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
	
	Shader grey_scale_shader_handler = shader_handler.get_shader("grey_scale_shader");
	
	grey_scale_shader_handler.use_program();
	grey_scale_shader_handler.load_mesh_to_buffers(standard_mesh);
		
	int gen_textures_size = texture_handler.generated_textures.size();
	GLuint texture = texture_handler.generated_textures[gen_textures_size - 1];
	
	glViewport(0.0f, 0.0f, screen_width, screen_heigth);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glm::mat4 orto = glm::ortho(float(float(-screen_width) / 2.0f), float(float(screen_width) / 2.0f),
								float(float(-screen_heigth) / 2.0f), float(float(screen_heigth) / 2.0f),
								//float(float(-screen_heigth) / 2.0) * z_aspect_correction, float(float(screen_heigth) / 2.0) * z_aspect_correction,
								float(-screen_heigth * 10), float(screen_heigth * 10));
								//0.01f, float(screen_width * 2));
						
	grey_scale_shader_handler.set_glmmat4("viewport_matrix", orto);
	
	glm::mat4 rotation_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::rotate(rotation_matrix, glm::radians(float(-rotation)), glm::vec3(0., 1., 0.));
	grey_scale_shader_handler.set_glmmat4("rotation_matrix", rotation_matrix);
	
	glm::mat4 view_rotation = glm::mat4(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(-90.0f), glm::vec3(1., 0., 0.));
	grey_scale_shader_handler.set_glmmat4("view_rotation_matrix", view_rotation);
	
	
	
	//glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translation_x * scale - float(screen_width) / 2.0f, 0.0, translation_y * scale - float(screen_heigth) / 2.0f));
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translation_x * scale, 0.0, translation_y * scale));
	
	glm::mat4 scalem = glm::scale(trans, glm::vec3(scale, scale, scale));
	
	grey_scale_shader_handler.set_glmmat4("trans_scale_matrix", scalem);
	
	//grey_scale_shader_handler.set_sampler("c_texture", 1);

	grey_scale_shader_handler.check_gl_errors();
	
	glDrawElements(GL_TRIANGLES, int(standard_mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	grey_scale_shader_handler.unuse_program();
}

void SceneHandler::render_colored_and_grey_sceen(int screen_width, int screen_heigth, float translation_x, float translation_y, float scale, float rotation, vec2D mouse_pos)
{
	float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
	
	// CREAMOS UN FRAMEBUFFER Y LE ASOCIAMOS DOS TEXTURAS: UNA PARA LA ESCENA CON COLOR Y LA OTRA PARA LA ESCENA EN ESCALA DE GRISES (REPRESENTANDO LA ALTURA DE CADA FRAGMENTO)
	
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glActiveTexture(GL_TEXTURE0);
	unsigned int framebuffer_texture[3];
	glGenTextures(3, framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[0]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture[0], 0);
	
	
	//glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[1]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, framebuffer_texture[1], 0);
	
	
	
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[2]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, framebuffer_texture[2], 0);
	
	
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	} 
	
	glClearColor(0., 0., 0., 0.);
	glClear(GL_COLOR_BUFFER_BIT);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	
	// RENDERIZAMOS LA ESCENA
	
	Shader cgf_shader_handler = shader_handler.get_shader("cgf_shader");
	
	cgf_shader_handler.use_program();
	cgf_shader_handler.load_mesh_to_buffers(standard_mesh);
		
	int gen_textures_size = texture_handler.generated_textures.size();
	GLuint texture = texture_handler.generated_textures[gen_textures_size - 2];
	
	glEnable(GL_TEXTURE_2D);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glm::mat4 orto = glm::ortho(float(float(-screen_width) / 2.0), float(float(screen_width) / 2.0),
								float(float(-screen_heigth) / 2.0) / z_aspect_correction , float(float(screen_heigth) / 2.0 / z_aspect_correction),
								//float(float(-screen_heigth) / 2.0), float(float(screen_heigth) / 2.0),
								float(-screen_heigth * 10), float(screen_heigth * 10));
								//0.01f, float(screen_width * 2));
	cgf_shader_handler.set_glmmat4("viewport_matrix", orto);
	
	glm::mat4 rotation_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::rotate(rotation_matrix, glm::radians(float(-rotation)), glm::vec3(0., 1., 0.));
	cgf_shader_handler.set_glmmat4("rotation_matrix", rotation_matrix);
	
	glm::mat4 view_rotation = glm::mat4(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(-45.0f), glm::vec3(1., 0., 0.));
	cgf_shader_handler.set_glmmat4("view_rotation_matrix", view_rotation);
	
	
	
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translation_x * scale, 0.0, translation_y * scale));
	
	glm::mat4 scalem = glm::scale(trans, glm::vec3(scale, scale, scale));
	
	cgf_shader_handler.set_glmmat4("trans_scale_matrix", scalem);
	
	int gen_t_size = texture_handler.generated_textures.size();
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_handler.generated_textures[gen_t_size - 2]);
	
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_handler.generated_textures[gen_t_size - 1]);
	
	cgf_shader_handler.set_sampler("c_texture", 1);
	cgf_shader_handler.set_sampler("n_texture", 2);
	
	cgf_shader_handler.set_glmmat4("rot_90", glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0., 1., 0.)));
	cgf_shader_handler.set_glmmat4("rot_180", glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0., 1., 0.)));
	cgf_shader_handler.set_glmmat4("rot_270", glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0., 1., 0.)));
	
	GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);
	glClearColor(0., 0., 0., 0.);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFragDataLocation(cgf_shader_handler.program, 0, "frag_color");
	glBindFragDataLocation(cgf_shader_handler.program, 1, "normals_color");
	glBindFragDataLocation(cgf_shader_handler.program, 2, "grey_color");
	
	glDrawElements(GL_TRIANGLES, int(standard_mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);
	
	cgf_shader_handler.check_gl_errors();
	
	cgf_shader_handler.unuse_program();
	
	
	
	// DIBUJAMOS LAS TEXTURAS OBTENIDAS EN OTRO BUFFER
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	unsigned int framebuffer2;
	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
	
	glActiveTexture(GL_TEXTURE0);
	unsigned int final_texture;
	glGenTextures(1, &final_texture);
	glBindTexture(GL_TEXTURE_2D, final_texture);
	
//	std::cout << "===========================" << std::endl;
//	std::cout << framebuffer << std::endl;
//	std::cout << framebuffer_texture << std::endl; 
//	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, final_texture, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	} //else std::cout << "Framebuffer created succesfully!" << std::endl;
	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0., 0., 0., 0.);
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	glViewport(0, 0, screen_width, screen_heigth);
	
	glEnable(GL_TEXTURE_2D);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	
	
	// TOMAMOS LA TEXTURE PLANA EN GRIS
	
	
	float quad[] = {  -1., -1,
					-1., 1,
					1., 1,
					1., -1
	};
	
	float uv_quad[] = {	0., 0.,
						0., 1., 
						1., 1.,
						1., 0.
		
	};
	
	Shader dsplce_mp_shader_handler = shader_handler.get_shader("dsplce_mp_shader");
	
	dsplce_mp_shader_handler.use_program();
	
	unsigned int vertex_buffer;
	glGenBuffers(1, &vertex_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(0); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	unsigned int uvs_buffer;
	glGenBuffers(1, &uvs_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_quad), &uv_quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(1); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<const void*>(2 * sizeof(float)));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	glEnable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[0]);
	dsplce_mp_shader_handler.set_sampler("color_map", 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[1]);
	dsplce_mp_shader_handler.set_sampler("normals_map", 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture[2]);
	dsplce_mp_shader_handler.set_sampler("heigth_map", 2);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grey_scale);
	dsplce_mp_shader_handler.set_sampler("shadow_map", 3);

	
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	
	dsplce_mp_shader_handler.set_float("height_correction", z_aspect_correction);
	
	
	float mouse[] = {float(mouse_pos.x) / float(screen_width), (float(screen_heigth) - float(mouse_pos.y)) / float(screen_heigth), 0.0};
	dsplce_mp_shader_handler.set_vec("ligth_pos", mouse, 3);
	
	float viewport_size[] = {float(screen_width), float(screen_heigth)};
	dsplce_mp_shader_handler.set_vec("viewport_size", viewport_size, 2);
	
	dsplce_mp_shader_handler.set_glmmat4("rot_mat", glm::rotate(glm::mat4(1.0f), glm::radians(-rotation), glm::vec3(0., 1., 0.)));
	dsplce_mp_shader_handler.set_float("scale", scale);
	
	
	glDrawArrays(GL_QUADS, 0, 8);
	dsplce_mp_shader_handler.unuse_program();
	dsplce_mp_shader_handler.check_gl_errors();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, final_texture);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glColor3f(1., 1., 1.);
	
	glBegin(GL_TRIANGLES);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
		
		glTexCoord2d(0.0, 1.0);
		glVertex2d(-1.0, 1.0);
		
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1.0, 1.0);
	glEnd();
	glBegin(GL_TRIANGLES);
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1., 1.);
		
		glTexCoord2d(1.0, 0.0);
		glVertex2d(1., -1.);
		
		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
	// ELIMINAMOS EL FRAMEBUFFER Y LAS TEXTURAS
	
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(1, &framebuffer2);
	
	glDeleteTextures(3, framebuffer_texture);
	glDeleteTextures(1, &grey_scale);
	glDeleteTextures(1, &final_texture);
	
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &uvs_buffer);
	
	std::cout << grey_scale << std::endl;
}

void SceneHandler::render_scene_lt(int screen_width, int screen_heigth, float translation_x, float translation_y, float scale, float rotation)
{
	
	// Draw Color_layer, Normals_layer and Positions_layer to 3 textures
	
	Shader *deferred_shader = shader_handler.get_shader_ptr("deferred_shader");
	
	deferred_shader->frame_buffers[0].prepare_target_textures(screen_width, screen_heigth);
	deferred_shader->frame_buffers[0].make_target();

	glClearColor(0., 0., 0., 0.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	

	// RENDERIZAMOS LA ESCENA
	
	deferred_shader->use_program();
	deferred_shader->load_mesh_to_buffers(standard_mesh);
		
	int gen_textures_size = texture_handler.generated_textures.size();
	GLuint texture = texture_handler.generated_textures[gen_textures_size - 2];
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
	z_aspect_correction = 1.0;
	
	glm::mat4 orto = glm::ortho(float(float(-screen_width) / 2.0), float(float(screen_width) / 2.0),
								float(float(-screen_heigth) / 2.0) / z_aspect_correction , float(float(screen_heigth) / 2.0 / z_aspect_correction),
								float(-screen_heigth * 10), float(screen_heigth * 10));
	
	deferred_shader->set_glmmat4("viewport_matrix", orto);
	
	glm::mat4 rotation_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::rotate(rotation_matrix, glm::radians(float(-rotation)), glm::vec3(0., 1., 0.));
	deferred_shader->set_glmmat4("rotation_matrix", rotation_matrix);
	
	glm::mat4 view_rotation = glm::mat4(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(-45.0f), glm::vec3(1., 0., 0.));
	deferred_shader->set_glmmat4("view_rotation_matrix", view_rotation);
	
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translation_x * scale, 0.0, translation_y * scale));
	
	glm::mat4 scalem = glm::scale(trans, glm::vec3(scale, scale, scale));
	
	deferred_shader->set_glmmat4("trans_scale_matrix", scalem);
	
	float viewport[] = {float(screen_width), float(screen_heigth)};
	deferred_shader->set_vec("viewport", viewport, 2);
	
	int gen_t_size = texture_handler.generated_textures.size();
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_handler.generated_textures[gen_t_size - 2]);
	
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_handler.generated_textures[gen_t_size - 1]);
	
	deferred_shader->set_sampler("c_texture", 1);
	deferred_shader->set_sampler("n_texture", 2);

	deferred_shader->set_glmmat4("rot_90", glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0., 1., 0.)));
	deferred_shader->set_glmmat4("rot_180", glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0., 1., 0.)));
	deferred_shader->set_glmmat4("rot_270", glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0., 1., 0.)));
	
	GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	glBindFragDataLocation(deferred_shader->program, 0, "frag_color");
	glBindFragDataLocation(deferred_shader->program, 1, "normals_color");
	glBindFragDataLocation(deferred_shader->program, 2, "positions_color");
	
	glDrawElements(GL_TRIANGLES, int(standard_mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);
	
	
	deferred_shader->unuse_program();
	
	deferred_shader->check_gl_errors();
	
	
	//float *data = new float[screen_width * screen_heigth];
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, deferred_shader->frame_buffers[0].attachments_ids[2]);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//	glReadPixels(0, 0, screen_width, screen_heigth, GL_RGBA, GL_FLOAT, data);
//	
//	int channels = 1;
//	int rowstride = screen_width * channels;
//	int x = screen_width / 2;
//	int y = screen_heigth / 2 - 100;
//	float value = data[x * channels + y * rowstride];
//	
//	for (int xi = x * channels; xi < rowstride; xi++) {
//		std::cout << "Pixel at: " << x + (xi - x * channels) / channels << ", " << y << " =========="<< std::endl;
//		for (int i = 0; i < 4; i++) {
//			value = data[ (xi + i) * channels + y * rowstride];
//			std::cout << std::to_string(value) << std::endl;
//			
//		}
//	}
	
	deferred_shader->frame_buffers[0].unmake_target();
	
	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, deferred_shader->frame_buffers[0].attachments_ids[0]);
//	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	
	glColor3f(1., 1., 1.);
//	
//	glBegin(GL_TRIANGLES);
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 1.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(1.0, 1.0);
//	glEnd();
	
//	glBindTexture(GL_TEXTURE_2D, deferred_shader->frame_buffers[0].attachments_ids[2]);
//	
//	glBegin(GL_QUADS);
//	
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 1.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(1.0, 1.0);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(1., -1.);
//	
//	glEnd();
	
	
	
	deferred_shader = NULL;
	delete(deferred_shader);
	
}

void SceneHandler::render_compute_output(int screen_width, int screen_heigth)
{
	
	float quad[] = { -1., 1.,
					1., 1.,
					1., -1.,
					-1., -1.
	};
	
	float uv_quad[] = {	0., 0.,
						0., 1., 
						1., 1.,
						1., 0.
		
	};
	
	
	
	Shader *comp_shadows = shader_handler.get_shader_ptr("comp_shadows");
	comp_shadows->use_program();

	unsigned int vertex_buffer;
	glGenBuffers(1, &vertex_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(0); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	unsigned int uvs_buffer;
	glGenBuffers(1, &uvs_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_quad), &uv_quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(1); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<const void*>(2 * sizeof(float)));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //

	
	unsigned int color_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[0];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	
	comp_shadows->set_sampler("color_texture", 0);
	
	unsigned int normals_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[1];
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normals_texture);
	
	comp_shadows->set_sampler("normals_texture", 1);
	
	unsigned int positions_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[2];
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, positions_texture);
	
	comp_shadows->set_sampler("positions_texture", 2);
	
	unsigned int shadow_map;
	glGenTextures(1, &shadow_map);
	
	glActiveTexture(GL_TEXTURE3);
	//glActiveTexture(GL_TEXTURE0);
	ligths_handler.ligths[0].load_oclusion_map_to_texture(shadow_map);
	Ligth ligth = ligths_handler.ligths[0];
	vec3D ligth_pos = ligths_handler.ligths[0].position;
	
	glBindTexture(GL_TEXTURE_2D, shadow_map);
	
	comp_shadows->set_sampler("shadow_map", 3);
	
	
	glm::mat4 translation_mat = glm::translate(glm::mat4(1.0), glm::vec3(-ligth_pos.x, -ligth_pos.y, -ligth_pos.z));
	comp_shadows->set_glmmat4("translation_mat", translation_mat);

	
	float ligth_angle = atan(ligth.direction.z / ligth.direction.x);

	glm::mat4 rotation_mat = glm::rotate(glm::mat4(1.0), glm::radians(-ligth_angle), glm::vec3(0.0, 1.0, 0.0));
	comp_shadows->set_glmmat4("rotation_mat", rotation_mat);
	
	float ligth_posf[] = {ligth_pos.x, ligth_pos.y, ligth_pos.z};
	comp_shadows->set_vec("ligth_pos", ligth_posf, 3);
	
	float viewport[] = {screen_width, screen_heigth};
	comp_shadows->set_vec("viewport", viewport, 2);
	
	
	glViewport(0, 0, screen_width, screen_heigth);
	glDrawArrays(GL_QUADS, 0, 8);
	
	
	comp_shadows->unuse_program();
	
	for(int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, shadow_map);

//glViewport(0, 0, 2, 2);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glColor3f(1., 1., 1.);
//	glEnable(GL_TEXTURE_2D);
//	
//	glBegin(GL_QUADS);
//	
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 0.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(0.0, 0.0);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(0., -1.);
//	
//	glEnd();
//	
	
	
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, & uvs_buffer);
	glDeleteTextures(1, &shadow_map);


	comp_shadows = NULL;
	delete(comp_shadows);
}

void SceneHandler::compute_shadows(int screen_width, int screen_heigth)
{
	
	float quad[] = { -1., 1.,
					1., 1.,
					1., -1.,
					-1., -1.
	};
	
	float uv_quad[] = {	0., 0.,
						0., 1., 
						1., 1.,
						1., 0.
		
	};
	
	
	
	Shader *comp_shadows = shader_handler.get_shader_ptr("comp_shadows2");
	comp_shadows->use_program();

	unsigned int vertex_buffer;
	glGenBuffers(1, &vertex_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(0); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	unsigned int uvs_buffer;
	glGenBuffers(1, &uvs_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_quad), &uv_quad, GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(1); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<const void*>(2 * sizeof(float)));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //

	
	unsigned int color_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[0];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	
	comp_shadows->set_sampler("color_texture", 0);
	
	unsigned int normals_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[1];
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normals_texture);
	
	comp_shadows->set_sampler("normals_texture", 1);
	
	unsigned int positions_texture = shader_handler.get_shader_ptr("deferred_shader")->frame_buffers[0].attachments_ids[2];
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, positions_texture);
	
	comp_shadows->set_sampler("positions_texture", 2);
	
	unsigned int tile_map;
	glGenTextures(1, &tile_map);
	glActiveTexture(GL_TEXTURE3);
	ligths_handler.load_pixbuf_to_texture(tile_map, ligths_handler.tile_map);
	glBindTexture(GL_TEXTURE_2D, tile_map);
	comp_shadows->set_sampler("tile_map", 3);
	
	unsigned int h_map1;
	glGenTextures(1, &h_map1);
	glActiveTexture(GL_TEXTURE4);
	ligths_handler.load_pixbuf_to_texture(h_map1, ligths_handler.heigth_map1);
	glBindTexture(GL_TEXTURE_2D, h_map1);
	comp_shadows->set_sampler("h_map1", 4);
	
	unsigned int h_map2;
	glGenTextures(1, &h_map2);
	glActiveTexture(GL_TEXTURE5);
	ligths_handler.load_pixbuf_to_texture(h_map2, ligths_handler.heigth_map2);
	glBindTexture(GL_TEXTURE_2D, h_map2);
	comp_shadows->set_sampler("h_map2", 5);
	
//	Ligth *ligth = &ligths_handler.ligths[0];
//	
//	float ligth_posf[] = {ligth->position.x, ligth->position.y, ligth->position.z};
//	comp_shadows->set_vec("ligth_pos", ligth_posf, 3);
//	
//	float ligth_dir[] = {ligth->direction.x, ligth->direction.y, ligth->direction.z};
//	comp_shadows->set_vec("ligth_dir", ligth_dir, 3);
//	
//	float ambient[] = {ligth->ambient.x, ligth->ambient.y, ligth->ambient.z};
//	comp_shadows->set_vec("ambient", ambient, 3);
//	
//	float specular[] = {ligth->specular.x, ligth->specular.y, ligth->specular.z};
//	comp_shadows->set_vec("specular", specular, 3);
//	
//	ligth->push_ligth_into_shader(comp_shadows);
//	
	comp_shadows->set_int("NUMBER_LIGTHS", int(ligths_handler.ligths.size()));
	
	int index = 0;
	for (auto ligth : ligths_handler.ligths) {
		ligth.push_ligth_into_shader(comp_shadows, index);
		index++;
	}
	
	//ligth = NULL;
	//delete(ligth);

	
	glViewport(0, 0, screen_width, screen_heigth);
	glDrawArrays(GL_QUADS, 0, 8);
	
	comp_shadows->check_gl_errors();
	comp_shadows->unuse_program();
	
	for(int i = 0; i < 6; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

	glActiveTexture(GL_TEXTURE0);
	
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, & uvs_buffer);
	
	glDeleteTextures(1, &tile_map);
	glDeleteTextures(1, &h_map1);
	glDeleteTextures(1, &h_map2);


	comp_shadows = NULL;
	delete(comp_shadows);
	
}