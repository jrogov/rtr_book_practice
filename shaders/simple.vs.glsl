#version 330 

layout(location = 0) in vec3 W_Pos;
out vec2 coord;

void main()
{
	coord = W_Pos.xy * 0.5 + 0.5;
	gl_Position = vec4(W_Pos, 1.0);
}