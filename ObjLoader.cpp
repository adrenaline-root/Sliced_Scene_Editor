#include "ObjLoader.h"
#include <algorithm>

ObjModel::ObjModel()
{
}

ObjModel::~ObjModel()
{
}

bool ObjModel::startsWith(std::string& line, const char* text)
{
	size_t textlen = strlen(text);
	if (line.size() < textlen)
	{
		return false;
	}
	for (size_t i = 0; i < textlen; i++)
	{
		if (line[i] == text[i]) continue;
		else return false;
	}
	
	return true;
}

void ObjModel::loadFromFile(const char* filename)
{
	std::ifstream file(filename);
	std::cout << "revisando el archivo: " << filename << std::endl;
	
	if (file)
	{
		std::string line;
		while (getline(file, line))
		{
			std::replace(line.begin(), line.end(), '.', ',');
			if (startsWith(line, "v ")) {
				vec3D pos;
				sscanf(line.c_str(), "v %f %f %f", &pos.x, &pos.y, &pos.z);
				

				positions.push_back(pos);
			}
			
			if (startsWith(line, "vn ")) {
				vec3D norm;
				sscanf(line.c_str(), "vn %f %f %f", &norm.x, &norm.y, &norm.z);
				normals.push_back(norm);
			}
			
			if (startsWith(line, "vt ")) {
				vec2D uv;
				sscanf(line.c_str(), "vt %f %f", &uv.x, &uv.y);
				uvs.push_back(uv);
			}
			
			if (startsWith(line, "f "))
			{
				vec3Di index;
				sscanf(line.c_str(), "f %d %d %d", &index.x, &index.y, &index.z);
				indexes.push_back(index);
			}
		}
	}
	else
	{
		std::cout << "Problem loading Obj file" << std::endl; 
	}
}

std::vector<float> ObjModel::getVertexPositions()
{
	std::vector<float> vpos;
	for (vec3D v : positions) {
		vpos.push_back(v.x);
		vpos.push_back(v.y);
		vpos.push_back(v.z);
	}
	
	return vpos;
}

std::vector<float> ObjModel::getVertexNormals()
{
	std::vector<float> vpos;
	for (vec3D v : normals) {
		vpos.push_back(v.x);
		vpos.push_back(v.y);
		vpos.push_back(v.z);
	}
	
	return vpos;
}

std::vector<float> ObjModel::getVertexTextCoords()
{
	std::vector<float> vpos;
	for (vec2D v : uvs) {
		vpos.push_back(v.x);
		vpos.push_back(v.y);
	}
	
	return vpos;
}

std::vector<int> ObjModel::getIndices()
{
	std::vector<int> vpos;
	for (vec3Di v : indexes) {
		vpos.push_back(v.x - 1);
		vpos.push_back(v.y - 1);
		vpos.push_back(v.z - 1);
	}
	
	return vpos;
}
