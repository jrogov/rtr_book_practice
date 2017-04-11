#include <GL/glew.h>
#include <GL/glu.h>

#include <stdio.h>
#include <err.h>

#include "log.h"
#include "IO_funcs.h"
#include "image.h"
#include "bmp.h"
#include "texture.h"

GLuint gen_plane_tex(GLint format, GLsizei width, GLsizei height, GLenum type, texparam_t* params)
{
	GLuint target;
	glGenTextures(1, &target);
	glBindTexture(GL_TEXTURE_2D, target);
	glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, NULL);
	set_tex_params(GL_TEXTURE_2D, params);
	return target;
}

void set_tex_params(GLuint target, texparam_t* params)
{
	for( texparam_t* p = &params[0] ; p->type != 0 ; ++p )
	{
		switch(p->type)
		{
			case GL_DEPTH_STENCIL_TEXTURE_MODE:
			case GL_TEXTURE_BASE_LEVEL:
			case GL_TEXTURE_COMPARE_FUNC:
			case GL_TEXTURE_COMPARE_MODE:
			case GL_TEXTURE_MIN_FILTER:
			case GL_TEXTURE_MAG_FILTER:
			case GL_TEXTURE_MAX_LEVEL:
			case GL_TEXTURE_SWIZZLE_R:
			case GL_TEXTURE_SWIZZLE_G:
			case GL_TEXTURE_SWIZZLE_B:
			case GL_TEXTURE_SWIZZLE_A:
			case GL_TEXTURE_WRAP_S:
			case GL_TEXTURE_WRAP_T:
			case GL_TEXTURE_WRAP_R:
				glTexParameteri(target, p->type, p->value.i);
				printf("ERROR CODE = %d\n", glGetError());
				printf("0x%X 0x%X\n", p->type, p->value.i);
				break;

			case GL_TEXTURE_LOD_BIAS:
			case GL_TEXTURE_MIN_LOD:
			case GL_TEXTURE_MAX_LOD:
				glTexParameterf(target, p->type, p->value.f);
				printf("ERROR CODE = %d\n", glGetError());
				printf("0x%X %.0f\n", p->type, p->value.f);
				break;
		}
	}
}

GLuint load_texture( image_t* image, texparam_t* params )
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(
		GL_TEXTURE_2D, 0,
		GL_RGB, image->width, image->height, 0,
		GL_BGR,
		GL_UNSIGNED_BYTE,
		image->pixels);

	if( params != NULL )
		set_tex_params(GL_TEXTURE_2D, params);


	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

/*
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/

	return textureID;
}

IO_stat_t fload_BMP_texture( const char* filename, texparam_t* params, GLuint *textureID )
{
	FILE* f;
	image_t* image;
	IO_stat_t status;

	f = fopen(filename, "r");
	if( NULL == f ) {
		wflog("ERROR", "%s not found", filename);
		return get_last_iostat();
	}

	status = load_BMP24(f, &image, NULL);
	if ( IO_OK != status ){
		wflog("ERROR", "BMP loading error (io code %u)", status);
		return status;
	}

	wflog("INFO", "Loaded texture %s (%dx%d)", filename, image->width, image->height);

	*textureID = load_texture(image, params);

	free_image(image);
	return IO_OK;
}