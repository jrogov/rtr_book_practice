#version 330

in vec2 coord;

uniform sampler2D texture;

uniform vec2 center;
uniform float radius;
uniform float strength;

const float PI = 3.141592653589793;

void addDrop(inout vec4 info);


/* info = position.y, velocity.y, normal.x, normal.z */
void main(){
	vec4 info = texture2D(texture, coord);

	addDrop(info);

	gl_FragColor = info;
}


void addDrop(inout vec4 info)
{

	float drop = max(	0.0, 
							1.0 - length(center * 0.5 + 0.5 - coord) / radius );
	drop = 0.5 - cos( drop * PI ) * 0.5;
	info.r += drop * strength;

}