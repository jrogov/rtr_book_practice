#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_


typedef struct
{
	GLfloat x, y, z;
	
	GLfloat fb, rl, ud;
	
	GLfloat speedFB, speedRL, speedUD;
	GLfloat accFB, accRL, accUD;
	
	GLfloat camX, camY;
	GLfloat camX_sens, camY_sens;
} movement_t;

#endif /* _MOVEMENT_H_ */