#ifndef _MODEL_H_
#define _MODEL_H_ value

#include "IO_funcs.h"

typedef struct
{

	GLuint VAO;
	/* num of positions if (only raw vertices) condition met, else number of indices of positions  */

	size_t verts_cnt;
	
	/* byte offset | -1 if no attrib*/
	size_t uv_offset;
	size_t normal_offset;

	/* Actually, it should be GLsizei just as glVertexAttribPointer call, but cc screech at it (int-to-pointer-cast) */
	
}
obj_t;

IO_stat_t
loadOBJ(const char* filename, obj_t* model);


void
loadMTLlib(const char* name);


void
useMTL(const char* name);


/* DEBUG */

/* print struct's info*/
void
__debug_print_model_info(obj_t* model);

#endif	/* _MODEL_H_ */