#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "image.h"
#include <stdint.h>


typedef struct
{
	GLenum type;
	union
	{
		GLint i;
		GLfloat f;
	} value;
}
texparam_t;


GLuint gen_plane_tex(GLint format, GLsizei width, GLsizei height, GLenum type, texparam_t* params);
GLuint load_texture( image_t* image, texparam_t* params );
IO_stat_t fload_BMP_texture( const char* filename, texparam_t* params, GLuint *textureID );
void set_tex_params( GLuint target, texparam_t* params );


#define TEXPARAM(a, b) { (a), (b) }
#define TEXPARAM_END { 0, 0 }

#endif /* _TEXTURES_H_ */

