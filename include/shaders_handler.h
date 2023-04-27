#ifndef SHADERS_HANDLER_H
#define SHADERS_HANDLER_H

#include "headers.h"
#include "mesh.h"


#define PI 3.14159265358979323846

enum uniform_types {GLSL_INT, GLSL_FLOAT, GLSL_MAT4, GLSL_SAMPLER_2D};

enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

class FrameBuffer {
public:
	unsigned int _id;
	std::vector<unsigned int> attachments_ids;
	
	FrameBuffer();
	virtual ~FrameBuffer();
	
	void set_framebuffer(int width, int heigth, int attachments);
	void prepare_target_textures(int width, int heigth);
	void make_target();
	void unmake_target();

};

class Shader {
public:
	
	std::string vertex_shader_source, fragment_shader_source, name;
	unsigned int program;
	VertexFormat vertex_format;
	std::vector<unsigned int> vertex_buffers;
	std::vector<FrameBuffer> frame_buffers;
	
	Shader();
	virtual ~Shader();
	
	void parse_shaders(std::string filepath);
	unsigned int compile_shader(unsigned int type, const std::string& source);
	void create_shaders();
	void use_program();
	void unuse_program();
	
	void set_int(const GLchar* uniform_name, int value);
	void set_float(const GLchar* uniform_name, float value);
	void set_vec(const GLchar* uniform_name, float *value, int count);
	void set_glmmat4(const GLchar* uniform_name, glm::mat4 matrix);
	void set_sampler(const GLchar* uniform_name, int texture_unit);
		
	void load_mesh_to_buffers(StandardMesh mesh);
	
	void check_gl_errors();
};
	

class ShaderHandler {
public:
	std::string vertex_shader_source, fragment_shader_source;
	unsigned int program;
	
	std::vector<Shader> shaders;

	ShaderHandler();
	virtual ~ShaderHandler();
	
	void get_versions();

	void add_shader(Shader shader);
	Shader get_shader(std::string name);
	Shader* get_shader_ptr(std::string name);
	
};



#endif