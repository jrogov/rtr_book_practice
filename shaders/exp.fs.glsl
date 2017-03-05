#version 330


in vec2 UV;

in float Dist;
in vec3 C_Normal;
in vec3 C_Eye;
in vec3 C_LightDir;

in vec3 ShadeLo;

out vec3 color; 

uniform mat4 ViewModel;
uniform mat4 View;
uniform mat4 MVP;
uniform mat4 Model;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;

//uniform vec3 W_LightDir;

uniform Light
{
	vec3 pos;
	vec3 colorpowered;	
};

vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);

vec3 Shade(
	vec3 Eye,
	vec3 LightDir,
	vec3 Norm,
	float Dist);

vec3 Shade_v2(
	vec3 LightDir,
	vec3 Pos,
	vec3 Norm,
	float m);


void main(){ 

	vec3 AmbientColor = 0*vec3(0.2, 0.2, 0.2);

	vec3 Lo;

	Lo = Shade_v2(C_LightDir, 
		C_Eye,
		normalize(C_Normal - vec3(View * vec4(texture( normalSampler, UV).rgb, 0) )), 
		Dist);
	// Lo = Shade_v2(C_LightDir, C_Eye, C_Normal, 50);

	// color = AmbientColor + ShadeLo + 0.001 * texture( textureSampler, UV).rgb ;

	color =  texture( textureSampler, UV).rgb + 3*Lo;

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

vec3 Shade(
	vec3 Eye,
	vec3 LightDir,
	vec3 Norm,
	float Dist)
{

	#define G 100
	#define LightPower 50
	#define SPEC_M 10

	vec3 DiffuseColor = 0.5*vec3(0.8,0.4,0.1);
	vec3 SpecularColor = 0.5*vec3(0.1, 0.1, 0.1);

	LightDir = normalize(LightDir);

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
			LightDir,
			Norm), 
		0, 1);

	float Common = LightPower/(Dist*Dist);

	vec3 Ls = (SPEC_M + 8)/(8*3.14) * SpecularColor * Common * cosTheta * pow(cosAlpha, SPEC_M);
	vec3 Ld = DiffuseColor * Common  * cosTheta;
	// Ls *= 0.5;
	vec3 Lo = Ls+Ld;
	return Lo; //Lo
}

vec3 Shade_v2(
	vec3 LightDir,
	vec3 Pos,
	vec3 Norm,
	float m)
{

	#define pi 3.1415

	vec3 LightColor = vec3(1,1,1);

	vec3 DiffColor = vec3(0.8,0.4,0.1);
	vec3 SpecColor = 0.1*vec3(1.0,1.0,1.0);//0.5*vec3(0.1, 0.1, 0.1);

	float dist = length(LightDir);

	LightDir = normalize(LightDir);

	vec3 h = normalize((0.5*LightDir) - Pos);

	float cosSpec = dot(h, Norm);
	float cosDiff = dot( LightDir, Norm );

	cosSpec = 
	clamp(
		dot(
			normalize(Pos),
			reflect(-LightDir, Norm)
			), 
		0, 1);

	cosDiff = 
	clamp( 
		dot( 
			LightDir,
			Norm), 
		0, 1);

	vec3 Lo 
		= 	(
				DiffColor / pi 
				+ SpecColor * (m+8)/(8*pi) * pow(cosSpec, m) // / (dist*dist)
			) 
			* LightColor  * cosDiff;

	return Lo;
}
