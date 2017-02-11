#version 150


layout(triangles) in;

// common
layout(triangle_strip, max_vertices = 3) out;

// wireframe
// layout(line_strip, max_vertices = 6) out;

in vec3 normal[];
out vec3 norm;

void pass_through();
void raycast();
void simple_wall();
void wireframe();

// why cant i fucking pass this stupid UV
void main()
{
	// wireframe();
	pass_through();
}

void pass_through(){
	for( int i = 0; i < gl_in.length(); ++i ){
		gl_Position = gl_in[i].gl_Position;
		//gl_PointSize = gl_in[i].gl_PointSize;
  		//gl_ClipDistance = gl_in[i].gl_ClipDistance;;
		norm = normal[i];
		EmitVertex();
	}
	EndPrimitive();
}

void wireframe(){
	for (int i=1; i< gl_in.length(); ++i){
		gl_Position = gl_in[i-1].gl_Position;
		norm=normal[i-1];
		EmitVertex();
		gl_Position = gl_in[i].gl_Position;
		norm=normal[i];
		EmitVertex();
		EndPrimitive();
	}
}






/*
void raycast(){

	float coef = 0.5 * cellHeight;
	float wall_length = 1.0/4 * sqrt(
		pow(orig_pos[1].x - orig_pos[0].x, 2) + 
		pow(orig_pos[1].y - orig_pos[0].y, 2));

	gl_Position = gl_in[0].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * vec2(0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = gl_in[1].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * vec2(wall_length, 0.0);
	EmitVertex();
	gl_Position = gl_in[1].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * vec2(wall_length, 1.0);
	EmitVertex();

	EndPrimitive();
}

void simple_wall(){
	gl_Position = vec4(-1.0, -1.0, 0.0, 0.5);
	EmitVertex();

	gl_Position = vec4(-1.0, 1.0*cellHeight/cellHeight, 0.0, 0.5);
	EmitVertex();

	gl_Position = vec4(1.0, 1.0, 0.0, 0.5);
	EmitVertex();

	gl_Position = vec4(1.0, -1.0, 0.0, 0.5);
	EmitVertex();

	gl_Position = vec4(-1.0, -1.0, 0.0, 0.5);
	EmitVertex();

	EndPrimitive();
}*/