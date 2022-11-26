---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;
in float vRotation;

uniform mat4 ortomatrix;
uniform mat4 rot_mat;
uniform mat4 rot_matb;
uniform mat4 trans_mat;

out vec4 frag_pos;
out vec2 out_uvs;

void main()
{ 
	frag_pos = vPosition * trans_mat * rot_mat * rot_matb * ortomatrix;
	gl_Position = vPosition * trans_mat * rot_mat * rot_matb * ortomatrix;
	out_uvs = vTextCoords;
}


---fragment shader

#version 130

in vec4 frag_pos;
in vec2 out_uvs;

out vec4 FragColor;

uniform sampler2D c_texture;

void main()
{
	float back_limit = -1024.0;
	float front_limit = 1024.0;
	
	float z_ratio = 0.5 + (frag_pos.z * 0.2) ;
	
	vec4 texture_color = texture(c_texture, out_uvs);
	FragColor = vec4(vec3(z_ratio), texture_color.w);
}