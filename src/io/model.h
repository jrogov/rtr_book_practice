#ifndef _MODEL_H_
#define _MODEL_H_ value

#include "IO_funcs.h"

typedef struct
{
	GLfloat* verts;
	GLfloat* uvs;
	GLfloat* norms;

	/* NULL or First element is -1 if no indices used: only raw positions */

	GLuint* vert_inds;
	GLuint* uv_inds;
	GLuint* norm_inds;

	/* num of positions if (only raw vertices) condition met, else number of indices of positions  */

	size_t verts_cnt;
	size_t uvs_cnt;
	size_t norms_cnt;
	size_t vert_ind_cnt;
	size_t uv_ind_cnt;
	size_t norm_ind_cnt;
}
model_t;

IO_stat_t
loadOBJ(const char* filename, model_t* model);


void
loadMTLlib(const char* name);


void
useMTL(const char* name);


/* DEBUG */

/* print struct's info*/
void
__debug_print_model_info(model_t* model);

#endif	/* _MODEL_H_ */