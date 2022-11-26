#ifndef SHADERS_HANDLER_H
#define SHADERS_HANDLER_H

#include "headers.h"
#include "mesh.h"


#define PI 3.14159265358979323846

enum uniform_types {GLSL_INT, GLSL_FLOAT, GLSL_MAT4, GLSL_SAMPLER_2D};

enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
	

class ShaderHandler {
public:
	std::string vertex_shader_source, fragment_shader_source;
	unsigned int program, vertex_buffer, uvs_buffer, rotation_buffer, index_buffer;
	VertexFormat vertex_format;
	
	float viewport_matrix[16];
	float trans_scale_matrix[16];
	float rotation_matrix[16];
	float view_rotation_matrix[16];

	
	ShaderHandler();
	virtual ~ShaderHandler();
	
	void get_versions();
	void parse_shaders(std::string filepath);
	unsigned int compile_shader(unsigned int type, const std::string& source);
	void create_shaders();
	void use_program();
	void unuse_program();
	
	void create_attributes();
	
	void set_int(const GLchar* uniform_name, int value);
	void set_float(const GLchar* uniform_name, float value);
	void set_mat4(const GLchar* uniform_name, float *value);
	void set_sampler(const GLchar* uniform_name, int texture_unit);
	
	void set_viewport_matrix(vec3Di values);
	void set_trans_scale_matrix(float scale, vec2D translation);
	void set_rotation_matrix(int angle);
	void set_view_rotation_matrix(int angle);
	
	void load_mesh_to_gl_buffers(LayeredMesh mesh);
	
	void check_gl_errors();
	
	
};



#endif