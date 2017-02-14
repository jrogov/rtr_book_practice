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
#include "init.h"
#include "camera.h"

#include "io/shader.h"
#include "io/texture.h"
#include "io/model.h"
#include "io/log.h"

#include <err.h>

#include "config.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

static GLuint getShaderProgram();
static void print_movement(movement_t *m);
static void rotateCamera(GLuint programID, movement_t *player , int32_t player_update );


#define ELNUM 40000

model_t init_obj(){
	model_t model;
	IO_stat_t status;

	model.verts = (GLfloat*) malloc( (sizeof(GLfloat)+sizeof(GLint))*3*ELNUM);
	model.uvs = &(model.verts)[ELNUM];
	model.norms = &(model.verts)[2*ELNUM];

	model.vert_inds = (GLuint*) &(model.verts)[3*ELNUM];
	model.uv_inds = (GLuint*) &(model.vert_inds)[ELNUM];
	model.norm_inds = (GLuint*) &(model.vert_inds)[2*ELNUM];

	status = loadOBJ("res/obj/pumpkin_tr.obj", &model);
	printf("Status ID: %d\n", status);
	if (status != IO_OK)
	{
		exit(0);
	}

	return model;
}

int main( int argc, char* argv[] ){

	SDL_Window *window;
	int32_t events_status;
	IO_stat_t io_status;

	GLuint programID;
	GLuint b_vert;
	GLuint b_ind_vert;
	GLuint b_norm;
	GLuint b_ind_norm;
	GLuint VAO;
	// GLuint ub_vert;

	GLint vertexPos2DLocation;
	// GLint vertexUVLocation;
	// GLint samplerLocation;

	// GLuint brick_textID;

	model_t model;

	init_log(NULL);
	window = initWindow(WIDTH, HEIGHT);
	io_status = fload_program(&shader_info, &programID);
	__debug_print_shader_cache__(stdout);
	if( io_status != IO_OK )
		return 1;

	model = init_obj();

	/* VAO */
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	/* b_vert */
	glGenBuffers( 1, &b_vert );
	glBindBuffer( GL_ARRAY_BUFFER, b_vert );
	glBufferData( 	GL_ARRAY_BUFFER,
						model.verts_cnt * sizeof(model.verts[0]),
						model.verts,
						GL_STATIC_DRAW );

	glGenBuffers( 1, &b_norm );
	glBindBuffer( GL_ARRAY_BUFFER, b_norm );
	glBufferData( GL_ARRAY_BUFFER,
						model.norms_cnt * sizeof(model.norms[0]),
						model.norms,
						GL_STATIC_DRAW );

	/* b_ind_vert */
	glGenBuffers( 1, &b_ind_vert);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, b_ind_vert );
	glBufferData( 	GL_ELEMENT_ARRAY_BUFFER,
						model.vert_ind_cnt * sizeof(model.vert_inds[0]),
						model.vert_inds,
						GL_STATIC_DRAW );

	glGenBuffers(1, &b_ind_norm);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, b_ind_norm);
	glBufferData( 	GL_ELEMENT_ARRAY_BUFFER,
						model.norm_ind_cnt*sizeof(model.norm_inds[0]),
						model.norm_inds,
						GL_STATIC_DRAW);

	glClearColor(0.5, 0, 0.5, 1);
	glUseProgram( programID );

	#define GET_UNIFORM(i, s)	if( (i = glGetUniformLocation( programID, s )) == -1) 			\
											wlog_fatal_error( "No " s " found in shader");

	#define GET_ATTRIB(i, s) 	if( (i = glGetAttribLocation( programID, s )) == -1) 			\
											wlog_fatal_error( "No " s " found in shader");

	GET_ATTRIB(vertexPos2DLocation, "Normal");
	glEnableVertexAttribArray( vertexPos2DLocation );
	glBindBuffer(GL_ARRAY_BUFFER, b_norm);
	glVertexAttribPointer( vertexPos2DLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GET_ATTRIB(vertexPos2DLocation, "Pos");
	glEnableVertexAttribArray( vertexPos2DLocation );
	glBindBuffer(GL_ARRAY_BUFFER, b_vert);
	glVertexAttribPointer( vertexPos2DLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer( GL_ARRAY_BUFFER, b_vert );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, b_ind_vert );


	__debug_print_model_info(&model);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// glShadeModel(GL_FLAT);
	glEnable(GL_MULTISAMPLE);

	#define MSAA_NUM 3
	GLuint tex;
	glGenTextures( 1, &tex );
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, MSAA_NUM, GL_RGBA8, 4*WIDTH, 4*HEIGHT, false );

	GLuint fbo;
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0 );

	// GLenum status = glCheckFramebufferStatus( target );

	while(1){
		
		glBindFramebuffer( GL_FRAMEBUFFER, fbo );

		events_status = handleEvents( &player );
		if ( -1 ==  events_status)
			return 0;
		if (events_status & 2){
			glDeleteProgram(programID);
			fload_program(&shader_info, &programID);
			__debug_print_shader_cache__(stdout);
			glUseProgram(programID);
		}

		rotateCamera( programID, &player, 1 );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDrawElements( 	GL_TRIANGLES,
								model.vert_ind_cnt,
								GL_UNSIGNED_INT,
								NULL);

		glFlush();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // Make sure no FBO is set as the draw framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); // Make sure your multisampled FBO is the read framebuffer
		glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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

	//constrain to vertical motion to [-pi/2;pi/2
	player->camY -= player-> camY_sens * ((GLfloat) yrel)/HEIGHT;
	player->camY = player->camY<3.14/2?player->camY:3.14/2;
	player->camY = player->camY>-3.14/2?player->camY:-3.14/2;

	player->fb = fmax(-player->speedFB, fmin(player->speedFB, player->fb + player->accFB * 2 * (f-b)));
	player->rl = fmax(-player->speedRL, fmin(player->speedRL, player->rl + player->accRL * 2 * (r-l)));
	player->ud = fmax(-player->speedUD, fmin(player->speedUD, player->ud + player->accUD * 2 * (u-d)));

	return update;
}


