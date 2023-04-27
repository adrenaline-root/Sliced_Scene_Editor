---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;

uniform mat4 viewport_matrix;
uniform mat4 view_rotation_matrix;
uniform mat4 translation_matrix;

uniform vec3 ligth_pos;

out vec4 Frag_pos;
out vec2 out_uvs;

void main()
{ 
	gl_Position = translation_matrix * vPosition * view_rotation_matrix * viewport_matrix;
	Frag_pos = vPosition;
	out_uvs = vTextCoords;
}


---fragment shader

#version 130

in vec2 out_uvs;
in vec4 Frag_pos;

uniform sampler2D c_texture;
out vec4 Frag_Color;

void main()
{
	vec4 texture_color = texture(c_texture, out_uvs);
	if (texture_color.w != 1.0) discard;
	else {
		if (Frag_pos.y != 0.0) Frag_Color = vec4(vec3(Frag_pos.y / 255.0), texture_color.w);
		else discard;
	}
}