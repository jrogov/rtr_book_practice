#ifndef _MODEL_H_
#define _MODEL_H_ value

#include "IO_funcs.h"

typedef struct
{
	GLfloat* vertices; 
	GLfloat* uvs; 
	GLfloat* normals;
	
	/* NULL or First element is -1 if no indices used: only raw positions */

	GLuint* vert_indices; 
	GLuint* uv_indices; 
	GLuint* norm_indices;

	/* num of positions if (only raw vertices) condition met, else number of indices of positions  */

	size_t vertices_count;
	size_t uvs_count;
	size_t normals_count;
	size_t vert_indices_count;
	size_t uv_indices_count;
	size_t norm_indices_count;
}
obj_t;

IO_stat_t
loadOBJ(const char* filename, obj_t* object);


void
loadMTLlib(const char* name);


void
useMTL(const char* name);



#endif	/* _MODEL_H_ */