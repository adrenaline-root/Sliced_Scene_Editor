---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;


uniform highp float height_correction;

out vec2 uvs;


void main()
{ 
	gl_Position = vPosition;
	uvs = vTextCoords;
}


---fragment shader

#version 130

uniform sampler2D color_map;
uniform sampler2D normals_map;
uniform sampler2D heigth_map;
uniform sampler2D shadow_map;

uniform vec2 viewport_size;
uniform highp float height_correction;
uniform vec3 ligth_pos;
uniform mat4 rot_mat;
uniform float scale;

in vec2 uvs;
out vec4 FragColor;

highp float texel_sizey = 1.0 / viewport_size.y;
highp float texel_sizex = 1.0 / viewport_size.x;

vec2 current_coords = vec2(gl_FragCoord.x / viewport_size.x, (gl_FragCoord.y / viewport_size.y));
//vec2 current_coords = uvs;

vec4 color_at_c_map = texture(color_map, current_coords);
vec4 color_at_h_map = texture(heigth_map, current_coords);
vec4 normals_color = texture(normals_map, current_coords);


void main()
{
	normals_color = vec4(vec3((normals_color.x - .5) * 2.0, (normals_color.y - .5) * 2.0, (normals_color.z - .5) * 2.0), 1.0) * rot_mat;
	//normals_color = vec4(vec3((normals_color.x * .5 + .5), (normals_color.y * .5 +  .5), (normals_color.z * .5 + .5)), 1.0);
	
	if (color_at_c_map.a != 0) {
		
		//if (color_at_h_map.r == 0.0) discard;
		
		vec2 actual_coords = vec2(current_coords.x, current_coords.y - texel_sizey * 3 - (color_at_h_map.r * 255.0 * texel_sizey) * scale);
		vec4 color_at_s_map = texture(shadow_map, actual_coords);
		
		//vec2 pixel_coords = vec2(uvs.x, uvs.y - (color_at_h_map.r * 255.0) * texel_sizey);
		
		vec3 ligth_dir = (vec3(ligth_pos.x, 0.1, actual_coords.y) - vec3(actual_coords.x, color_at_h_map.r, ligth_pos.y));
		
		//vec3 ligth_dir = (vec3(0., 0, 0.) - vec3(128, 0., 0.));
		float diff = dot(normalize(normals_color.xyz), normalize(ligth_dir));
		
		vec3 ambient = vec3(1., .4, .4);
		vec3 specular = vec3(.3);
		
		float distance = length(ligth_dir);
		float attenuation = 1.0 / (1.0 + 1.7 * distance + 3.8 * (distance * distance));
		float intensity = .9;
		
		//vec3 diffuse = (ambient * diff * attenuation);
		vec3 diffuse =  ((ambient * attenuation * intensity + diff * attenuation * intensity + specular * attenuation * intensity));
		
		vec4 heigth_at_left = texture(shadow_map, actual_coords + (-texel_sizex, 0));
		vec4 heigth_at_rigth = texture(shadow_map, actual_coords + (texel_sizex, 0));
		vec4 heigth_at_top = texture(shadow_map, actual_coords + (0, texel_sizey ));
		vec4 heigth_at_bottom = texture(shadow_map, actual_coords + (0, -texel_sizey));
		
		
		if (color_at_s_map.r > color_at_h_map.r && color_at_s_map.a != 0.0 ) {
			//&& heigth_at_left.r > color_at_h_map.r && heigth_at_rigth.r > color_at_h_map.r
			//&& heigth_at_top.r > color_at_h_map.r && heigth_at_bottom.r > color_at_h_map.r)) {
			
			highp float dr = color_at_s_map.r; 
			FragColor = vec4((color_at_c_map.rgb * ((ambient - vec3(.4, .0, .0) - .35) + (diff * specular))), 1.0);
			//FragColor = vec4(normalize(normals_color.xyz), 1.0);
		
		}
		else {
			FragColor = vec4(color_at_c_map.rgb * diffuse * 1.5, 1.0);
			//FragColor = vec4(normalize(normals_color.xyz), 1.0);
		}
	}
	else discard;
	
	//FragColor = color_at_c_map;
	
	
}