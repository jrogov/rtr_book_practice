#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include "init.h"
#include "camera.h"
#include "light.h"

#include "io/shader.h"
#include "io/texture.h"
#include "io/model.h"
#include "io/log.h"

#include <err.h>

#include "config.h"

SDL_Window *window;
unsigned int window_height = HEIGHT;
unsigned int window_width = WIDTH;
unsigned int window_posx = POSX;
unsigned int window_posy = POSY;


static int32_t handleEvents( movement_t* );
static void rotateCamera(GLuint programID, movement_t *player );

int main( int argc, char* argv[] ){

	int32_t events_status;
	IO_stat_t io_status;

	GLuint programID;

	obj_t model;

	init_log(NULL);

	#define REPORT(s, code) if ( io_status != IO_OK ) wflog_fatal_error( (s), code);

	window = initWindow(WIDTH, HEIGHT, MODEL_PATH, window_posx, window_posy);

	io_status = fload_program(&shader_info, &programID);
	REPORT("Program load failed: %u", ECODE_GL);

	io_status = loadOBJ(MODEL_PATH, &model);
	REPORT("Obj load failed: %u", ECODE_IO);

	glUseProgram( programID );

	glBindVertexArray(model.VAO);


	#define SET_UNIFORM(s,v) glUniform1i( glGetUniformLocation(programID, (s)), (v) )
	
	// GLuint normID = fload_BMP_texture("res/obj/pool_n.bmp");
	// SET_UNIFORM("normalSampler", 1);

	// GLuint texID = fload_BMP_texture("res/obj/pool_c.bmp");
	// SET_UNIFORM("textureSampler", 0);


	glClearColor(0.5, 0.5, 0.5, 1);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LESS); 

	

	glEnable(GL_MULTISAMPLE);
	#define MSAA_NUM 4
	enum e_framebuffer {FB_COLOR, FB_DEPTHSTENCIL};

	GLuint tex[2];
	glGenTextures( 2, tex );
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex[FB_COLOR] );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, MSAA_NUM, GL_RGBA8, WIDTH, HEIGHT, false );
	
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex[FB_DEPTHSTENCIL] );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, MSAA_NUM, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, false );


	GLuint fbo;
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex[FB_COLOR], 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, tex[FB_DEPTHSTENCIL], 0);


	time_t prev, last;
	time(&prev);
	size_t counter=0;
	while(1){
		

		events_status = handleEvents( &player );
		if ( -1 ==  events_status)
			return 0;
		if (events_status & 2){
			glDeleteProgram(programID);
			fload_program(&shader_info, &programID);
			//__debug_print_shader_cache__(stdout);
			glUseProgram(programID);
		}

		rotateCamera( programID, &player );

		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, texID);

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, normID);


		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDrawArrays(GL_TRIANGLES, 0, model.verts_cnt);
		glFlush();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 
								0, 0, WIDTH, HEIGHT, 
								GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, 
								GL_NEAREST);
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

		SDL_GL_SwapWindow( window );
		
		// SDL_Delay(16);
		counter++;
		time(&last);
		if(last - prev >=5){
			wflog("INFO", "FPS: %.2f", ((double) counter)/(last-prev));
			prev = last;
			counter = 0;
		}
	}

}

float
fclamp(float value, float min, float max)
{
	return fmax(min, fmin(max, value));
}

/* -1 - quit, 0 - no updates, 1 - player update, 2 - reload shader */
static int32_t handleEvents( movement_t* player ){



	SDL_Event e;
	GLuint update = 0;

	static GLint
			f = 0, b = 0,
			r = 0, l = 0,
			u = 0, d = 0,
			shift = 0;
	static GLint
			focus = 1;
	static uint32_t fullscreen = 0;

	GLint xrel = 0,
			yrel = 0;

	#define BRAKE_PLAYER(ax, acc) \
			player->ax = fabs(player->ax) < 0.01 ? \
									0: \
									(player->ax + ( -2*(player->ax > 0) + 1 ) * 0.3 * player->acc)

	BRAKE_PLAYER(fb, accFB);
	BRAKE_PLAYER(rl, accRL);
	BRAKE_PLAYER(ud, accUD);

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
					case SDLK_LSHIFT:
						shift = 1;
					case SDLK_F1:
						update |= 2;
						break;
					case SDLK_F11:
						fullscreen ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
						SDL_SetWindowFullscreen(window, fullscreen);
						break;
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
					case SDLK_LSHIFT:
						shift = 0;
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
	player->camY -= player-> camY_sens * ((GLfloat) yrel)/HEIGHT;
	player->camY = fclamp(player->camY, -3.14/2, 3.14/2);	

	player->fb = fclamp( player->fb + (shift+1) * player->accFB * 2 * (f-b), (shift+1) * -player->speedFB, (shift+1) * player->speedFB );
	player->rl = fclamp( player->rl + (shift+1) * player->accRL * 2 * (r-l), (shift+1) * -player->speedRL, (shift+1) * player->speedRL );
	player->ud = fclamp( player->ud + (shift+1) * player->accUD * 2 * (u-d), (shift+1) * -player->speedUD, (shift+1) * player->speedUD );
	
	return update;
}


static void rotateCamera(GLuint programID, movement_t *player ){

	static GLfloat light_param[2] = {0.0f, sqrt(2)/2};
	static glm::vec3 Up = glm::vec3(0, 1, 0);

	static struct timeval last;

	GLfloat verticalAngle, horizontalAngle;

	glm::vec3 *Position;
	glm::vec3 Direction;
	glm::vec3 Right;
	glm::mat4 Model, View, Projection;
	
	glm::mat4 ViewModel, MVP;


	#define MAX_UTIME_VALUE (1e9-1)
	#define UTIME(t) ( (GLfloat) ( t.tv_sec % 1000000 * 10000 )  + (GLfloat (t.tv_usec / 100) ) )

	/* calculate data */
	GLfloat unow = - (UTIME(last));
	// printf("%f\n", unow);
	gettimeofday(&last, NULL);
	unow += ( UTIME(last) );

	// GLfloat mult = 1e-8;
	light_param[0] += 0.5 * ((GLfloat) unow) * 3.14 * 2 * 1e5 / MAX_UTIME_VALUE;
	// printf("%f\n", light_param[0]);

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

	Position = (glm::vec3*)(player);

	*Position += glm::normalize(glm::cross(Up, Right)) *	(player->fb)
				 + Right * (player->rl)
				 + Up * (player->ud);

	View = glm::lookAt(
							*Position,
							*Position+Direction,
							Up
	);


	Projection = glm::perspective(FOV, ( (GLfloat) window_width)/window_height, 0.1f, 1000.0f);

	
	ViewModel = View * Model;
	MVP = Projection * ViewModel;

	
	/* send data to server */

	glUseProgram(programID);

	#define SET_UMAT4(name, data)	glUniformMatrix4fv( glGetUniformLocation( programID, name ) , 1, GL_FALSE, &((data))[0][0]);
	#define SET_UVEC3(name, data) glUniform3fv( glGetUniformLocation( programID, name ) , 1, &(data)[0]);

	SET_UMAT4("View", View);
	SET_UMAT4("ViewModel", ViewModel);
	SET_UMAT4("MVP", MVP);
	SET_UMAT4("Model", Model);

	light[0].mut_func(&light[0], light_param);

	// SET_UVEC3("W_Eye", Position+Direction);
	// glGetUnifomBlockIndex(programID, "Light");
	
	
	SET_UVEC3("W_LightDir", (const GLfloat*) &light[0].pos);

}
