#include "../include/sliced_object.h"


Sliced_Object::Sliced_Object()
{
	
}

Sliced_Object::~Sliced_Object() {
	
}

void Sliced_Object::add_slice(slice capa)
{
	int count_indices = slices.size() * capa.geometry.size();
	
	slices.push_back(capa);
	
	for (auto c : capa.indices) {
		indices.push_back(c + count_indices);
	}
}


void Sliced_Object::add_to_layered_mesh()
{
	for (slice lamina : slices) {
		for (instance instancia : instancias) {
			for (vec3D v : lamina.geometry) {
				mesh_ref->vertex_pool.push_back(v.x * float(instancia.position.x) + float(lamina.dimensiones.x) / 2.0);
				mesh_ref->vertex_pool.push_back(v.z * float(instancia.position.z));
				mesh_ref->vertex_pool.push_back(v.y * float(instancia.position.y) + float(lamina.dimensiones.y) / 2.0);
				
			}
		}
	}
}

slice* Sliced_Object::get_slice(int z) 
{
	for (auto &sl : slices) {
		auto it = &sl - &slices[0];
		
		if (sl.geometry[0].z == z) return &slices[it];
	}
	
	return NULL; 
}

void Sliced_Object::print_object()
{
	int count = 0;
	for (auto s : slices) {
		std::cout << "Capa " << count << std::endl;
		s.print_();
		count++;
	}
}