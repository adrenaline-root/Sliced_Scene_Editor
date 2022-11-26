#include "mesh.h"
#include "ObjLoader.h"

//================================
//=== METODOS DE LAYERED MESH ====
//================================


LayeredMesh::LayeredMesh()
{
	
}


LayeredMesh::~LayeredMesh()
{
	
}


float* LayeredMesh::get_vertex_pool()
{
	vertex_pool_buff = new float[vertex_pool.size()];
	std::copy(vertex_pool.begin(), vertex_pool.end(), vertex_pool_buff);
	return vertex_pool_buff;
}

float* LayeredMesh::get_textcoord_pool()
{
	textcoord_pool_buff = new float[textcoord_pool.size()];
	std::copy(textcoord_pool.begin(), textcoord_pool.end(), textcoord_pool_buff);
	return textcoord_pool_buff;
}


unsigned int* LayeredMesh::get_index_pool()
{
	index_pool_buff = new unsigned int[index_pool.size()];
	std::copy(std::begin(index_pool), std::end(index_pool), index_pool_buff);
	return index_pool_buff;
}

float* LayeredMesh::get_rotation_pool()
{
	rotation_pool_buff = new float[rotation_pool.size()];
	std::copy(std::begin(rotation_pool), std::end(rotation_pool), rotation_pool_buff);
	return rotation_pool_buff;
}


//================================
//=== METODOS DE MODEL ====
//================================



Model::Model()
{
}

Model::~Model() 
{
}

void Model::loadModel(const char* path)
{
	ObjModel obj = ObjModel();
	obj.loadFromFile(path);
	
	vertexPool = obj.getVertexPositions();
	normalsPool = obj.getVertexNormals();
	textcoordPool = obj.getVertexTextCoords();
	indexPool = obj.getIndices();
}

void Model::createCube(float size)
{
	vertexPool = std::vector<float> {	0.0f, 0.0f, 0.0f, 
										size, 0.0f, 0.0f, 
										size, 0.0f, size, 
										0.0f, 0.0f, size,
										
										0.0f, size, 0.0f,
										size, size, 0.0f,
										size, size, size,
										0.0f, size, size};
										
//	std::cout << "";
//	copy(vertexPool.begin(), vertexPool.end(), std::ostream_iterator<float>(std::cout, " "));

	indexPool = std::vector<int> {	0, 1, 1, 2, 2, 3, 3, 0, 
									0, 4, 1, 5, 2, 6, 3, 7,
									4, 5, 5, 6, 6, 7, 7, 4};
}

void Model::move(int dx, int dy, int dz)
{
	
	position.x += dx;
	position.y += dz; // La z y la y están invertidas en opengl
	position.z += dy;
	
	for (int x = 0; x < int(vertexPool.size()); x++) {
		if (x % 3 == 0) {
			vertexPool[x] = vertexPool[x] + float(dx);
		}
		if (x % 3 == 1) {
			vertexPool[x] = vertexPool[x] + float(dy);
		}
		if (x % 3 == 2) {
			vertexPool[x] = vertexPool[x] + float(dz);
		}
	}
}