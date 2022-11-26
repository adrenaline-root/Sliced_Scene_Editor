#ifndef SLICED_OBJECT_H
#define SLICED_OBJECT_H

#include "mesh.h"

enum ActionType {ROTATE, MOVE, VOLTEAR, ADD, DELETE, NO_ACTION};

struct slice {
	std::vector<vec3D> geometry;
	std::vector<vec2D> uvs;
	std::vector<int> indices{0, 1, 2, 2, 3, 0};
	vec2Di dimensiones;
	
	void set_geometry(vec2Di dimensions, int z) {
		
		dimensiones = dimensions;
		
		float x_ratio = float(dimensions.x) / 2.0;
		float y_ratio = float(dimensions.y) / 2.0;
		
		geometry = {
			
			{-x_ratio, -y_ratio, float(z)},
			{x_ratio, -y_ratio, float(z)},
			{x_ratio, y_ratio, float(z)},
			{-x_ratio, y_ratio, float(z)}
			
		};
	};
	
	void set_uvs(std::vector<vec2D> _uvs) {
		uvs = _uvs;
	};
	
	std::vector<vec3D> rotar(int rotation) {
	
		
		if (rotation == 0) {
			return geometry;
		} 
		else if (rotation == 1) {
			return {geometry[1], geometry[2], geometry[3], geometry[0]};
		}
		else if (rotation == 2) {
			return {geometry[2], geometry[3], geometry[0], geometry[1]};
		}
		
		else if (rotation == 3) {
			return {geometry[3], geometry[0], geometry[1], geometry[2]};
		}
		
	};
	
	void print_() {
		for (auto v : geometry) {
			std::printf("Vertice (%f, %f, %f)\n", v.x, v.y, v.z);
		}
	};
	
	void print_uvs() {
		for (auto v : uvs) {
			std::printf("Vertice (%f, %f)\n", v.x, v.y);
		}
	};
};

struct instance {
	vec3Di position = {0, 0, 0};
	int rotation = 0;
	int index_in_mesh = 0;
	bool upside_down = false;
	bool is_equal_to(instance inst)
	{
		if (position.x == inst.position.x && position.y == inst.position.y && position.z == inst.position.z && rotation == inst.rotation) return true;
		else return false;
	};
};

class Sliced_Object {
public:
	LayeredMesh *mesh_ref;
	std::string name;
	std::vector<slice> slices;
	std::vector<int> indices;
	std::vector<instance> instancias;
	vec3D uv_transformation = {0.0, 1.0, 1.0}; // x_increment, scalar_x, scalar_y;
	
	Sliced_Object();
	virtual ~Sliced_Object();
	void add_slice(slice capa);
	void add_to_layered_mesh();
	slice* get_slice(int z);
	void print_object();
	
};


#endif