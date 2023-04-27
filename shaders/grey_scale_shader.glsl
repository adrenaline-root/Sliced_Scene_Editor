---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;

uniform mat4 viewport_matrix;
uniform mat4 rotation_matrix;
uniform mat4 view_rotation_matrix;
uniform mat4 trans_scale_matrix;

uniform vec3 ligth_pos;

out vec4 Frag_pos;
out vec4 ligthPos;
out vec2 out_uvs;

void main()
{ 
	gl_Position = trans_scale_matrix * vPosition * rotation_matrix * view_rotation_matrix * viewport_matrix;
	Frag_pos = vPosition;
	out_uvs = vTextCoords;
	ligthPos = trans_scale_matrix * vec4(ligth_pos, 1.0) * rotation_matrix * view_rotation_matrix * viewport_matrix;
}


---fragment shader

#version 130

in vec2 out_uvs;
in vec4 Frag_pos;
in vec4 ligthPos;


uniform sampler2D c_texture;
uniform vec3 ligth_pos;
uniform vec2 viewport_size;

uniform mat4 inverse_view_rotation_matrix;
uniform mat4 inverse_rotation_matrix;
uniform mat4 inverse_viewport_matrix;


// Fragment coords at grey_scale_map
//vec4 frag_greyspace_coords = view_rotation_matrix * vec4(vec3(gl_FragCoord.xyz / gl_FragCoord.w), 1.0) * gl_FragColor.w;

// Ligth pos at grey_scale_map
//vec4 ligth_greyspace_coords = ligthPos; 

//vec3 frag_space = vec3(gl_FragCoord.x * viewport_size.x, gl_FragCoord.y * viewport_size.y, gl_FragCoord.z);
//vec4 frag_space_pos = vec4(frag_space, 1.0) * inverse_view_rotation_matrix;

out vec4 Frag_Color;

vec4 ndc = vec4(
	(gl_FragCoord.x / viewport_size.x - 0.5) * 2.0,
	(gl_FragCoord.y / viewport_size.y - 0.5) * 2.0,
	(gl_FragCoord.z - 0.5) * 2.0,
	1.0);

vec4 frag_space_pos = inverse_viewport_matrix * ndc * inverse_view_rotation_matrix * inverse_rotation_matrix;
	


void main()
{
	vec3 ligth_dir = normalize(ligth_pos - Frag_pos.xyz);
	vec4 texture_color = texture(c_texture, out_uvs);

	Frag_Color = vec4(vec3(Frag_pos.y / 255.0), texture_color.w);
	//Frag_Color = vec4(vec3(Frag_pos.y / 255.0), 1.0);
	//if (Frag_pos.y == 0.0) discard;
	//Frag_Color = vec4((ligth_dir + 1.0) / 2.0, texture_color.w);

}