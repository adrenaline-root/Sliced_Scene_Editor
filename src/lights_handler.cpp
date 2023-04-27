#include "../include/ligths_handler.h"



int screen_width = 1024;
int screen_heigth = 1024;

// LIGTH METHODS

Ligth::Ligth()
{
	
}

Ligth::~Ligth() 
{
	
}

void Ligth::render_oclusion_map(Glib::RefPtr<Gdk::Pixbuf> tilemap, Glib::RefPtr<Gdk::Pixbuf> heigth_map1, Glib::RefPtr<Gdk::Pixbuf> heigth_map2, vec2Di screen)
{
	int screen_size = 256;
	
	
	oclusion_map = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, screen_size, int(screen_size))->copy();
	oclusion_map->fill(0x00000000);
	Glib::RefPtr<Gdk::Pixbuf> pix_scan = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, screen.x, screen.y);
	pix_scan->fill(0x00000000);
	
	float fov = 33.;
	
	
	// Variables para la transformación de las y en pantalla
	vec2D center_screen = {float(oclusion_map->get_width()) / 2., float(oclusion_map->get_height()) / 2.}; 
	float heigth_ratio = screen_size / 100.;
	float dist_incr = abs((screen_size / 2.0f) / tan(glm::radians(fov)));

		
	glm::vec4 v_view = glm::vec4(direction.x, direction.y, direction.z, 0.0);
	
	// Obtenemos Perperdicular al v_view;
	
	glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::vec4 v_screen = v_view * rot_mat;
	v_screen.z *= -1.;
		
	// Starting Points de view y screen
	
	float adyacente = (screen_size / 2.0) / tan(glm::radians(15.0f));
	vec2D horizon_point = {position.x - (adyacente * v_view.x), position.z + (adyacente * v_view.z)};
	vec2D screen_point = {(horizon_point.x) - ((float)screen_size / 2.0 * v_screen.x), (horizon_point.y) - (float(screen_size) / 2.0 * v_screen.z)};

	
	int tile_size = 64;
	
	vec2Di ligth_in_tile = {int(position.x / tile_size), int(position.z / tile_size)};
	
	int vline_left_at = ligth_in_tile.x * tile_size;
	int vline_rigth_at = (ligth_in_tile.x + 1) * tile_size;
	
	int hline_bottom_at = (ligth_in_tile.y) * tile_size;
	int hline_top_at = (ligth_in_tile.y + 1) * tile_size;
		
	float pix_in_SC_incr = 1.0;

	vec2D scale_factor = {float(screen_width) / float(tilemap->get_width() * 16), float(screen_heigth) / float(tilemap->get_height() * 16)};
	
		
	//Some computation here
	
	auto start = std::chrono::system_clock::now();

	int screen_color[] = {255, 255, 0, 255};
	float degrees_ratio = (30.) / float(screen_size);

	for (float pix_inSW = 0.0; pix_inSW < screen_size; pix_inSW += pix_in_SC_incr) {
		
		int max_heigth = 0;
		vec2D drawed_line = {0, 0};
		vec2D current_ps = {screen_point.x + v_screen.x * pix_inSW, screen_point.y + v_screen.z * pix_inSW};
		
		//draw_pixel_at(int(current_ps.x), screen.y - int(current_ps.y), pix_scan, screen_color);
		
		float ray_angle = atan(float((current_ps.y) - (position.z)) / float(position.x - (current_ps.x) ));
		float ray_dist = sqrt(pow(position.x - current_ps.x, 2) + pow(position.z - current_ps.y, 2));

		
		glm::vec4 v_ray = glm::vec4(float((current_ps.x - position.x) / abs(ray_dist)), 0.0f, float(((current_ps.y) - (position.z)) / abs(ray_dist)), 0.0f );
		
		// Calculo del angulo diferencia entre v_view y v_ray
		
//		double cross_product = (v_view.x * v_ray.x) + (v_view.z * -v_ray.z);
//		double magnitud = sqrt(pow(v_view.x, 2) + pow(v_view.z, 2)) * sqrt(pow(v_ray.x, 2) + pow(-v_ray.z, 2));
//		
//		double cosen = cross_product / magnitud;
//		
//		if (cosen < -1.0 || cosen > 1.0) {
//			cosen /= abs(cosen);
//		}
//		
//		float alpha = acos(cosen);

		float alpha = glm::radians(-15. + pix_inSW * degrees_ratio);
		
		vec2D starting_lines = {0., 0.};
		vec2Di jump_dir = {0, 0};
		
		if (v_ray.x < 0.0) {
			starting_lines.x = vline_left_at;
			jump_dir.x = -tile_size;
		} else {
			starting_lines.x = vline_rigth_at;
			jump_dir.x = tile_size;
		}
		
		if (v_ray.z < 0.0) {
			starting_lines.y = hline_bottom_at;
			jump_dir.y = -tile_size;
		} else {
			starting_lines.y = hline_top_at;
			jump_dir.y = tile_size;
		}
		
		vec2D intersect_point = {position.x, position.z};

		while (int(intersect_point.x) >= 0 && int(intersect_point.x) < screen_width && int(intersect_point.y) >= 0 && int(intersect_point.y) < screen_heigth) {
				
			int factor_x = 0;
			int factor_y = 0;
			
			if (abs(jump_dir.x) != 1) {
		
				if (intersect_point.x == starting_lines.x) factor_x = jump_dir.x / 2;
				if (intersect_point.y == starting_lines.y) factor_y = jump_dir.y / 2;
			}
			
			else {
				int *pixel_hm = get_pixel_at(int(intersect_point.x), heigth_map1->get_height() - int(intersect_point.y), heigth_map1);
				int *pixel_hm_down = get_pixel_at(int(intersect_point.x), heigth_map2->get_height() - int(intersect_point.y), heigth_map2);
				
				
				// Calculamos las posiciones del pixel en la pantalla
				
				if (pixel_hm && pixel_hm[0] != 0 && pixel_hm[3] != 0) {//&& pixel_hm[0] > max_heigth) {
					
					int pixel_height = pixel_hm[0];
					int pixel_botom = pixel_hm_down[0];
					max_heigth = pixel_height;
					
					
					//float r_dist = sqrt(pow(position.z - (intersect_point.y), 2) + pow(position.x - (intersect_point.x), 2));
					float r_dist = (position.x - intersect_point.x) / cos(ray_angle); 
					float distant_to_pixel = abs(cos(alpha)) * abs((r_dist));
					
					
					float dy = center_screen.y * distant_to_pixel / dist_incr;
					float ratio = center_screen.y / dy;
					
					float half_screen = screen_size / 2;
					
					float y_pos = pixel_botom * heigth_ratio - center_screen.y;
					//float y_pos = 0 - ph_altitude;
					float y_in_screen = center_screen.y + (y_pos * ratio);
					y_in_screen = half_screen + (pixel_botom * heigth_ratio - half_screen) * dist_incr / (distant_to_pixel);
					
					float z_pos = (pixel_height * heigth_ratio) - center_screen.y;
					float dz = center_screen.y + (z_pos * ratio);
					dz = half_screen + (pixel_height * heigth_ratio - half_screen) * dist_incr / (distant_to_pixel);
					
					
					int current_x = pix_inSW;
					//int color[] =  {pixel_height, pixel_height, pixel_height, 255};
					float length = glm::length(glm::vec3(position.x - intersect_point.x, half_screen - dz, position.z - intersect_point.y));
					int sample = (1.0 - abs(distant_to_pixel) / (1024.)) * 255.0;
					int color[] = {sample, sample, sample, 255};
					
					if (y_in_screen <= 1.0) y_in_screen = 1.0;
					if (y_in_screen > oclusion_map->get_height() - 1) y_in_screen = oclusion_map->get_height() - 1;
					if (dz > oclusion_map->get_height() - 1) dz = oclusion_map->get_height() - 1;
					//std::cout << y_in_screen << ", " << dz << std::endl;
				
					if (y_in_screen < drawed_line.x) {
						if (dz < drawed_line.x) {
							for (int i = 0; i < int(pix_in_SC_incr); i++) draw_line(current_x + i, y_in_screen, dz, oclusion_map, color);
						}
						else {
							for (int i = 0; i < int(pix_in_SC_incr); i++) draw_line(current_x + i, y_in_screen, drawed_line.x, oclusion_map, color);
						}
					}
					
					if (dz > drawed_line.y){
						if (y_in_screen >= drawed_line.y) {
							for (int i = 0; i < int(pix_in_SC_incr); i++) draw_line(current_x + i, y_in_screen, dz, oclusion_map, color);
						}
						else {
							for (int i = 0; i < int(pix_in_SC_incr); i++) draw_line(current_x + i, drawed_line.y, dz, oclusion_map, color);
						}
					}
					
					
					drawed_line.x = y_in_screen;
					drawed_line.y = dz;
				}
				
				
				delete(pixel_hm);
				delete(pixel_hm_down);
			} 
			
			vec2D check_point = { int(intersect_point.x) + v_ray.x + factor_x, int(intersect_point.y) + v_ray.z + factor_y};
			
			vec2D n_interst_point = {check_point.x / screen_width * scale_factor.x, 1. - check_point.y / screen_heigth * scale_factor.y};
			int *pixel_at = get_pixel_at(int(n_interst_point.x * float(tilemap->get_width())), int(n_interst_point.y * float(tilemap->get_height())), tilemap);
			
			int tile_manipulation = 64;
			
			if (pixel_at) {
				
				if (pixel_at[1] == 255) tile_manipulation = 1;
				else if (pixel_at[0] == 255) tile_manipulation = 16;
				else if (pixel_at[2] == 255) tile_manipulation = 32;
				else if (pixel_at[0] != 255 && pixel_at[1] != 255 && pixel_at[2] != 255) tile_manipulation = 64;
			
				int decr = int(intersect_point.x) % tile_manipulation;
				int incr = tile_manipulation - decr;
				
				jump_dir.x = (jump_dir.x / abs(jump_dir.x)) * tile_manipulation;
				jump_dir.y = (jump_dir.y / abs(jump_dir.y)) * tile_manipulation;
				
				if (starting_lines.x > intersect_point.x) starting_lines.x = int(intersect_point.x) + incr;
				else {
					starting_lines.x = int(intersect_point.x) - decr;
					if (int(starting_lines.x) == int(intersect_point.x)) starting_lines.x += jump_dir.x;
				}
				
				decr = int(intersect_point.y) % tile_manipulation;
				incr = tile_manipulation - decr;
				
				if (starting_lines.y > intersect_point.y) starting_lines.y = int(intersect_point.y) + incr;
				else {
					starting_lines.y = int(intersect_point.y) - decr;
					if (int(starting_lines.y) == int(intersect_point.y)) starting_lines.y += jump_dir.y;
				}
				
			
			}
			delete(pixel_at);
			
			float y_ray_length = abs((intersect_point.y - starting_lines.y) / sin(ray_angle));
			float x_ray_length = abs((intersect_point.x - starting_lines.x) / cos(ray_angle));
			
			if(y_ray_length < x_ray_length) {
				intersect_point.x = position.x + (position.z - starting_lines.y) / tan(ray_angle);
				intersect_point.y = starting_lines.y;
				starting_lines.y += jump_dir.y;
				
			}
			else {
				intersect_point.y = position.z + (position.x - starting_lines.x) * tan(ray_angle);
				intersect_point.x = starting_lines.x;
				starting_lines.x += jump_dir.x;
			}
			
			if (max_heigth > 90) break;
		}

		
	}

