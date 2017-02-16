#version 330

out vec3 color; 
in vec3 normal;
in float Dist;
in vec3 ShadeLo;

uniform vec3 Dir;
uniform vec3 Eyecam;

#define G 100
#define LightPower 100
#define SPEC_M 4

vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);
vec3 Shade(
	vec3 Eye,
	vec3 Dir,
	vec3 Norm);

void main(){ 

	vec3 AmbientColor = 0.5*vec3(0.8,0.4,0.1);

	vec3 Lo = Shade(Eyecam, Dir, normal);
	color = AmbientColor + ShadeLo;
	// color = AmbientColor + Lo;
}

vec3 Shade(
	vec3 Eye,
	vec3 LightDir,
	vec3 Norm)
{
	vec3 DiffuseColor = 5*vec3(0.1, 0.1, 0.0);
	vec3 SpecularColor = vec3(0.1, 0.0, 0.1);

	vec3 hsvDiffuse = vec3(32.0/360.0, 0.32, 0.32);
	vec3 hsvSpecular = vec3(16.0/360.0, 0.64, 0.00);

	float cosAlpha = 
	clamp(
		dot(
			normalize(Eye),
			reflect(-LightDir, Norm)
			), 
		0, 1);

	float cosTheta = 
	clamp( 
		dot( 
			normalize(LightDir),
			normalize(Norm)), 
		0, 1);

	float Common = 1/(Dist*Dist) * LightPower;

	vec3 Ls = (SPEC_M + 8) * 1/8 * 1/3.14 * SpecularColor * Common * cosTheta * pow(cosAlpha, SPEC_M);
	vec3 Ld = DiffuseColor * Common  * cosTheta;

	return Ls + Ld; //Lo
}



vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

 
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
