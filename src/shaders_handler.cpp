#include "../include/shaders_handler.h"

#define PI 3.14159265358979323846

// FrameBuffer Methods 

FrameBuffer::FrameBuffer() 
{
	
}

FrameBuffer::~FrameBuffer() 
{
	
}

void FrameBuffer::set_framebuffer(int width, int heigth, int attachments)
{
	glGenFramebuffers(1, &_id);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glActiveTexture(GL_TEXTURE0);
	
	
	for (int i = 0; i < attachments; i++) {
		unsigned int attachment;
		attachments_ids.push_back(attachment);
		glGenTextures(1, &attachments_ids[i]);
		glBindTexture(GL_TEXTURE_2D, attachments_ids[i]);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB12, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments_ids[i], 0);
	}
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void FrameBuffer::prepare_target_textures(int width, int heigth) 
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glActiveTexture(GL_TEXTURE0);
	
	for (int i = 0; i < int(attachments_ids.size()); i++) {
		glBindTexture(GL_TEXTURE_2D, attachments_ids[i]);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB12, width, heigth, 0, GL_RGB, GL_FLOAT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments_ids[i], 0);
	}
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "============== Framebuffer is not ready!!! ===================" << std::endl; 
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::make_target()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void FrameBuffer::unmake_target()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Shader Methods

Shader::Shader()
{
	
}

Shader::~Shader()
{
	
}

void Shader::parse_shaders(std::string filepath)
{
	std::ifstream file(filepath);
	std::stringstream ss[2];
	
	
	ShaderType sType = ShaderType::NONE;
	
	std::string line;
	while (getline(file, line))
	{
		if (line.find("shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos) { sType = ShaderType::VERTEX; }
			else if (line.find("fragment") != std::string::npos) { sType = ShaderType::FRAGMENT; }
		}
		else { ss[int(sType)] << line << "\n"; }
	}
	
	vertex_shader_source = ss[0].str();
	fragment_shader_source = ss[1].str();
	
//	std::cout << vertex_shader_source << std::endl;
//	std::cout << fragment_shader_source << std::endl;

}

unsigned int Shader::compile_shader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	
	// Comprobar errores de sintaxis en los shaders //
	
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	
	if (!result) 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		
		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
		std::cout << message << std::endl;
		
		glDeleteShader(id);
		return 0;
	}
	
	return id;
}


void Shader::create_shaders()
{
	std::cout << "creating shader: " << name <<  std::endl;
	program = glCreateProgram();
	unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
	unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
	
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	//return program;
}

void Shader::use_program()
{
	glUseProgram(program);
}

void Shader::unuse_program()
{
	glUseProgram(0);
}

void Shader::set_int(const GLchar* uniform_name, int value)
{
	GLint value_loc = glGetUniformLocation(program, uniform_name);
	glUniform1i(value_loc, value);
}

void Shader::set_float(const GLchar* uniform_name, float value)
{
	GLint value_loc = glGetUniformLocation(program, uniform_name);
	glUniform1f(value_loc, value);
}


void Shader::set_vec(const GLchar* uniform_name,  float *value, int count)
{
	GLint value_loc = glGetUniformLocation(program, uniform_name);
	if (count == 2) glUniform2fv(value_loc, 1, value);
	else if (count == 3) glUniform3fv(value_loc, 1, value);
	
}


void Shader::set_glmmat4(const GLchar* uniform_name, glm::mat4 value)
{
	GLint value_loc = glGetUniformLocation(program, uniform_name);
	glUniformMatrix4fv(value_loc, 1, GL_FALSE, glm::value_ptr(value));
	
}

void Shader::set_sampler(const GLchar* uniform_name, int texture_unit)
{
	GLint sampler_loc = glGetUniformLocation(program, uniform_name);
	glUniform1i(sampler_loc, texture_unit);
}