//	auto end = std::chrono::system_clock::now();
//	std::chrono::duration<double> elapsed_seconds = end - start;
//	std::cout << elapsed_seconds.count() << std::endl;

//	unsigned int heigth_map;
//	glGenTextures(1, &heigth_map);
//	glBindTexture(GL_TEXTURE_2D, heigth_map);
//	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heigth_map1->get_width(), heigth_map1->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, heigth_map1->get_pixels());
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	
//	unsigned int screen_buffer;
//	glGenTextures(1, &screen_buffer);
//	glBindTexture(GL_TEXTURE_2D, screen_buffer);
//	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, oclusion_map->get_width(), oclusion_map->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, oclusion_map->get_pixels());
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	
//	unsigned int oclusion_buffer;
//	glGenTextures(1, &oclusion_buffer);
//	glBindTexture(GL_TEXTURE_2D, oclusion_buffer);
//	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tilemap->get_width(), tilemap->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tilemap->get_pixels());
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	
//	unsigned int scan_buffer;
//	glGenTextures(1, &scan_buffer);
//	glBindTexture(GL_TEXTURE_2D, scan_buffer);
//	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pix_scan->get_width(), pix_scan->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pix_scan->get_pixels());
//	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	
//	
//	
//	glUseProgram(0);
//	
//	glDisable(GL_TEXTURE_2D);
//	
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	
//	vec2D scale = {2. / screen.x * 1024., 2. / screen.y * 1024.};
//	
//
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, heigth_map);
//
//	glColor4f(1., 1., 1., 1.);
//	
//	glBegin(GL_QUADS);
//	
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, 1.0 - scale.y);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 1.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(-1.0 + scale.x, 1.0);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(-1. + scale.x, 1. - scale.y);
//	
//	glEnd();
//	
//	
//	glBindTexture(GL_TEXTURE_2D, oclusion_buffer);
//
//	glColor4f(1., 1., 1., .1);
//	
//	glBegin(GL_QUADS);
//	
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, 1.0 - scale.y);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 1.0);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(-1.0 + scale.x, 1.0);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(-1. + scale.x, 1. - scale.y);
//	
//	glEnd();
//	
//	
//	
//	glBindTexture(GL_TEXTURE_2D, scan_buffer);
//
//	glColor4f(1., 1., 1., 1.);
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
//	
//
//	glBindTexture(GL_TEXTURE_2D, screen_buffer);
//
//	glColor3f(1., 1., 1.);
//	
//	glEnable(GL_TEXTURE_2D);
//	
//	glBegin(GL_QUADS);
//	
//		glTexCoord2d(0.0, 0.0);
//		glVertex2d(-1.0, -1.0);
//		
//		glTexCoord2d(0.0, 1.0);
//		glVertex2d(-1.0, 0.);
//		
//		glTexCoord2d(1.0, 1.0);
//		glVertex2d(0.0, 0.0);
//		
//		glTexCoord2d(1.0, 0.0);
//		glVertex2d(0., -1.);
//	
//	glEnd();
//	
//	glDeleteTextures(1, &heigth_map);
//	glDeleteTextures(1, &screen_buffer);
//	glDeleteTextures(1, &scan_buffer);
//	glDeleteTextures(1, &oclusion_buffer);
	

}

