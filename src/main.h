#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef struct
	{
		GLfloat camX;
		GLfloat camY;
		GLfloat fb;
		GLfloat rl;
		GLfloat ud;	
		GLfloat speedFB;
		GLfloat speedRL;
		GLfloat speedUD;
		GLfloat camX_sens;
		GLfloat camY_sens;
		GLfloat accFB;
		GLfloat accRL;
		GLfloat accUD;
	} movement_t ;

static int32_t handleEvents( movement_t* );
static void initOGL();
static GLuint getShaderProgram();
static SDL_Window* initWindow( uint32_t width, uint32_t height );
