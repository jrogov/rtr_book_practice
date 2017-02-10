#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include "main.h"
#include "io/shader.h"
#include "io/texture.h"
#include "io/model.h"
#include "io/log.h"

#include <err.h>

#define ERRX_SDL(code, s) 				errx( code, s ": %s\n", SDL_GetError( ))
#define ERRX_GLEW(code, s, error) 	errx( code, s ": %s\n", glewGetErrorString( error ));
#define ERRX_GL(code,s)

#include "config.h"

#define STRINGIFY2(x) #x 
#define STRINGIFY(x) STRINGIFY2(x)

static GLuint getShaderProgram();
static void initWindow( );
static SDL_Window* initWindow( uint32_t width, uint32_t height );
static void print_movement(movement_t *m);
static void rotateCamera(GLuint programID, movement_t *player , int32_t player_update );


#define ELNUM 15000


obj_t init_obj(){
	obj_t object;
	IO_stat_t status;

	GLfloat *vertices, *uvs, *normals;
	GLuint *vert_indices, *uv_indices, *norm_indices;

	object.vertices = (GLfloat*) malloc( (sizeof(GLfloat)+sizeof(GLint))*3*ELNUM);
	object.uvs = &(object.vertices)[ELNUM];
	object.normals = &(object.vertices)[2*ELNUM];

	object.vert_indices = (GLuint*) &(object.vertices)[3*ELNUM];
	object.uv_indices = (GLuint*) &(object.vert_indices)[ELNUM];
	object.norm_indices = (GLuint*) &(object.vert_indices)[2*ELNUM];

	status = loadOBJ("res/obj/cube.obj", &object);
	printf("Status ID: %d\n", status);
	if (status != IO_OK)
	{
		exit(0);
	}

	return object;
}

int main( int argc, char* argv[] ){
	
	SDL_Window *window;
	int32_t events_status;

	GLuint programID; 
	GLuint VBO; 
	GLuint IBO;
	GLuint VAO;
	GLuint uvBO;

	GLint vertexPos2DLocation;
	GLint vertexUVLocation;
	GLint samplerLocation;

	GLuint brick_textID;
	
	obj_t object;

	init_log(NULL);
	window = initWindow(WIDTH, HEIGHT);
	programID = getShaderProgram();
	object = init_obj();

	/* VAO */
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	/* VBO */
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( 	GL_ARRAY_BUFFER, 
						object.vertices_count * sizeof(object.vertices[0]), 
						object.vertices, 
						GL_STATIC_DRAW );

	/* IBO */
	glGenBuffers( 1, &IBO);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
	glBufferData( 	GL_ELEMENT_ARRAY_BUFFER, 
						object.vert_indices_count * sizeof(object.vert_indices[0]), 
						object.vert_indices, 
						GL_STATIC_DRAW );

	glClearColor(0.5, 0, 0.5, 1);
	glUseProgram( programID );
	
	#define GET_UNIFORM(i, s)	if( (i = glGetUniformLocation( programID, s )) == -1) 			\
											ERRX_GL( 1, "No " s " found in shader");

	#define GET_ATTRIB(i, s) 	if( (i = glGetAttribLocation( programID, s )) == -1) 			\
											ERRX_GL( 1, "No " s " found in shader");					

	GET_ATTRIB(vertexPos2DLocation, "Pos");	
	glEnableVertexAttribArray( vertexPos2DLocation );
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer( vertexPos2DLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	//glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	while(1){

		events_status = handleEvents( &player );
		if ( -1 ==  events_status)
			return 0;
		if (events_status & 2){
			glDeleteProgram(programID);
			programID = getShaderProgram();
			glUseProgram(programID);
		}
		
		rotateCamera( programID, &player, 1 );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDrawElements( 	GL_TRIANGLES, 
								3*(object.vert_indices_count) , //sizeof(testLevelIndex)/sizeof(testLevelIndex[0]), 
								GL_UNSIGNED_INT, 
								NULL);
								
		glFlush();

		SDL_GL_SwapWindow( window );
		SDL_Delay(16);
	}

}

/* -1 - quit, 0 - no updates, 1 - player update, 2 - reload shader */
static int32_t handleEvents( movement_t* player ){

	SDL_Event e;
	GLuint update = 0;

	static GLint 
			f = 0, b = 0,
			r = 0, l = 0,
			u = 0, d = 0;

	GLint xrel = 0, 
			yrel = 0;

	
	while( SDL_PollEvent(&e) !=0 ){
		switch((e.type)){
			case SDL_QUIT: 
				return -1;
			case SDL_MOUSEMOTION:
				xrel += e.motion.xrel;
				yrel += e.motion.yrel;
				update = 1;
				break;
			case SDL_KEYDOWN:
				if(SDL_PRESSED != e.key.state) break;

				switch(e.key.keysym.sym){
					case SDLK_w:
						f = 1; break;
					case SDLK_s:
						b = 1; break;
					case SDLK_d:
						r = 1; break;
					case SDLK_a:
						l = 1; break;
					case SDLK_SPACE:
						u = 1; break;
					case SDLK_LCTRL:
						d = 1; break;
					case SDLK_F1:
						update |= 2;
				}
				update |= 1;
				break;

			case SDL_KEYUP:
				if(SDL_RELEASED != e.key.state) break;

				switch(e.key.keysym.sym){
					case SDLK_w:
						f = 0; break;
					case SDLK_s:
						b = 0; break;
					case SDLK_a:
						l = 0; break;
					case SDLK_d:
						r = 0; break;
					case SDLK_SPACE:
						u = 0; break;
					case SDLK_LCTRL:
						d = 0; break;
				}
				update |= 1;
				break;
		}
	}


	player->camX -= player-> camX_sens * ((GLfloat) xrel)/WIDTH;
	player->camY -= player-> camY_sens * ((GLfloat) yrel)/HEIGHT;
	player->fb = fmax(-player->speedFB, fmin(player->speedFB, player->fb + player->accFB * 2 * (f-b)));
	player->rl = fmax(-player->speedRL, fmin(player->speedRL, player->rl + player->accRL * 2 * (r-l)));
	player->ud = fmax(-player->speedUD, fmin(player->speedUD, player->ud + player->accUD * 2 * (u-d)));
	
	/*printf("\nAfter update\n");
	print_movement(player);
*/
	return update;
}