void Ligth::load_oclusion_map_to_texture(unsigned int texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, oclusion_map->get_width(), oclusion_map->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, oclusion_map->get_pixels());
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Ligth::push_ligth_into_shader(Shader *shader, int index)
{
	std::string uniform = "ligths[" + std::to_string(index) + "].";
	
	shader->set_vec((uniform + "position").c_str(), new float[3]{position.x, position.y, position.z}, 3);
	
	glm::vec3 normalized = glm::normalize(glm::vec3(direction.x, direction.y, direction.z)); 
	shader->set_vec((uniform + "direction").c_str(), new float[3]{normalized.x, normalized.y, normalized.z}, 3);
	
	shader->set_vec((uniform + "ambient").c_str(), new float[3]{ambient.x, ambient.y, ambient.z}, 3);
	shader->set_float((uniform + "frustrum").c_str(), frustrum);
	shader->set_float((uniform + "strength").c_str(), strength);
	shader->set_float((uniform + "attenuation").c_str(), attenuation);
	
}


// LIGTH HANDLER METHODS

LigthsHandler::LigthsHandler()
{
}

LigthsHandler::~LigthsHandler()
{
	
}

void LigthsHandler::prepare_shaders()
{
	int screen_width = 1024;
	int screen_heigth = 1024;
	
	// Preparing Light Shaders
	Shader heigth_map_shader;
	heigth_map_shader.name = "heigth_map_shader";
	heigth_map_shader.parse_shaders("source-files/shaders/heigth_map_shader.glsl");
	heigth_map_shader.create_shaders();
	
	
	// Preparing frame buffers target for height map
	
	FrameBuffer frame_buffer;
	frame_buffer.set_framebuffer(screen_width, screen_heigth, 1);
	heigth_map_shader.frame_buffers.push_back(frame_buffer);
	
	shader_handler.add_shader(heigth_map_shader);
	
}

