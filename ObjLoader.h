#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include "headers.h"

class ObjModel {
public:
	struct vec2D {float x, y;};
	struct vec3D {float x, y, z;};
	struct vec3Di {int x, y, z;};
	
	std::vector<vec3D> positions, normals;
	std::vector<vec2D> uvs;
	std::vector<vec3Di> indexes;
	
	ObjModel();
	virtual ~ObjModel();
	bool startsWith(std::string& line, const char* text);
	void loadFromFile(const char* filename);
	std::vector<float> getVertexPositions();
	std::vector<float> getVertexNormals();
	std::vector<float> getVertexTextCoords();
	std::vector<int> getIndices();
};

#endif