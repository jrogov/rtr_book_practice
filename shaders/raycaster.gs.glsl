#version 150


layout(lines) in;
//layout (triangles) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float cellHeight;

out vec2 UV;


in vec2 orig_pos[];
in vec2 geomUV[];

void pass_through();
void raycast();
void raycast2();
void simple_wall();

// why cant i fucking pass this stupid UV
void main()
{
	raycast();
	//placeholder for vertexUV and geomUV
	UV = geomUV[0];
}

void pass_through(){
	for( int i = 0; i < gl_in.length(); ++i ){
		gl_Position = cellHeight/cellHeight+ gl_in[i].gl_Position;
		//gl_PointSize = gl_in[i].gl_PointSize;
  		//gl_ClipDistance = gl_in[i].gl_ClipDistance;;
		EmitVertex();
	}
	EndPrimitive();
}

void raycast2(){

	float coef = 0.5 * cellHeight;
	float wall_length = 1.0/4 * sqrt(
		pow(orig_pos[1].x - orig_pos[0].x, 2) + 
		pow(orig_pos[1].y - orig_pos[0].y, 2));

	gl_Position = gl_in[1].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * geomUV[1];
	EmitVertex();
	gl_Position = gl_in[1].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * (geomUV[1] + vec2(0.0, 1.0));
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * geomUV[0];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * (geomUV[0] + vec2(0.0, 1.0));
	EmitVertex();

	EndPrimitive();
}


void raycast(){

	float coef = 0.5 * cellHeight;
	float wall_length = 1.0/4 * sqrt(
		pow(orig_pos[1].x - orig_pos[0].x, 2) + 
		pow(orig_pos[1].y - orig_pos[0].y, 2));


	gl_Position = gl_in[1].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * vec2(wall_length, 0.0);
	EmitVertex();
	gl_Position = gl_in[1].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * vec2(wall_length, 1.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, cellHeight, 0.0, 0.0);
	UV = coef * vec2(0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -cellHeight, 0.0, 0.0);
	UV = coef * vec2(0.0, 1.0);
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
}