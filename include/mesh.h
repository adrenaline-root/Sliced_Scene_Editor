#ifndef MESH_H
#define MESH_H

//#include "ObjLoader.h"
//#include "sliced_object.h"
#include "headers.h"


struct vec2D {double x, y;};
struct vec3D {float x, y, z;};

struct vec2Di {int x, y;};
struct vec3Di {
	int x, y, z;
	bool is_equal_to(vec3Di vec) {
		if (vec.x == x && vec.y == y && vec.z == z) return true;
		else return false;
	};
};


struct Attribute {
	Glib::ustring name;
	GLuint GL_Type;
	int size;
	int size_in_bytes = 0;
	int index_in_format = 0;
	
};

struct VertexFormat {
	std::vector<Attribute> attributes;
	int size = 0;
	int sizeInBytes = 0;
	
	void addAttribute(Attribute attr) {
		attr.index_in_format = sizeInBytes;
		
		if (attr.GL_Type == GL_FLOAT) attr.size_in_bytes = sizeof(float) * attr.size;
		if (attr.GL_Type == GL_UNSIGNED_INT) attr.size_in_bytes = sizeof(unsigned int) * attr.size;
		
		attributes.push_back(attr);
		size += attr.size;
	};
	
	int get_attribute_index(std::string attr_name) {
		int index = -1;
		for (int i = 0; i < int(attributes.size()); i++) {
			if (attributes[i].name == attr_name) {
				index = i;
				break;
			}
		}
		
		if (index == -1) {
			std::cerr << "No such attribute in VertexFormat" << std::endl;
		}
		
		return index;
	};
};


class LayeredMesh {
public:
	std::vector<float> vertex_pool, textcoord_pool, rotation_pool;
	std::vector<int> index_pool;
	
	float *vertex_pool_buff, *textcoord_pool_buff, *rotation_pool_buff;
	unsigned int *index_pool_buff;
	
	LayeredMesh();
	virtual ~LayeredMesh();
	
	float* get_vertex_pool();
	float* get_textcoord_pool();
	float* get_rotation_pool();
	unsigned int* get_index_pool();
	
	
};

// 	PRUEBA NUEW TYPE MESH

class StandardMesh {
public:
	VertexFormat vertex_format;
	
	// Aquí irán los atributos de vértice, ya sean position, uv_coords, translations etc ect;
	// El orden de los buffers va según el orden de los atributos del vertex format;
	std::vector<std::vector<float>> float_buffer_data; // Para los parámetros float;
	std::vector<std::vector<int>> int_buffer_data; // Para los parámetros int;
	
	float* gl_float_data[10];
	unsigned int* gl_int_data[10];

	StandardMesh();
	virtual ~StandardMesh();
	
	void add_float_data(std::vector<float> float_vector);
	void add_int_data(std::vector<int> int_vector);
	
};


class Model {
public:
	std::vector<float> vertexPool, textcoordPool, colorPool, normalsPool;
	std::vector<int> indexPool;
	int textureID;
	Cairo::RefPtr<Cairo::ImageSurface> textureSource;
	vec3Di position = {0, 0, 0};

	Model();
	virtual ~Model();
	void loadModel(const char* path);
	void createCube(float size);
	void move(int dx, int dy, int dz);
	
	
};



#endif