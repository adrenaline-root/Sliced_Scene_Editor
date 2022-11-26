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