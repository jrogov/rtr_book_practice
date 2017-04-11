#include "init.h"
#include "io/log.h"
#include "io/error_codes.h"

#include <GL/glew.h>
#include <GL/glu.h>



SDL_Window*
initWindow( unsigned int width, unsigned int height,
				const char* name,
				unsigned int posx, unsigned int posy)
{
	SDL_Window *window;
	SDL_GLContext context;
	GLenum glewError;

	#define FATAL(m, code) { wflog_fatal_error( m "(code %d)", code ); exit(1); }

	if ( 0 > SDL_Init( SDL_INIT_VIDEO )) FATAL( "SDL Init error" , ECODE_SDL_INIT);

	window = SDL_CreateWindow(name,
										posx,
										posy,
										width,
										height,
										SDL_WINDOW_SHOWN
											| SDL_WINDOW_OPENGL
											);

	if ( NULL == window ) FATAL( "Window Creation error", ECODE_SDL_WINDOW );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	context = SDL_GL_CreateContext( window );
	if ( NULL == context ) FATAL( "Context Creation error", ECODE_SDL_GL_CONTEXT );

	SDL_SetRelativeMouseMode( (SDL_bool) 1);

	glewExperimental = GL_TRUE;
	glewError = glewInit();
	glGetError();
	if ( GLEW_OK != glewError ) wflog_fatal_error("GLEW init error (code: %d; glewcode: %d)", ECODE_GLEW, glewError);

	if ( SDL_GL_SetSwapInterval( 1 ) < 0 ) FATAL( "Vsync Init Error", ECODE_SDL_VSYNC );

	return window;
}

