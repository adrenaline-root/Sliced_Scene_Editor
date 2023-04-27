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
out float rot;
out vec4 Frag_pos;

void main()
{ 
	gl_Position = trans_scale_matrix * vPosition * rotation_matrix * view_rotation_matrix * viewport_matrix;
	out_uvs = vTextCoords;
	rot = vRotation;
	Frag_pos = vPosition;
}


---fragment shader

#version 130

in vec2 out_uvs;
in float rot;
in vec4 Frag_pos;

uniform sampler2D n_texture;
uniform sampler2D c_texture;

uniform mat4 mat_rot90;
uniform mat4 mat_rot180;
uniform mat4 mat_rot270;

out vec4 Frag_Color;

void main()
{
	vec3 spotligth_pos = vec3(0, 128, 0);
	vec3 light_dir = normalize(vec3(128, 128, 128) - vec3(0, 0, 0));
	vec3 light_dir1 = normalize(spotligth_pos - vec3(Frag_pos.xyz));
	vec3 ligth_color = vec3(.7, .7, .7);
	
	float distance = length(light_dir);
	float attenuation = 1.0 / (1.0 + 0.022 * distance + 0.0019 * (distance * distance));
	
	
	
	vec3 spotligth_dir = normalize(spotligth_pos) - normalize(vec3(64, -128, 64));
	
	float theta = dot(light_dir1, spotligth_dir);
	float inner_cutoff = 1.1;
	float outer_cutoff = .9;
	float epsilon =  inner_cutoff - outer_cutoff;
	float intensity = clamp((theta - outer_cutoff) / epsilon, 0.0, 1.0);
	
	
	vec4 normal_color = texture(n_texture, out_uvs);
	vec4 texture_color = texture(c_texture, out_uvs);
	
	normal_color.x = (0.5 - normal_color.x) * (-2);
	normal_color.y = (0.5 - normal_color.y) * (-2);
	normal_color.z = (0.5 - normal_color.z) * (-2); 
	
	if (rot == 1) {
		normal_color =  normalize(vec4(normal_color.xzyw)) * mat_rot270 ;
	}
	else if (rot == 2) {
		normal_color =  normalize(vec4(normal_color.xzyw)) * mat_rot180 ;
	}
	
	else if (rot == 3) {
		normal_color = normalize(vec4(normal_color.xzyw)) * mat_rot90 ;
	}
	
	else {
		normal_color = normalize(normal_color.xzyw);
	}
	
	float diff = max(dot(normal_color.xyz, light_dir1), 0.0);
	vec3 ambient = vec3(1., .4, .4) * .9;
	vec3 specular = vec3(.3);
	//intensity = intensity * .9;
	
	vec3 diffuse;
	
	//diffuse = ((ambient * attenuation * intensity + diff * attenuation * intensity + specular * attenuation * intensity) * (texture_color.xyz * .9) );
	//diffuse = (ambient * diff) + (texture_color.xyz);
	diffuse = (texture_color.xyz);
	
	Frag_Color = vec4(diffuse, texture_color.w);
	//if (Frag_pos.y < 1.0) discard;
	
	
	
	
}