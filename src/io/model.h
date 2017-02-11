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
obj_t;

IO_stat_t
loadOBJ(const char* filename, obj_t* object);


void
loadMTLlib(const char* name);


void
useMTL(const char* name);



#endif	/* _MODEL_H_ */