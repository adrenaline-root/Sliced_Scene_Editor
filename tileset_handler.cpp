#include "tileset_handler.h"
#include <regex>
#include <cstdlib>
#include "stdlib.h"
#include <iostream>

// ================================
// ======= Tile Set Methods =======
// ================================

tileset::tileset()
{
	
}

tileset::~tileset() 
{
	
}


Glib::RefPtr<Gdk::Pixbuf> tileset::get_element_image(int element_index)
{
	auto slice_object = sliced_objects[element_index];
	int z_init = slice_object.slices[0].geometry[0].z;
	int margin = 2;
	int x_region = element_index * (elements_dimensions.x + margin);
	
	Cairo::RefPtr<Cairo::ImageSurface> target_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, elements_dimensions.x, elements_dimensions.y + int(dimensions.z));
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(target_surface);
	
	cr->set_source_rgba(0.0, 0.0, 0.0, 0.0);
	cr->paint();
	
	for (int z = 0; z < int(dimensions.z); z++) {
		int y_region = z * (elements_dimensions.y + margin);
		
		if (y_region + elements_dimensions.y <= color_texture->get_height()) {
			
			Glib::RefPtr<Gdk::Pixbuf> layer = Gdk::Pixbuf::create_subpixbuf(color_texture, x_region + 1, y_region + 1, elements_dimensions.x, elements_dimensions.y);
			
			cr->save();
			cr->translate(0, target_surface->get_height() - z_init - z - elements_dimensions.y);
			Gdk::Cairo::set_source_pixbuf(cr, layer, 0, 0);
			cr->paint();
			cr->restore();
		}
	}
	
	return Gdk::Pixbuf::create(target_surface, 0, 0, target_surface->get_width(), target_surface->get_height());

}

Sliced_Object tileset::get_selected_object()
{
	return sliced_objects[selected_element];
}


Sliced_Object tileset::get_object_by_name(std::string name) 
{
	for (Sliced_Object sobj : sliced_objects) {
		if (sobj.name == name) return sobj;
	}
}


// ================================
// ===== TLS Handler Methods ======
// ================================

tlsHandler::tlsHandler()
{
	
}

tlsHandler::~tlsHandler()
{
	
}

bool tlsHandler::load_tile_from_file(Glib::ustring filename)
{
	//std::cout << "loading from file " << filename << std::endl;
	int filename_size = filename.size();
	std::string extension = filename.substr(filename_size - 4);
	
	//std::cout << "File Extension: " << extension << std::endl;
	
	if (extension == ".tls") {
	
		std::ifstream file(filename);
		vec2Di ele_dimen;
		bool is_not_in = true;
		
		
		if (file) {
			tileset tile_set;
			tile_set.path = filename;
			std::string line;
			while(getline(file, line) && is_not_in) {
				if (line.size() != 0) {
					
					std::string prefijo = line.substr(0, 4);
					std::string info = line.substr(4);
					
					if (prefijo == "n-- ") {
						for (auto tile : tile_set_list) {
							if (tile.name == info) {
								is_not_in = false;
								break;
							}
						}
						tile_set.name = info;
					}
					
					if (prefijo == "d-- ") {
						sscanf(info.c_str(), "%d %d %d",  &tile_set.dimensions.x, &tile_set.dimensions.y, &tile_set.dimensions.z);
						//std::printf("Las dimensiones del tileset son: %d, %d, %d\n", tile_set.dimensions.x, tile_set.dimensions.y, tile_set.dimensions.z);
					}
					
					if (prefijo == "ne- ") {
						sscanf(info.c_str(), "%d",  &tile_set.number_of_elements);
						
						for (int n = 0; n < tile_set.number_of_elements; n++) {
							Sliced_Object s_object;
							s_object.name = tile_set.name + "_" + std::to_string(n);
							tile_set.sliced_objects.push_back(s_object);
						}
						
						//std::printf("El numero de elementos del tileset es: %d\n", int(tile_set.sliced_objects.size()));
					}
					
					if (prefijo == "ed- ") {
						sscanf(info.c_str(), "%d %d", &ele_dimen.x, &ele_dimen.y);
						tile_set.elements_dimensions = ele_dimen;
						
						//std::printf("Las dimensiones de cada elemento son: %d, %d\n", ele_dimen.x, ele_dimen.y);
						
					}
					
					if (prefijo == "e-- ") {
						
						int e_index;
						int indice;
						vec2D uv1, uv2, uv3, uv4;
						
						std::replace(info.begin(), info.end(), '.', ',');
						
						sscanf(info.c_str(), "%d %d %lf %lf %lf %lf %lf %lf %lf %lf ", &e_index, &indice, &uv1.x, &uv1.y, &uv2.x, &uv2.y, &uv3.x, &uv3.y, &uv4.x, &uv4.y);
						
					
						slice layer;
						layer.set_geometry(ele_dimen, float(indice));
						layer.set_uvs({uv1, uv2, uv3, uv4});
						
						tile_set.sliced_objects[e_index].add_slice(layer);
					}
				}
			}
			
			if (is_not_in) {
				std::string path = filename;
				path = path.substr(0, path.size() - 4);
				
				std::string color_map_path = path + "_color_map.png";
				std::string normal_map_path = path + "_normal_map.png";
				
				try {
					tile_set.color_texture = Gdk::Pixbuf::create_from_file(color_map_path);
					tile_set.normal_texture = Gdk::Pixbuf::create_from_file(normal_map_path);
				}
				
				catch (const Glib::Error &e) {
					std::cerr << "Error loading textures: " << e.what() << std::endl;
				}
				
				tile_set_list.push_back(tile_set);
			}
		}

		return is_not_in;

	}
	
	else {
		std::cerr << "La extensión de archivo es incorrecta" << std::endl;
		return false;
	}
	
	
}

void tlsHandler::select_tile(int index)
{
	selected_tile = index;
}

tileset tlsHandler::get_selected_tile()
{
	return tile_set_list[selected_tile];
}

tileset* tlsHandler::get_selected_tile_pntr()
{
	return &tile_set_list[selected_tile];
}