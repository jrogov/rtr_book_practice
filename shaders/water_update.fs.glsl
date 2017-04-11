#version 330

in vec2 coord;

uniform sampler2D texture;
uniform vec2 delta;

layout(location = 0) out vec4 tex;

void updateInfo(inout vec4 info);

/* info = position.y, velocity.y, normal.x, normal.z */
/* info = height, wave' insinutation velocity, normal */
void main(){
	vec4 info = texture2D(texture, coord);
	updateInfo(info);
	// gl_FragColor = info;
	tex = info;
	gl_FragDepth = 100.0;
}


void updateInfo(inout vec4 info)
{

	/* calculate average neighbor height */
	vec2 dx = vec2(delta.x, 0.0);
	vec2 dy = vec2(0.0, delta.y);

	vec2 n1 = coord - dx; n1 = vec2(clamp(n1.x, 0, 1), clamp(n1.y, 0, 1));
	vec2 n2 = coord - dy; n2 = vec2(clamp(n2.x, 0, 1), clamp(n2.y, 0, 1));
	vec2 n3 = coord + dx; n3 = vec2(clamp(n3.x, 0, 1), clamp(n3.y, 0, 1));
	vec2 n4 = coord + dy; n4 = vec2(clamp(n4.x, 0, 1), clamp(n4.y, 0, 1));


	float average = (
		texture2D(texture, n1).r +
		texture2D(texture, n2).r +
		texture2D(texture, n3).r +
		texture2D(texture, n4).r
	) * 0.25;

	/* change the velocity to move toward the average */
	info.g += (average - info.r) * 2;

	/* attenuate the velocity a little so waves do not last forever */
	info.g *= 0.955;

	// info.r = clamp( info.r, 0, 1);

	/* move the vertex along the velocity */
	info.r += 0.99*info.g;
	// info.r *= 0.75;
}