static void rotateCamera(GLuint programID, movement_t *player , int32_t player_update ){

	//static float last_ticks = ((float) clock());
	//float current_ticks;
	//float cycle_time;
	//current_ticks = ((float) clock());
	//cycle_time = (current_ticks- last_ticks)/CLOCKS_PER_SEC;

	//static GLfloat mousespeed = 2.5f;
	static GLfloat angle = 0.0f;


	GLfloat verticalAngle, horizontalAngle;

	static GLfloat FoV = FOV;

	static glm::vec3 Position = glm::vec3( 0, 4, 5 );
	static glm::vec3 realUp = glm::vec3(0, 1, 0);

	glm::vec3 Up;
	glm::vec3 Direction;
	glm::vec3 Right;

	glm::mat4 Model, View, Projection;
	glm::mat4 MVP;


	angle+=0.03;

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

	Position += glm::normalize(glm::cross(realUp, Right)) *	(player->fb);
	Position += Right * 		(player->rl);
	Position += realUp * 			(player->ud);

	player->fb = fabs(player->fb) < 0.02? 0: (player->fb + (player->fb > 0? -1 : 1) * 0.3 * player->accFB);
	player->rl = fabs(player->rl) < 0.02? 0: (player->rl + (player->rl > 0? -1 : 1) * 0.3 * player->accRL);
	player->ud = fabs(player->ud) < 0.02? 0: (player->ud + (player->ud > 0? -1 : 1) * 0.3 * player->accUD);

	 /* Swap Y and -Z for human-intuitive way */
	Model = glm::mat4(0.0);
	Model[0][0] = 1.0;
	Model[1][1] = 1.0;
	Model[2][2] = -1.0;
	Model[3][3] = 1.0;

	View = glm::lookAt(
							Position,
							Position+Direction,
							realUp
	);

	Projection = glm::perspective(FoV, ( (GLfloat) WIDTH)/HEIGHT, 0.1f, 100.0f);


	MVP = Projection * View * Model;

	glUseProgram(programID);

	GLint eye_loc = glGetUniformLocation( programID, "Eyecam");
	glUniform3fv(eye_loc, 1, &(Position+Direction)[0]);

	glm::mat4 ViewModel = View * Model;
	GLint m_loc = glGetUniformLocation( programID, "Model" );
	glUniformMatrix4fv(m_loc, 1, GL_FALSE, &(Model)[0][0]);
	GLint vm_loc = glGetUniformLocation( programID, "ViewModel" );
	glUniformMatrix4fv(vm_loc, 1, GL_FALSE, &(ViewModel)[0][0]);
	GLint mvp_loc = glGetUniformLocation( programID, "MVP" );
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &(Projection * ViewModel)[0][0]);

	GLint dir_loc = glGetUniformLocation(programID, "Dir");
	// glUniform3fv(dir_loc, 1, &Direction[0]);
	glUniform3f(dir_loc, 5 + 5*cos(angle), 5, 5 + 5*sin(angle));

}
