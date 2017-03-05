#include "light.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/glu.h>


globlight_t*
initGlobLight(GLfloat posx, GLfloat posy, GLfloat posz,
			 GLfloat colorr, GLfloat colorg, GLfloat colorb,
			 GLfloat Power,
			 void (*func)(globlight_t* light, GLfloat* args)
			 )
{
	globlight_t* light;

	light = (globlight_t*) malloc(sizeof(globlight_t));

	light->pos[0] = posx;
	light->pos[1] = posy;
	light->pos[2] = posz;

	colorr*=Power;
	colorg*=Power;
	colorb*=Power;

	light->colorpowered[0] = colorr;
	light->colorpowered[1] = colorg;
	light->colorpowered[2] = colorb;

	light->mut_func = func;

	return light;
}

#define VEC_SIZE 3

globlight_t*
initGlobLightv(GLfloat pos[VEC_SIZE], 
					GLfloat color[VEC_SIZE], 
					GLfloat Power,
					void (*func)(globlight_t* light, GLfloat* args)
					)
{

	globlight_t* light;

	light = (globlight_t*) malloc(sizeof(globlight_t));

	memcpy(&(light->pos), pos, sizeof(VEC_SIZE) * sizeof(*pos));

	light->colorpowered[0] = Power * color[0];
	light->colorpowered[1] = Power * color[1];
	light->colorpowered[2] = Power * color[2];

	light->mut_func = func;

	return light;
}

#undef VEC_SIZE

/* args = XZ angle, XY angle, - */
void
mut_func_rotation(globlight_t* light, GLfloat args[2]){
	light->pos[0] = light->func_param[0] * cos(args[0]) * sin(args[1]);
	light->pos[1] = light->func_param[1] * cos(args[1]);
	light->pos[2] = light->func_param[2] * sin(args[0]);
}

void mut_func_nope(globlight_t* light, GLfloat *args){}