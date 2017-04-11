#version 140

in vec2 Pos2D; 
in vec2 vertexUV;

out vec2 geomUV;
//out vec2 UV;

out vec2 orig_pos;

uniform mat4 MVP;
uniform float levelSize;

void main(){
	//UV = vertexUV;
	mat4 scale = mat4(levelSize);
	scale[3][3] = 1.0;

	geomUV = vec2(MVP * scale * vec4(vertexUV, 0.0, 0.0));

	orig_pos = Pos2D;
	gl_Position =  MVP * (scale * vec4( Pos2D.x, 0.0, Pos2D.y, 1.0 ));
	//gl_Position =  MVP * (scale * vec4( Pos2D.x, Pos2D.y, 0.0, 1.0 ));

}
