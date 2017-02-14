#ifndef _CAMERA_H_
#define _CAMERA_H_ 

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glu.h>


typedef struct
{
	GLfloat camX;
	GLfloat camY;
	GLfloat fb;
	GLfloat rl;
	GLfloat ud;
	GLfloat speedFB;
	GLfloat speedRL;
	GLfloat speedUD;
	GLfloat camX_sens;
	GLfloat camY_sens;
	GLfloat accFB;
	GLfloat accRL;
	GLfloat accUD;
} movement_t ;

void 
__debug_print_movement__(FILE* f, movement_t *m);

#endif /* _CAMERA_H */