void LigthsHandler::add_ligth(Ligth ligth)
{
	int size = ligths.size();
	ligth.name = "Ligth" + std::to_string(size);
	ligths.push_back(ligth);
}

void LigthsHandler::remove_ligth(std::string name)
{
	
}

Ligth LigthsHandler::get_ligth(std::string name)
{
	Ligth light_ret;

	for (Ligth ligth : ligths) {
		if (ligth.name == name) {
			light_ret = ligth;
			break;
		}
	}
	
	return light_ret;
}

Ligth* LigthsHandler::get_selected_ligth()
{
	return &ligths[selected_ligth];
}


// Prepare height maps for RayCasting
void LigthsHandler::render_heigth_maps(StandardMesh mesh, unsigned int texture)
{
	Shader *hm_shader = shader_handler.get_shader_ptr("heigth_map_shader");
	hm_shader->use_program();
	hm_shader->load_mesh_to_buffers(mesh);

	
	glViewport(0.0f, 0.0f, screen_width, screen_heigth);
	
	
	hm_shader->frame_buffers[0].prepare_target_textures(screen_width, screen_heigth);
	hm_shader->frame_buffers[0].make_target();
	
	glClearColor(0., 0., 0., 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glm::mat4 orto = glm::ortho(float(float(-screen_width) / 2.0f), float(float(screen_width) / 2.0f),
								float(float(-screen_heigth) / 2.0f), float(float(screen_heigth) / 2.0f),
								float(-screen_heigth * 10), float(screen_heigth * 10));
						
	hm_shader->set_glmmat4("viewport_matrix", orto);
	
	glm::mat4 view_rotation = glm::mat4(1.0f);
	view_rotation = glm::rotate(view_rotation, glm::radians(-90.0f), glm::vec3(1., 0., 0.));
	
	hm_shader->set_glmmat4("view_rotation_matrix", view_rotation);
	
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(-screen_width / 2.0, 0.0, -screen_heigth / 2.0));
	hm_shader->set_glmmat4("translation_matrix", trans);
	
	
	hm_shader->check_gl_errors();
	
	glDrawElements(GL_TRIANGLES, int(mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);

	
	// Guardamos el primer height map (de abajo a arriba)
	guint8 *data = new guint8[screen_width * screen_heigth * sizeof(float)];
	glReadPixels(0, 0, screen_width, screen_heigth, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	
	heigth_map1 = Gdk::Pixbuf::create_from_data(data, Gdk::COLORSPACE_RGB, true, 8, screen_width, screen_heigth, screen_width * sizeof(float))->copy();
	//heigth_map1->save("/home/usuario/Prácticas_Programación/heigth_map1.png", "png");
	
	// Invertimos el orden de la malla: los elementos con la y más alta serán los primeros;
	std::vector<float> vertex_positions = mesh.float_buffer_data[0];
	int vertex_size = vertex_positions.size();
	std::vector<float> vertex_uvs = mesh.float_buffer_data[1];
	
	std::vector<float> inverse_vertex;
	std::vector<float> inverse_uvs;
	
	for (int i = vertex_size / 3; i > 0; i-=4) {
		
		for(int j = 0; j < 12; j++) {
			inverse_vertex.push_back(vertex_positions[i * 3 + j]);
		}
		
		for(int u = 0; u < 8; u++) {
			inverse_uvs.push_back(vertex_uvs[i * 2 + u]);
		}
	}
	
	// Preparamos una malla invertida
	StandardMesh inverse_mesh;
	inverse_mesh.vertex_format.addAttribute({"vPosition", GL_FLOAT, 3});
	inverse_mesh.vertex_format.addAttribute({"vTextCoords", GL_FLOAT, 2});
	
	inverse_mesh.add_float_data(inverse_vertex);
	inverse_mesh.add_float_data(inverse_uvs);
	
	inverse_mesh.add_int_data(mesh.int_buffer_data[0]);
	
	// La cargamos a los buffers
	hm_shader->load_mesh_to_buffers(inverse_mesh);
	
	// Limpiamos el framebuffer y dibujamos de nuevo
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawElements(GL_TRIANGLES, int(inverse_mesh.int_buffer_data[0].size()), GL_UNSIGNED_INT, nullptr);
	
	
	// Guardamos el segundo heigth map: (de arriba a abajo)
	glReadPixels(0, 0, screen_width, screen_heigth, GL_RGBA, GL_UNSIGNED_BYTE, data);
	heigth_map2 = Gdk::Pixbuf::create_from_data(data, Gdk::COLORSPACE_RGB, true, 8, screen_width, screen_heigth, screen_width * sizeof(float))->copy();
	//heigth_map2->save("/home/usuario/Prácticas_Programación/heigth_map2.png", "png");
	
	hm_shader->unuse_program();
	delete(data);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	hm_shader->frame_buffers[0].unmake_target();
}

// Prepare a tilemap for RayCasting
void LigthsHandler::render_tile_map()
{
	
	vec2Di tiles64 = {screen_width / 64, screen_heigth / 64};
	
	vec2Di sect64_start = {0, 0};
	
	
	tile_map = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, tiles64.x * 4, tiles64.y * 4);
	tile_map->fill(0x00000000);
	
	int color64[] = {0, 0, 255, 255};
	int color32[] = {0, 255, 0, 255};
	int color16[] = {255, 0, 0, 255};
	
	// Recorremos todas las tilas de 64x64
	
	for (int t_row = 0; t_row < tiles64.y; t_row++) {
		
		sect64_start.y = t_row * 64;
		
		for (int t_col = 0; t_col < tiles64.x; t_col++) {
			
			sect64_start.x = t_col * 64;
			
			bool tile64_to_fill = false;
			
			// Recorremos todas las tilas de 32x32 de cada tila de 64x64
			
			for (int sect32_col = 0; sect32_col < 2; sect32_col++) {
				for (int sect32_row = 0; sect32_row < 2; sect32_row++) {
					
					bool tile32_to_fill = false;
				
					vec2Di sect32_start = {sect64_start.x + sect32_col * 32, sect64_start.y + sect32_row * 32};
					
					// Recorremos todas las tilas de 16x16 de cada tila de 32x32
					
					for (int sect16_col = 0; sect16_col < 2; sect16_col++) {
						for (int sect16_row = 0; sect16_row < 2; sect16_row++) {
					 
							vec2Di sect16_start = {sect32_start.x + sect16_col * 16, sect32_start.y + sect16_row * 16};
							
							bool tile16_to_fill = false;
							
							// Si en la tila de 16x16 hallamos un pixel solido, coloreamos la tila, y marcamos la tila de 32x32 y la tila de 64x64 como opacas;
							
							for (int x = sect16_start.x; x < sect16_start.x + 16; x++) {
								for (int y = sect16_start.y; y < sect16_start.y + 16; y++) {
									
									int* pixel = get_pixel_at(x, y, heigth_map1);
									
									if (pixel) {
										if (pixel[0] != 0 && pixel[3] == 255) {
											
											int target_x = t_col * 4 + sect32_col * 2 + sect16_col;
											int target_y = t_row * 4 + sect32_row * 2 + sect16_row;
											
											draw_pixel_at(target_x, target_y, tile_map, color32);
											
											tile16_to_fill = true;
											tile32_to_fill = true;
											tile64_to_fill = true;
											
										}
										delete(pixel);
									}
									
									if (tile16_to_fill) break;
									
								}
								
								if (tile16_to_fill) break;
							}
							
							
						}
					}
					
					// Si la tila de 32x32 se ha de rellenar, buscamos las tilas de 16x16 vacías y las rellenamos
					
					if (tile32_to_fill) {
						for (int col = 0; col < 2; col++) {
							for (int row = 0; row < 2; row++) {
								
								int target_x = t_col * 4 + sect32_col * 2 + col; 
								int target_y = t_row * 4 + sect32_row * 2 + row;
								
								int* tile16 = get_pixel_at(target_x, target_y, tile_map);
								if (tile16) {
									if (tile16[3] == 0) {
										draw_pixel_at(target_x, target_y, tile_map, color16);
									delete(tile16);
									}
								}
							}
						}
					}
				}
			}
			
			// Si la tila de 64x64 se ha de rellenar, buscamos las tilas de 32x32 vacías y las rellenamos
			
			if (tile64_to_fill) {
				for (int sect32_col = 0; sect32_col < 2; sect32_col++) {
					for (int sect32_row = 0; sect32_row < 2; sect32_row++) {
						
						int target_x = t_col * 4 + sect32_col * 2;
						int target_y = t_row * 4 + sect32_row * 2;
						int *tile32 = get_pixel_at(target_x, target_y, tile_map);
						
						if (tile32) {
							if (tile32[3] == 0) {
								
								for (int i = 0; i < 2; i++) {
									for (int j = 0; j < 2; j++) {
										draw_pixel_at(target_x + i, target_y + j, tile_map, color64);
									}
								}
							}
							delete(tile32);
						}
					}
				}
			}
		}
	}
	
	tile_map->save("/home/usuario/Programacion/tests/sprite-stacking-tests/blender_slicer_saved_test/tileMaps_test_inverse.png", "png");
	
}

void LigthsHandler::load_pixbuf_to_texture(int texture_id, Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixbuf->get_width(), pixbuf->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixbuf->get_pixels());
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}