static void rotateCamera(GLuint programID, movement_t *player , int32_t player_update ){

	//static float last_ticks = ((float) clock());
	//float current_ticks;
	//float cycle_time;

	static GLfloat verticalAngle=0.0f, horizontalAngle=3.14f;
	//static GLfloat mousespeed = 2.5f;

	static GLfloat FoV = FOV;
	
	static glm::vec3 Position = glm::vec3( 0, 4, 5 );

	glm::vec3 Up;
	glm::vec3 Direction;
	glm::vec3 Right;

	glm::mat4 Model, View, Projection;
	glm::mat4 MVP;
	static glm::vec3 realUp = glm::vec3(0, 1, 0);

	glUseProgram(programID);

	//current_ticks = ((float) clock());
	//cycle_time = (current_ticks- last_ticks)/CLOCKS_PER_SEC;


	/* ??? */
	horizontalAngle = player->camX ;
	verticalAngle 	= 	player->camY ;

	Direction = glm::vec3(
								cos(verticalAngle) * sin(horizontalAngle),
								sin(verticalAngle),
								cos(verticalAngle) * cos(horizontalAngle)
	);

	Right = glm::vec3(
					sin(horizontalAngle - 3.14f/2.0f),
	0,
	cos(horizontalAngle - 3.14f/2.0f)
	);
	Up = glm::cross( Right, Direction );

	Position += Direction *	(player->fb);
	Position += Right * 		(player->rl);
	Position += realUp * 			(player->ud);

	player->fb = fabs(player->fb) < 0.02? 0: (player->fb + (player->fb > 0? -1 : 1) * 0.3 * player->accFB);
	player->rl = fabs(player->rl) < 0.02? 0: (player->rl + (player->rl > 0? -1 : 1) * 0.3 * player->accRL);
	player->ud = fabs(player->ud) < 0.02? 0: (player->ud + (player->ud > 0? -1 : 1) * 0.3 * player->accUD);

	Projection = glm::perspective(FoV, ( (GLfloat) WIDTH)/HEIGHT, 0.1f, 100.0f);
	View = glm::lookAt(
							Position,		   
							Position+Direction, 
							Up				  
	);		

	MVP = Projection * View * Model;
	GLint mvp_loc = glGetUniformLocation( programID, "MVP" );
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &MVP[0][0]);

}

static void print_movement(movement_t *m){
	printf(
		"\ttCamX\tCamY\n"
		"\t%.2f\t%.2f\n"
		"\tF_B\tR_L\tU_D\n"
		"\t%.2f\t%.2f\t%.2f\n",
		m->camX, m->camY,
		m->fb, m->rl, m->ud
		);		
}

static GLuint getShaderProgram(){

	static char log_buffer[200];
	GLuint programID; 
	IO_stat_t io_status;

	io_status = fload_program(
					"shaders/simple.vs.glsl",
					"shaders/simple.fs.glsl",
					"shaders/simple.gs.glsl",
					NULL, NULL,
					&programID
				); 

	fprintf(stdout, "[SHADER CACHE]\n");
	__debug_print_shader_cache__(stdout);

	if( IO_OK != io_status){
		glGetProgramInfoLog( programID, sizeof(log_buffer)/sizeof(log_buffer[0]), NULL, log_buffer);
		errx(1, "shader loading error %d\n%s", io_status, log_buffer);
	}
	return programID;
}

static
SDL_Window*
initWindow( uint32_t width, uint32_t height )
{
	SDL_Window *window;
	SDL_GLContext context; 
	GLenum glewError;


	if ( 0 > SDL_Init( SDL_INIT_VIDEO )) ERRX_SDL( 1, "SDL Init Error" );

	window = SDL_CreateWindow("Cuke'n'Die", 
										SDL_WINDOWPOS_UNDEFINED, 
										SDL_WINDOWPOS_UNDEFINED,
										width,
										height,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if ( NULL == window ) ERRX_SDL( 1, "Window Creation Error" );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 ); 
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 ); 
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	context = SDL_GL_CreateContext( window ); 
	if ( NULL == context ) ERRX_SDL( 3, "Context Creation Error" );

	SDL_SetRelativeMouseMode( (SDL_bool) 1); /* remove cast??? */

	glewExperimental = GL_TRUE;
	glewError = glewInit( );
	if ( GLEW_OK != glewError ) ERRX_GLEW( 2, "GLEW Init error", glewError );

	if ( SDL_GL_SetSwapInterval( 1 ) < 0 ) ERRX_SDL( 3, "Vsync Init Error" );

	return window;
}

