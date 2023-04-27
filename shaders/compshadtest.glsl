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

//uniform vec3 ligth_pos;
//uniform vec3 ligth_dir;
//uniform vec3 ambient;
//uniform vec3 specular;

uniform sampler2D color_texture;
uniform sampler2D normals_texture;
uniform sampler2D positions_texture;

uniform sampler2D tile_map;
uniform sampler2D h_map1;
uniform sampler2D h_map2;

vec2 tm_coords = vec2(0., 0.);
vec2 hm_coords = vec2(0., 0.);

float hm_size = 1024.;

out vec4 frag_color;

#define DIRECTIONAL 0
#define POINT 1
#define FOCUS 2

struct Ligth {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 specular;
	int type;
	float frustrum;
	float strength;
	float attenuation;
};

#define MAX_LIGTHS 8
uniform Ligth ligths[MAX_LIGTHS];
uniform int NUMBER_LIGTHS;

bool ray_cast(const Ligth ligth, vec3 point_)
{
	bool hit = false;
	int jump = 64;
	int tile_size = 64;
	vec3 l_pos = ligth.position;
	
	point_ = vec3(int(point_.x), int(point_.y), int(point_.z));
	
	vec3 st_point = point_;
	vec2 ray_dir = normalize(vec2(st_point.xz) - vec2(l_pos.xz));
	
	vec2 ray_2d = ray_dir;
	vec3 ray_3d = normalize(vec3(st_point.xyz) - vec3(l_pos.xyz));
	
	vec2 l_in_tile = vec2(l_pos.xz) / 64.;

	int left_line = int(l_in_tile.x) * tile_size;
	int rigth_line = (int(l_in_tile.x) + 1) * tile_size;
	int bottom_line = int(l_in_tile.y) * tile_size;
	int top_line = (int(l_in_tile.y) + 1) * tile_size;
	
	vec2 st_lines;
	vec2 jump_dir;
	
	if (ray_3d.x < 0.0) {
		st_lines.x = left_line;
		jump_dir.x = -tile_size;
	} else {
		st_lines.x = rigth_line;
		jump_dir.x = tile_size;
	}
	
	if (ray_3d.z < 0.0) {
		st_lines.y = bottom_line;
		jump_dir.y = -tile_size;
	} else {
		st_lines.y = top_line;
		jump_dir.y = tile_size;
	}
	
	float limit_length = abs(length(l_pos.xz - point_.xz));
	float current_length = 0.0;
	point_.xyz = l_pos.xyz;

	//while (point_.x > 0. && point_.z > 0.0 && point_.x < hm_size && point_.z < hm_size)
	//for (int x = 0; x < 100; x++)
	int range = 0;
	
	float ratio = length(vec3(st_point.xyz) - vec3(l_pos.xyz)) / 100.;
	ratio = 2.;
	
	vec2 range_x = vec2(0.);
	if (l_pos.x > st_point.x) range_x = vec2(st_point.x, l_pos.x);
	else range_x = vec2(l_pos.x, st_point.x);
	
	vec2 range_z = vec2(0.);
	if (l_pos.z > st_point.z) range_z = vec2(st_point.z, l_pos.z);
	else range_z = vec2(l_pos.z, st_point.z);
	
	//while (int(point_.x) != int(st_point.x))
	//while (range < 10)
	while (point_.x >= range_x.x && point_.x <= range_x.y && point_.z >= range_z.x && point_.z <= range_z.y)
	{
		//range++;
		hm_coords = point_.xz / hm_size;
		hm_coords.y = 1. - hm_coords.y; 
		vec4 p_at_hm1 = texture(h_map1, hm_coords);
		vec4 p_at_hm2 = texture(h_map2, hm_coords);
		
		//if (int(st_point.x) == int(point_.x) && int(st_point.z) == int(point_.z)) {
		//	hit = true;
		//	break;
		//}
		
		if (p_at_hm1.a != 0.0 ) {
			float pHeigth = p_at_hm1.r * 255.;
			float pHeigth2 = p_at_hm2.r * 255.;
			
			float distance_diff = length(st_point.xz - point_.xz);
			
			//if (pHeigth > l_pos.y) {
			//	hit = true;
			//	break;
			//}
			
			if (pHeigth > point_.y && pHeigth2 < point_.y) {
				if (distance_diff > 8.) {
					hit = true;
					break;
				}
			}
		}
		

		//vec2 tile_coords = vec2(hm_coords.x, hm_coords.y);
		//vec4 p_at_tm = texture(tile_map, hm_coords);
		//
		//
		//if (p_at_tm.r  == 1.0) jump = 16;
		//else if (p_at_tm.g == 1.0) jump = 4;
		//else if (p_at_tm.b == 1.0) jump = 32;
		//else jump = 64;
		//
		//int decr = int(point_.x) % jump;
		//int incr = jump - decr;
		//
		//jump_dir.x = int(jump_dir.x / abs(jump_dir.x)) * jump;
		//jump_dir.y = int(jump_dir.y / abs(jump_dir.y)) * jump;
		//
		//if (st_lines.x > point_.x) st_lines.x = int(point_.x) + incr;
		//else {
		//	st_lines.x = int(point_.x) - decr;
		//	if (int(st_lines.x) == int(point_.x)) st_lines.x += jump_dir.x;
		//}
		//
		//decr = int(point_.y) % jump;
		//incr = jump - decr;
		//
		//if (st_lines.y > point_.z) st_lines.y = int(point_.z) + incr;
		//else {
		//	st_lines.y = int(point_.z) - decr;
		//	if (int(st_lines.y) == int(point_.z)) st_lines.y += jump_dir.y;
		//}
		//
		//float y_ray_length = abs((point_.z - st_lines.y) / sin(atan(ray_3d.z / ray_3d.x)));
		//float x_ray_length = abs((point_.x - st_lines.x) / cos(atan(ray_3d.z / ray_3d.x)));
		//
		//if (y_ray_length < x_ray_length) {
		//	point_.x = l_pos.x + (abs(l_pos.z - st_lines.y) / (ray_3d.z / ray_3d.x));
		//	point_.z = st_lines.y;
		//	st_lines.y += jump_dir.y;
		//	point_.y = l_pos.y + (abs(l_pos.z - st_lines.y) / (ray_3d.y / ray_3d.x));
		//	
		//	
		//}
		//else {
		//	point_.z = l_pos.z + (abs(l_pos.x - st_lines.x) * (ray_3d.z / ray_3d.x));
		//	point_.x = st_lines.x;
		//	st_lines.x += jump_dir.x;
		//	point_.y = l_pos.y + (abs(l_pos.x - st_lines.x) * (ray_3d.y / ray_3d.x));
		//}
		
		point_.x += ray_3d.x * ratio; 
		point_.z += ray_3d.z * ratio;
		point_.y += ray_3d.y * ratio;
		
		
		current_length = length(l_pos.xz - point_.xz);
		
	}
	
	return hit;
	
}

