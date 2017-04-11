#version 330

in vec2 coord;

uniform sampler2D texture;
uniform vec2 delta;

layout(location = 0) out vec4 tex;

void updateNormal(inout vec4 info);

/* info = position.y, velocity.y, normal.x, normal.z */
void main(){
	vec4 info = texture2D(texture, coord);
	updateNormal(info);
	// gl_FragColor = info;
	tex = info;
	gl_FragDepth = 100.0;
}

void updateNormal(inout vec4 info)
{
	/* update the normal */
	vec3 dx = vec3(delta.x, texture2D(texture, vec2(coord.x + delta.x, coord.y)).r - info.r, 0.0);
	vec3 dy = vec3(0.0, texture2D(texture, vec2(coord.x, coord.y + delta.y)).r - info.r, delta.y);
	info.ba = normalize(cross(dy, dx)).xz;

}