#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <GL/glu.h>

typedef struct globlight_t
{
	/* GL transfer data */

	/* TRANSFER */
	GLfloat pos[3];
	GLfloat colorpowered[3];			/* light's color times light's power: multiplication optimization */

	/* /TRANSFER */

	/* Application calculations data */

	GLfloat func_param[3];	/* function constant params. e.g.: ellipsoid params */
	void (*mut_func)(struct globlight_t* light, GLfloat* args); /* function for changing light's pos */
} globlight_t;

/* update manually according to TRANSFER tags above */
#define TRANSFER_DATA_SIZE (sizeof(GLfloat) * (3 + 3))

globlight_t*
initGlobLight(GLfloat posx, GLfloat posy, GLfloat posz,
			 GLfloat colorr, GLfloat colorg, GLfloat colorb,
			 GLfloat Power,
			 void (*func)(globlight_t* light, GLfloat* args)
			 );


#define VEC_SIZE 3

globlight_t*
initGlobLightv(GLfloat pos[VEC_SIZE], 
					GLfloat color[VEC_SIZE], 
					GLfloat Power,
					void (*func)(globlight_t* light, GLfloat* args)
					);

#undef VEC_SIZE



/* MUT FUNCS */

void
mut_func_rotation(globlight_t* light, GLfloat* args);

#endif /* _LIGHT_H_ */