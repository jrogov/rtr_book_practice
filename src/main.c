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

#include "init.h"
#include "camera.h"

#include "io/shader.h"
#include "io/texture.h"
#include "io/model.h"
#include "io/log.h"

#include <err.h>

#include "config.h"

#define GET_UNIFORM(i, s)	if( (i = glGetUniformLocation( programID, s )) == -1) 			\
										wlog_fatal_error( "No " s " found in shader");
#define GET_ATTRIB(i, s) 	if( (i = glGetAttribLocation( programID, s )) == -1) 			\
										wlog_fatal_error( "No " s " found in shader");

int window_height = HEIGHT;
int window_width = WIDTH;


static int32_t handleEvents( movement_t* );
static void rotateCamera(GLuint programID, movement_t *player );


#define MODEL_PATH "res/obj/al_tr.obj"

int main( int argc, char* argv[] ){

	SDL_Window *window;
	int32_t events_status;
	IO_stat_t io_status;

	GLuint programID;

	obj_t model;

	init_log(NULL);

	window = initWindow(WIDTH, HEIGHT);

	io_status = fload_program(&shader_info, &programID);
	if( io_status != IO_OK)
		wlog_fatal_error("No suitable shader found");

	io_status = loadOBJ(MODEL_PATH, &model);
	if( io_status != IO_OK)
		printf("%s\n", str_ioerror(io_status));

	glUseProgram( programID );

	__debug_print_model_info(&model);

	glClearColor(0.5, 0, 0.5, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// glCullFace(GL_FRONT);
	glEnable(GL_MULTISAMPLE);


	#define MSAA_NUM 4
	GLuint tex;
	glGenTextures( 1, &tex );
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, MSAA_NUM, GL_RGBA8, 4*WIDTH, 4*HEIGHT, false );

	GLuint fbo;
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0 );

	glBindVertexArray(model.VAO);

	time_t prev, now;
	size_t counter;
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

		rotateCamera( programID, &player );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDrawArrays(GL_TRIANGLES, 0, model.verts_cnt);
		glFlush();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // Make sure no FBO is set as the draw framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); // Make sure your multisampled FBO is the read framebuffer
		glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		SDL_GL_SwapWindow( window );
		
		// SDL_Delay(16);
		counter++;
		time(&now);
		if(now - prev >=10){
			wflog("INFO", "FPS: %.2f", ((double) counter)/(now-prev));
			prev = now;
			counter = 0;
		}
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
	static GLint
			focus = 1;

	GLint xrel = 0,
			yrel = 0;

	player->fb = fabs(player->fb) < 0.01? 0: (player->fb + ( -2*(player->fb > 0) + 1 ) * 0.3 * player->accFB);
	player->rl = fabs(player->rl) < 0.01? 0: (player->rl + ( -2*(player->rl > 0) + 1 ) * 0.3 * player->accRL);
	player->ud = fabs(player->ud) < 0.01? 0: (player->ud + ( -2*(player->ud > 0) + 1 ) * 0.3 * player->accUD);

	while( SDL_PollEvent(&e) !=0 ){
		switch((e.type)){
			case SDL_QUIT:
				return -1;
			case SDL_MOUSEMOTION:
				if(focus)
				{
					xrel += e.motion.xrel;
					yrel += e.motion.yrel;
				}
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
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focus = 1;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						focus = 0;
						break;

				}
		}
	}


	player->camX -= player-> camX_sens * ((GLfloat) xrel)/WIDTH;

	//constrain to vertical motion to [-pi/2;pi/2]
	player->camY -= player-> camY_sens * ((GLfloat) yrel)/HEIGHT;
	player->camY = player->camY<3.14/2?player->camY:3.14/2;
	player->camY = player->camY>-3.14/2?player->camY:-3.14/2;

	player->fb = fmax(-player->speedFB, fmin(player->speedFB, player->fb + player->accFB * 2 * (f-b)));
	player->rl = fmax(-player->speedRL, fmin(player->speedRL, player->rl + player->accRL * 2 * (r-l)));
	player->ud = fmax(-player->speedUD, fmin(player->speedUD, player->ud + player->accUD * 2 * (u-d)));

	return update;
}


static void rotateCamera(GLuint programID, movement_t *player ){

	//static float sublast_ticks = ((float) clock());
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

	 /* Swap Y and -Z for human-intuitive way */
	Model = glm::mat4(1.0);
	
	View = glm::lookAt(
							Position,
							Position+Direction,
							realUp
	);

	Projection = glm::perspective(FoV, ( (GLfloat) WIDTH)/HEIGHT, 0.1f, 100.0f);


	MVP = Projection * View * Model;
	glm::mat4 ViewModel = View * Model;

	glUseProgram(programID);

	GLint eye_loc = glGetUniformLocation( programID, "W_Eye");
	glUniform3fv(eye_loc, 1, &(Position+Direction)[0]);


	#define SET_UMATRIX(name, data)	glUniformMatrix4fv( glGetUniformLocation( programID, name ) , 1, GL_FALSE, &((data))[0][0]);

	SET_UMATRIX("Model", Model);
	SET_UMATRIX("View", View);
	SET_UMATRIX("ViewModel", ViewModel)
	SET_UMATRIX("MVP", MVP)

	GLint dir_loc = glGetUniformLocation(programID, "W_LightDir");
	glUniform3f(dir_loc, 5*cos(angle), 5,  5*sin(angle));

}
