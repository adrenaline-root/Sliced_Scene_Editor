---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;

out vec2 out_uvs;

void main()
{ 
	gl_Position = vPosition;
	out_uvs = vTextCoords;
}


---fragment shader

#version 130

in vec2 out_uvs;

uniform mat4 translation_mat;
uniform mat4 rotation_mat;

uniform vec3 ligth_pos;
uniform vec2 viewport;

uniform sampler2D color_texture;
uniform sampler2D normals_texture;
uniform sampler2D positions_texture;
uniform sampler2D shadow_map;

out vec4 frag_color;


void main()
{
	vec4 texture_color = texture(color_texture, out_uvs);
	vec4 normal_color = texture(normals_texture, out_uvs);
	vec4 fragment_position = texture(positions_texture, out_uvs);
	
	//fragment_position = vec4(gl_FragCoord.x - (viewport.x / 2.), fragment_position.y * 1024., (viewport.y / 2.) - gl_FragCoord.y, 1.0);
	
	fragment_position = vec4(fragment_position.xyz * 1024., 1.0);
	
	vec4 shadow_color = texture(shadow_map, out_uvs);
	
	vec4 transformed_position;
	if (texture_color.w != 0.0) {
		transformed_position = translation_mat * fragment_position * rotation_mat;
		
		normal_color.x = (0.5 - normal_color.x) * (-2);
		normal_color.y = (0.5 - normal_color.y) * (-2);
		normal_color.z = (0.5 - normal_color.z) * (-2);
	}
	
	
	vec3 light_dir = vec3(1., 0., 0.0); 
	vec3 v_ray = normalize(transformed_position.xyz);
	
	float theta = dot(normalize(light_dir), v_ray);
	
	float ratio = 1. / 1024.; 
	vec3 global_v_ray = normalize(ligth_pos) - normalize(fragment_position.xyz);
	
	float diff = dot(normalize(normal_color.xyz), normalize(global_v_ray));
	
	float inner_cutoff = cos(radians(14.5f));
	float outer_cutoff = cos(radians(15.5f));
	float epsilon =  inner_cutoff - outer_cutoff;
	float intensity = clamp((theta - outer_cutoff) / epsilon, 0.0, 1.0) + .5;
	//intensity = .9;
	
	vec3 ambient = vec3(1., .4, .4) * .9;
	vec3 specular = vec3(.3);
	
	float distance = length(normalize(global_v_ray));
	float attenuation = 1.0 / (1.0 + .7 * distance + 1.4 * (distance * distance));

	vec3 diffuse;
	diffuse = (ambient * attenuation * intensity + diff * attenuation * intensity + specular * attenuation * intensity);
	
	float screen_size = 256. * 2.;
	float screen_half = screen_size / 2.;
	float texel = 1. / screen_size;
	
	if (theta > cos(radians(15.5f))) {
		
		float ray_length = length(global_v_ray);
		
		//float ray_angle = asin(transformed_position.z / ray_length);
		float scrlength_at_distance = tan(radians(15.0)) * transformed_position.x;
		float x_ratio = transformed_position.z / scrlength_at_distance;
		float xp = screen_half + screen_half * x_ratio;
		
		float pinhole_incr = screen_half / tan(radians(33.)); 
		float ratio = screen_size / 100.; 
		float yp = screen_half + pinhole_incr * (transformed_position.y * ratio) / (transformed_position.x + pinhole_incr);
		
		vec2 current_coords = clamp(vec2(xp, yp) / screen_size, 0.0, 1.0);
		vec4 current_pDepth = texture(shadow_map, current_coords);

		float sm_distance = (1. - current_pDepth.r) * 1024.;
		float compare_ratio = 32. / 512. * transformed_position.x;
		vec2 range = vec2(transformed_position.x - compare_ratio, transformed_position.x + 16.);
		
		float media = 0.;
		for (int x = -2; x <= 2; x++) {
			media += texture(shadow_map, clamp(current_coords + vec2(x, 0.) * texel, 0., 1.)).r; 
		}
		
		media = (1. - (media / 5.)) * 1024.;
		sm_distance = 512.;
		media = 512.;
		if (sm_distance > range.x && range.x < media) {
			frag_color = vec4(diffuse * texture_color.xyz * (1.5), texture_color.a);
		}
		else {
			frag_color = vec4((texture_color.xyz * ((ambient - vec3(.4, .0, .0) - .305) + (diff * specular))), texture_color.a);
			
		}
	}
	else {
		frag_color = vec4((texture_color.xyz * ((ambient - vec3(.4, .0, .0) - .505) + (diff * specular))), texture_color.a);
	}
}