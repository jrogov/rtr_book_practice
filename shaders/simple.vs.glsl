in vec3 W_Pos;
out vec2 coord;

void main()
{
	coord = 	W_Pos.xz;
	gl_Position = vec4(W_Pos, 1.0);
}