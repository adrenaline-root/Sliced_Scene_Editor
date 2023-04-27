---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;
in float vRotation;

uniform mat4 viewport_matrix;
uniform mat4 rotation_matrix;
uniform mat4 view_rotation_matrix;
uniform mat4 trans_scale_matrix;

out vec2 out_uvs;
out vec4 Frag_pos;
out float rotation;

void main()
{ 
	gl_Position = trans_scale_matrix * vPosition * rotation_matrix * view_rotation_matrix * viewport_matrix;
	out_uvs = vTextCoords;
	Frag_pos = vPosition;
	rotation = vRotation;
}


---fragment shader

#version 130

in vec2 out_uvs;
in vec4 Frag_pos;
in float rotation;

uniform mat4 rot_90;
uniform mat4 rot_180;
uniform mat4 rot_270;

uniform mat4 trans_scale_matrix;

uniform vec2 viewport;


uniform sampler2D c_texture;
uniform sampler2D n_texture;

out vec4 frag_color;
out vec4 normals_color;
out vec4 positions_color;


void main()
{
	
	vec4 texture_color = texture(c_texture, out_uvs);
	vec4 normal_color = texture(n_texture, out_uvs);
	
	normal_color.x = (0.5 - normal_color.x) * (-2);
	normal_color.y = (0.5 - normal_color.y) * (-2);
	normal_color.z = (0.5 - normal_color.z) * (-2); 
	
	// Transformamos las normales x, y, z (con z para la altura) a formato x, y, z (con y para la altura);
	// y procedemos a su rotatición
	
	if (rotation == 1) {
		normal_color =  vec4(normal_color.xzyw) * rot_90 ;
	}
	else if (rotation == 2) {
		normal_color =  vec4(normal_color.xzyw) * rot_180 ;
	}
	
	else if (rotation == 3) {
		normal_color = vec4(normal_color.xzyw) * rot_270 ;
	}
	
	else {
		normal_color = normal_color.xzyw;
	}
	
	
	normal_color.x = (normal_color.x + 1.) / 2.0;
	normal_color.y = (normal_color.y + 1.) / 2.0;
	normal_color.z = (normal_color.z + 1.) / 2.0;
	
	float ratio = 1. / 1024.; 
	vec4 frag_spacet = vec4(gl_FragCoord.xyz, 1.0) - trans_scale_matrix[3];
	vec3 n_frag_space = vec3((frag_spacet.x * 2.) - viewport.x / 2., (frag_spacet.y * 2.) - viewport.y / 2., (frag_spacet.z + 512.) * -2.);
	vec3 frag_space = vec3(n_frag_space.x / viewport.x, n_frag_space.y / viewport.y, n_frag_space.z / 1024.);
	//frag_space = (frag_space - .5) * 2.;
	
	frag_color = texture_color;
	normals_color = vec4(normal_color.xyz, texture_color.w);
	positions_color = vec4(Frag_pos.xyz * ratio, texture_color.w);
	//positions_color = vec4(vec3(frag_space.x, Frag_pos.yz * ratio), texture_color.w);
}