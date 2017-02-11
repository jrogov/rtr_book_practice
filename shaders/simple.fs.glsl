#version 140

out vec3 color; 
// in vec3 original_pos;
in vec3 norm;

uniform vec3 Dir;

void main(){ 
	color = vec3(0.0,0.0,0.4) + dot(normalize(Dir), norm) * vec3(0.6, 0.6, 0.0);// vec3(0.1, 0.9, 0.1);// + original_pos;
}