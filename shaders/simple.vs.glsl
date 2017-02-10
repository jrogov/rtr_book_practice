#version 140

in vec3 Pos; 
out vec3 original_pos;

uniform mat4 MVP;

void main(){
	original_pos = Pos;
	gl_Position =  MVP * vec4(Pos, 1);
}