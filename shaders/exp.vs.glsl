// Interpolation
// extern functins

#version 330

layout(location = 0) in vec3 M_Pos; 
layout(location = 1) in vec2 M_UV;
layout(location = 2) in vec3 M_Normal;

out vec2 UV;

out float Dist;
out vec3 C_Normal;
out vec3 C_Eye;
out vec3 C_LightDir;
out vec3 ShadeLo;


uniform mat4 ViewModel;
uniform mat4 View;
uniform mat4 MVP;
uniform mat4 Model;

uniform vec3 W_LightDir;



vec3 Shade(
	vec3 Eye,
	vec3 W_LightDir,
	vec3 Norm);

#define pi 3.1415

vec3 Shade_v2(
	vec3 LightDir,
	vec3 Pos,
	vec3 Norm,
	float m);

void main(){

	UV = M_UV;

	gl_Position = (MVP) * vec4(M_Pos, 1);
	
	C_Normal = vec3( ViewModel * vec4(M_Normal,0));


	vec3 C_LightPos = vec3(View * vec4(W_LightDir, 1));
	C_Eye = vec3(0,0,0) - vec3(ViewModel * vec4(M_Pos,1));

	C_LightDir = C_LightPos + C_Eye;

	Dist = 0.5* length(C_LightDir);

	ShadeLo = Shade(C_Eye, C_LightDir, C_Normal);
	ShadeLo = Shade_v2(C_LightDir, C_Eye, C_Normal, 10);

}


vec3 Shade(
	vec3 Eye,
	vec3 LightDir,
	vec3 Norm)
{

	#define G 100
	#define LightPower 3
	#define SPEC_M 30

	vec3 DiffuseColor = vec3(0.8,0.4,0.1);
	vec3 SpecularColor = vec3(1.0,1.0,1.0);//0.5*vec3(0.1, 0.1, 0.1);

	float Dist = distance(Eye, LightDir);

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

	// float Common = LightPower /(Dist*Dist);

	vec3 Ls = ((SPEC_M + 8)/(8*3.14) /(Dist*Dist) * cosTheta * pow(cosAlpha, SPEC_M)) * SpecularColor ;
	vec3 Ld = DiffuseColor * LightPower  * cosTheta;
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

	vec3 LightColor = vec3(1,1,1);

	vec3 DiffColor = vec3(0.8,0.4,0.1);
	vec3 SpecColor = vec3(1.0,1.0,1.0);//0.5*vec3(0.1, 0.1, 0.1);

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