vec3 calcLigthValues(Ligth ligth, vec3 point, vec4 normal) {

	vec3 v_ray = normalize(ligth.position - point);
	float theta = dot((ligth.direction), normalize(-v_ray));
	
	float diff = dot((normal.xyz), v_ray);
		
	float inner_cutoff = cos(radians(ligth.frustrum - 10));
	float outer_cutoff = cos(radians(ligth.frustrum));
	float epsilon =  inner_cutoff - outer_cutoff;
	
	float intensity = clamp((theta - outer_cutoff) / epsilon, 0.0, 1.0) + .5;
			
	float distance = length(normalize(v_ray));
	float attenuation = 1.0 / (1.0 + .001 * distance + ligth.attenuation * (distance * distance));
	
	float strength = 1. - length(ligth.position - point) / ligth.strength; 
	
	vec3 diffuse = (ligth.ambient + ligth.specular + diff) * attenuation * intensity * strength;
	
	
	if (theta > cos(radians(ligth.frustrum))) {
		bool has_hit = ray_cast(ligth, point);
		//bool has_hit = false;
		if (!has_hit) {
			diffuse *= 3.5;
		}
		else {
			diffuse *= .7;
		}
	}
	
	else diffuse *= .35;
	
	return diffuse;
		
}


void main()
{
	vec4 texture_color = texture(color_texture, out_uvs);
	vec4 normal_color = texture(normals_texture, out_uvs);
	vec4 fragment_position = texture(positions_texture, out_uvs);
	
	
	if (texture_color.a == 0.0) discard;
	else {
		vec3 point_ = fragment_position.xyz * 1024.;
		
		normal_color.x = (0.5 - normal_color.x) * (-2);
		normal_color.y = (0.5 - normal_color.y) * (-2);
		normal_color.z = (0.5 - normal_color.z) * (-2);
		
		vec3 result = vec3(0.);
		
		for (int i = 0; i < NUMBER_LIGTHS; i++) {
			result += calcLigthValues(ligths[i], point_, normal_color);
		}
		
		
		frag_color = vec4(result * texture_color.rgb, texture_color.a);
		
		//vec3 ambient = vec3(0.);
		//vec3 specular = vec3(0.);
		//vec3 strength = 0.;
		//float attenuation = 0.;
		//float intensity = 0.;
		//
		//
		//vec3 v_ray = normalize(ligth_pos - point_);
		//float theta = dot((ligth_dir), normalize(-v_ray));
		////vec3 ambient = vec3(.7, .5, .2) * .9;
        //
		//
		//float diff = dot((normal_color.xyz), v_ray);
		//
		//float inner_cutoff = cos(radians(ligths[0].frustrum - 10));
		//float outer_cutoff = cos(radians(ligths[0].frustrum));
		//float epsilon =  inner_cutoff - outer_cutoff;
		//
		//float intensity = clamp((theta - outer_cutoff) / epsilon, 0.0, 1.0) + .5;
		//		
		//float distance = length(normalize(v_ray));
		//float attenuation = 1.0 / (1.0 + .001 * distance + ligths[0].attenuation * (distance * distance));
		//
		////vec3 specular = vec3(0., 0., .35);
		//
		//float strength = 1. - length(ligth_pos - point_) / ligths[0].strength; 
		////strength = 1.;
		//
		////vec3 diffuse = (ambient * attenuation * intensity * strength + diff * attenuation * intensity * strength + specular * attenuation * intensity * strength);
		//vec3 diffuse = (ambient + specular + diff) * attenuation * intensity * strength;
		//
		//if (theta > cos(radians(ligths[0].frustrum))) {
		//	bool has_hit = ray_cast(ligths[0], point_);
		//	//bool has_hit = false;
		//	if (!has_hit) {
		//		frag_color = vec4(diffuse * texture_color.xyz * (3.5), texture_color.a);
		//	}
		//	else {
		//		frag_color = vec4(texture_color.xyz * diffuse * .7, texture_color.a);
		//		//frag_color = vec4(vec3(1., 0., 1.), 1.);
		//	}
		//}
		//
		//else frag_color = vec4(texture_color.xyz * diffuse * .35, texture_color.a);
		////frag_color = vec4(diffuse * texture_color.xyz * (1.5), texture_color.a);
		
	}
	
}