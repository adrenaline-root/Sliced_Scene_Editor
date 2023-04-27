---vertex shader

#version 130

in vec4 vPosition;
in vec2 vTextCoords;

out vec2 uvs;


void main()
{ 
	gl_Position = vPosition;
	uvs = vTextCoords;
}


---fragment shader

#version 130

in vec2 uvs;
out vec4 FragColor;

uniform vec3 ligth_pos;
uniform sampler2D src_pixbuf;
uniform vec2 viewport_size;

highp float texel_size = 1.0 / viewport_size.x;

float hits[4] = float[4](1., 1., 1., 1.);


vec2 current_coords = vec2(gl_FragCoord.x / viewport_size.x, gl_FragCoord.y / viewport_size.y);

void main()
{
	float du = (ligth_pos.x - current_coords.x);
	float dv = (ligth_pos.y - current_coords.y);
	int count = 0;
	bool hit = false;
	float max = 0.0;
	//float max = texture(src_pixbuf, current_coords).w;
	vec3 ligth_hit;
	float current_higth = 0.0;

	if (texture(src_pixbuf, current_coords).w != 0.0) {
		
		if (abs(du) >= abs(dv)) {
			for (float x = 0.0; x < abs(du); x+= texel_size) {
				highp float fraction_x = x / abs(du);
				highp float fraction_y = (dv / (abs(du) / texel_size)) * (x / texel_size);
				
				vec2 current_pos = vec2(current_coords.x + fraction_x * du, current_coords.y + fraction_y);
				
				vec4 pixel_candidate = texture(src_pixbuf, current_pos);
				
				if (pixel_candidate.a != 0.0 && pixel_candidate.r != 0.0) {
					hit = true;
					//if (pixel_candidate.r != max && count < 4) {
					//	hits[count] = pixel_candidate.r;
					//	max = pixel_candidate.r;
					//	count++;
					//}
					if (pixel_candidate.r > max) {
						max = pixel_candidate.r;
						//ligth_hit = clamp((vec3(current_coords.xy, max) - ligth_pos), 0.0, 1.0);
						ligth_hit = vec3(max);
					}
				}
				
			}
		}
		
		else {
			for (float x = 0.0; x < abs(dv); x+= texel_size) {
				highp float fraction_x = x / abs(dv);
				highp float fraction_y = (du / (abs(dv) / texel_size)) * (x / texel_size);
				
				vec2 current_pos = vec2(current_coords.x + fraction_y, current_coords.y + fraction_x * dv);
				
				vec4 pixel_candidate = texture(src_pixbuf, current_pos);
				
				if (pixel_candidate.a != 0.0 && pixel_candidate.r != 0.0) {
					hit = true;
					//if (pixel_candidate.r != max && count < 4) {
					//	hits[count] = pixel_candidate.r;
					//	count++;
					//}
					if (pixel_candidate.r > max) {
						max = pixel_candidate.r;
						//ligth_hit = clamp((vec3(current_coords.xy, max) - ligth_pos), 0.0, 1.0);
						ligth_hit = vec3(max);
					}
				}
			}
		}
	
	}
	
	else discard;
	
	
	//if (!hit) FragColor = vec4(1., 1., 1., 1.);
	//else FragColor = texture(src_pixbuf, current_coords);
	if (hit) FragColor = vec4(ligth_hit, 1.0);
	//FragColor = vec4(hits[0], hits[1], hits[2], texture(src_pixbuf, current_coords).w);
	else discard;
}