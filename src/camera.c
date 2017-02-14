#include "camera.h"

void 
__debug_print_movement__(FILE* f, movement_t *m)
{
	printf(
		"\ttCamX\tCamY\n"
		"\t%.2f\t%.2f\n"
		"\tF_B\tR_L\tU_D\n"
		"\t%.2f\t%.2f\t%.2f\n",
		m->camX, m->camY,
		m->fb, m->rl, m->ud
		);
}


