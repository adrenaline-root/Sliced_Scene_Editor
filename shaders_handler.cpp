#include "shaders_handler.h"


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

void ShaderHandler::parse_shaders(std::string filepath)
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

unsigned int ShaderHandler::compile_shader(unsigned int type, const std::string& source)
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


void ShaderHandler::create_shaders()
{
	std::cout << "creating shaders" << std::endl;
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

void ShaderHandler::use_program()
{
	glUseProgram(program);
}

void ShaderHandler::unuse_program()
{
	glUseProgram(0);
}

void ShaderHandler::create_attributes()
{
	std::cout << "creating atribtues" << std::endl;
	vertex_format.addAttribute({"vPosition", GL_FLOAT, 3});
	vertex_format.addAttribute({"vTextCoords", GL_FLOAT, 2});
	vertex_format.addAttribute({"vRotation", GL_FLOAT, 1});
	
}


void ShaderHandler::set_int(const GLchar* uniform_name, int value)
{
	
}
void ShaderHandler::set_float(const GLchar* uniform_name, float value)
{
	
}
void ShaderHandler::set_mat4(const GLchar* uniform_name, float *value)
{
	GLint value_loc = glGetUniformLocation(program, uniform_name);
	glUniformMatrix4fv(value_loc, 1, GL_FALSE, value);
	
}

void ShaderHandler::set_sampler(const GLchar* uniform_name, int texture_unit)
{
	GLint sampler_loc = glGetUniformLocation(program, uniform_name);
	glUniform1i(sampler_loc, texture_unit);
}


void ShaderHandler::set_viewport_matrix(vec3Di values)
{
	float _viewport_matrix[] = {	2.0 / (float(values.x)), 0, 0, 0,
							0, 2.0 / (float(values.y)), 0, 0,
							0, 0, 2.0 / (float(values.z)), 0,
							0, 0, 0, 1
						};
	std::copy(std::begin(_viewport_matrix), std::end(_viewport_matrix), std::begin(viewport_matrix));
}

void ShaderHandler::set_trans_scale_matrix(float scale, vec2D translation)
{
	float _trans_scale_matrix[] = {	scale, 0, 0, scale * translation.x,
							0, scale, 0, 0,
							0, 0, scale, scale * translation.y,
							0, 0, 0, 1
						  };
	std::copy(std::begin(_trans_scale_matrix), std::end(_trans_scale_matrix), std::begin(trans_scale_matrix));
	
}

void ShaderHandler::set_rotation_matrix(int angle)
{
		
	float coseno = float(cos(double(angle) * PI / double(180)));
	float seno = float(sin(double(angle) * PI / double(180)));
	
	float _rotation_matrix[] = {	coseno, 0, seno, 0, // rotación eje y
									0, 1, 0, 0,
									-seno, 0, coseno, 0,
									0, 0, 0, 1
									};
	std::copy(std::begin(_rotation_matrix), std::end(_rotation_matrix), std::begin(rotation_matrix));

		
}

void ShaderHandler::set_view_rotation_matrix(int angle)
{
	float coseno = float(cos(double(angle) * PI / double(180)));
	float seno = float(sin(double(angle) * PI / double(180)));
			
	float _view_rotation_matrix[] = {	1, 0, 0, 0, // rotación eje x
										0, coseno, -seno, 0, 
										0, seno, coseno, 0,
										0, 0, 0, 1
										};
										
	std::copy(std::begin(_view_rotation_matrix), std::end(_view_rotation_matrix), std::begin(view_rotation_matrix));
}



void ShaderHandler::load_mesh_to_gl_buffers(LayeredMesh mesh)
{
	std::cout << "loading mesh to gl buffers" << std::endl;
	
	// Add vertex to gl buffers
	
	glGenBuffers(1, &vertex_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, mesh.vertex_pool.size() * sizeof(float), mesh.get_vertex_pool(), GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(0); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(0, vertex_format.attributes[0].size, GL_FLOAT, GL_FALSE, vertex_format.attributes[0].size_in_bytes, reinterpret_cast<const void*>(vertex_format.attributes[0].index_in_format));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	// Add uvs to gl buffers
	
	glGenBuffers(1, &uvs_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, mesh.textcoord_pool.size() * sizeof(float), mesh.get_textcoord_pool(), GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(1); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(1, vertex_format.attributes[1].size, GL_FLOAT, GL_FALSE, vertex_format.attributes[1].size_in_bytes, reinterpret_cast<const void*>(vertex_format.attributes[1].index_in_format));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	
	
	// Add rotation to gl buffers
	
	glGenBuffers(1, &rotation_buffer); // Genera un buffer (lista de slots en memoria) al que pasaremos nuestros datos, en este caso los valores de los vértices; //
	
	glBindBuffer(GL_ARRAY_BUFFER, rotation_buffer); 											// Selecciona el buffer indicado (vertexBuffer) al que se añadirán o del que se tomarán los datos; //
	glBufferData(GL_ARRAY_BUFFER, mesh.rotation_pool.size() * sizeof(float), mesh.get_rotation_pool(), GL_DYNAMIC_DRAW);// Copia los datos en el buffer que le hemos indicado; //
	
	glEnableVertexAttribArray(2); // Habilita un atributo de vértice (por ejemplo: la posición); en este caso en la posición 0; // 
	
	glVertexAttribPointer(2, vertex_format.attributes[2].size, GL_FLOAT, GL_FALSE, vertex_format.attributes[2].size_in_bytes, reinterpret_cast<const void*>(vertex_format.attributes[2].index_in_format));	// Describe la estructura del atributo en cuestión: en este caso el atributo 0 //
	

	// Add index to gl bufers

	glGenBuffers(1, &index_buffer);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);											// Selecciona el buffer indicado (indexbuffer) al que se añadiran o del que se tomarań los datos //
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_pool.size() * sizeof(unsigned int), mesh.get_index_pool(), GL_DYNAMIC_DRAW);	// Copia los datos de (indices) en el buffer binded (en este caso, indexbuffer)

	check_gl_errors();
}

void ShaderHandler::check_gl_errors()
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