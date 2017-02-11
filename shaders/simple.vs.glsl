#version 140

in vec3 Pos; 
in vec3 Normal;
out vec3 original_pos;
out vec3 normal;

uniform mat4 MVP;

void main(){
	original_pos = Pos;
	// normal = (MVP * -vec4(Normal.x, Normal.z, Normal.y, 0)).xyz;
	normal = vec3(MVP*vec4(Normal,0));
	gl_Position = MVP * vec4(Pos, 1);
	// gl_Position =  MVP * vec4(Pos.x, Pos.z, Pos.y, 1);
}