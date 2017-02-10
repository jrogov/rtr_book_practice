#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>

/* Various statuses for IO functions */

#include "IO_funcs.h"

/*
	Load shader to cache.
	Cache will be rewritten ONLY if
		shader file was modificated
		OR passed another shader type for the same shader file
*/
IO_stat_t load_shader(const char* filename, GLuint *shader, GLenum type);

/*
	Loads and compiles shader program from shader files.
	If some _sfile is NULL, this shader won't be compiled and linked to program
*/
IO_stat_t fload_program(
    const char* vertex_sfile,
    const char* fragment_sfile,
    const char* geometry_sfile,
    const char* tess_control_sfile,
    const char* tess_evaluation_sfile,
    GLuint *program
    );

/*
	DEBUG
	prints current cache state
*/
void __debug_print_shader_cache__(FILE* f);

#endif
