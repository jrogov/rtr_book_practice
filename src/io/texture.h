#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "image.h"

GLuint load_texture( image_t* image );
GLuint fload_BMP_texture( const char* filename );

#endif /* _TEXTURES_H_ */