void Shader::load_mesh_to_buffers(StandardMesh mesh)
{
	int at_size = int(mesh.vertex_format.attributes.size());
	
	// Mandamos los atributos de vértice a la memoria
	if (vertex_buffers.size() == 0) {
		
		for(int i = 0; i < at_size; i++) {
			unsigned int buffer;
			glGenBuffers(1, &buffer);
			vertex_buffers.push_back(buffer);
			
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[i]);
			glBufferData(GL_ARRAY_BUFFER, mesh.float_buffer_data[i].size() * sizeof(float), mesh.gl_float_data[i], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, mesh.vertex_format.attributes[i].size, mesh.vertex_format.attributes[i].GL_Type, GL_FALSE, mesh.vertex_format.attributes[i].size_in_bytes, reinterpret_cast<const void*>(mesh.vertex_format.attributes[i].index_in_format));

		}
		
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		vertex_buffers.push_back(buffer);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffers[at_size]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.int_buffer_data[0].size() * sizeof(unsigned int), mesh.gl_int_data[0], GL_DYNAMIC_DRAW);

		
		
	}
	
	else {
		
		for (int i = 0; i < at_size; i++) {
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[i]);
			glBufferData(GL_ARRAY_BUFFER, mesh.float_buffer_data[i].size() * sizeof(float), mesh.gl_float_data[i], GL_DYNAMIC_DRAW);
			//glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.float_buffer_data[i].size() * sizeof(float), mesh.gl_float_data[i]);
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, mesh.vertex_format.attributes[i].size, mesh.vertex_format.attributes[i].GL_Type, GL_FALSE, mesh.vertex_format.attributes[i].size_in_bytes, reinterpret_cast<const void*>(mesh.vertex_format.attributes[i].index_in_format));
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffers[at_size]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.int_buffer_data[0].size() * sizeof(unsigned int), mesh.gl_int_data[0], GL_DYNAMIC_DRAW);
		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh.int_buffer_data[0].size() * sizeof(unsigned int), mesh.gl_int_data[0]);
		
	}
	
	check_gl_errors();
	
}

void Shader::check_gl_errors()
{
	if (GLenum error = glGetError())
	{
		std::string stError;
		char ErrorNumber[20];
		sprintf(ErrorNumber, "%X", error);
		
		
		switch (atoi(ErrorNumber)) 
		{
			case 500:
				stError = "GL_INVALID_ENUM";
				break;
			case 501:
				stError = "GL_INVALID_VALUE";
				break;
			case 502:
				stError = "GL_INVALID_OPERATION";
				break;
			case 503:
				stError = "GL_STACK_OVERFLOW";
				break;
			case 504:
				stError = "GL_STACK_UNDERFLOW";
				break;
			case 505:
				stError = "GL_OUT_OF_MEMORY";
				break;
		}
		
		std::cout << "An OpenGl error has occurred: (" << stError << ") " << std::endl; 
	}
}




// ShaderHandler Methods


ShaderHandler::ShaderHandler()
{

}

ShaderHandler::~ShaderHandler()
{
	
}

void ShaderHandler::get_versions()
{
	std::printf("Vendor graphic card: %s\n", glGetString(GL_VENDOR));
	std::printf("Renderer: %s\n", glGetString(GL_RENDERER));
	std::printf("Version GL: %s\n", glGetString(GL_VERSION));
	std::printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void ShaderHandler::add_shader(Shader shader)
{
	shaders.push_back(shader);
}

Shader ShaderHandler::get_shader(std::string name)
{
	Shader _shader;
	
	for (Shader shader : shaders) {
		if (shader.name == name) {
			_shader = shader;
			break;
		}
	}
	
	return _shader;

}

Shader* ShaderHandler::get_shader_ptr(std::string name) 
{
	Shader *_shader = NULL;
	
	for (auto &shader : shaders) {
		if (shader.name == name) {
			_shader = &shader;
			break;
		}
	}
	
	return _shader;
}